# ICUM 工程说明

本文面向接手 RH850/U2C4 演示工程的开发人员，描述当前工作区的 ICUM 固件；PE 调用和串口测试见 [PE 工程说明](PE_PROJECT_GUIDE.md)，测试密钥、输入与预期值见 [算法测试输入与预期结果](ALGORITHM_TEST_VECTORS.md)。**本文是源码与当前构建映射的说明，不代表目标板所有算法已经验证通过。** 地址以此次 `icup_B` 构建为例，重新编译后应检查 map；区间均为左闭右开。

## 1. 工程简介与构建

ICUM 是安全协处理器固件，接收 PE 放入共享 RAM 的服务描述符（ISD），将任务分发给 AES、RSA、随机数、ECC 和 HASH 等服务，并通过响应队列和中断通知 PE。PE 不直接获取本演示的 ICUM 内部密钥；请求中的密钥索引由 ICUM 密钥管理模块解析，部分字段还会被分发器覆盖。

构建入口为 [top.U2C.gpj](top/u2c/top.U2C.gpj)，[U2C_firm.gpj](top/u2c/U2C_firm.gpj) 将 ICUM、PE1、PE2 和 shared 组成多核镜像。当前配置为 U2C4、`cfg_build_fw=1`、调试版和测试工程，使用 Green Hills 工具链；设备头文件和 RFD 路径是工程配置里的本机路径，换机器需先检查这些路径。

```powershell
gbuild -top top\u2c\top.U2C.gpj
```

常用结果为 `icum\target\u2cx\build\out\icup_B`、同目录 `icup_B.map`，以及多核构建生成的固件和 S-record/HEX。编译成功不等于已烧录或已完成板级验证。CPU 320 MHz、CLK_SBUS 160 MHz 依赖已编程的 Option Bytes，详见 [README](README.md)。

## 2. 目录与代码分层

| 位置 | 职责 |
|---|---|
| [icum/icup_B.gpj](icum/icup_B.gpj) | ICUM 构建入口 |
| [icum/generic/src/kernel/k_issd](icum/generic/src/kernel/k_issd) | 状态机、任务队列、优先级、服务分发与响应 |
| [icum/generic/src/service](icum/generic/src/service) | 各算法的服务参数检查、资源申请和驱动调用 |
| [icum/generic/src/middle](icum/generic/src/middle) | 密钥、配置、上下文、资源仲裁、系统信息和数据集管理 |
| [icum/sal](icum/sal) | 算法及平台相关库接口 |
| [icum/target/u2cx/src/driver](icum/target/u2cx/src/driver) | 通信、AES、Flash、中断等 U2C4 驱动 |
| [pe/generic/icumif](pe/generic/icumif) | PE 与 ICUM 共用的服务 ID 和参数结构定义 |
| [icum/target/u2cx/config/u2c/icup_B.ld](icum/target/u2cx/config/u2c/icup_B.ld) | ICUM 链接布局 |

```text
PE ISD + 数据（Cluster RAM）
        │ 请求队列 / 中断
        ▼
D_COMM 驱动 → K_ISSD 请求处理 / Job 队列 / 服务分发
        → S_xx 服务 → MH_KEY / ML_CTX / ML_ABTR
                    → SAL / D_AES、PKCC、HASH、RNG 等驱动
        → 回写 ISD / 输出缓冲区 → 响应队列 / 中断 → PE
```

服务**分组号**与源文件函数名前缀不完全相同：例如 RSA 分在 service_02，但部分函数仍叫 `S_01_*`；随机数分在 service_03，但函数仍叫 `S_02_RAND_*`。以 [分发表](icum/generic/src/kernel/k_issd/icum_k_issd_dispatch_tables.c)为准，不应从文件名推断 ID。

## 3. Code Flash、Data Flash 和 RAM

### 3.1 链接器定义的区域

| 资源 | 起始 | 结束（不含） | 说明 |
|---|---:|---:|---|
| Code Flash Bank A | `0x00000000` | `0x00200000` | ICUM 链接脚本的 Side A 地址空间；PE 也使用 Bank A，不能理解为全区域独占 |
| Code Flash Bank B | `0x00400000` | `0x00600000` | ICUM Side B 地址空间；PE2 也使用 Bank B |
| ICUM Side B 启动保留区 | `0x00590000` | `0x005A0000` | 本次 `icup_B` 启动区，64 KiB |
| ICUM Side B 固件保留区 | `0x005A0000` | `0x00600000` | 6 × 64 KiB；实际已用到哪里应查看 map |
| ICUM Side A 固件保留区 | `0x001A0000` | `0x00200000` | 链接符号定义的对应 Side A 固件区，不表示本次已放置 Side A 镜像 |
| ICUM Local RAM | `0xFEDE0000` | `0xFEE00000` | 128 KiB；代码搬运区、数据、密钥、配置区与栈 |
| Cluster RAM 0 | `0xFE000000` | `0xFE070000` | 448 KiB，可用于 PE/ICUM 共享；具体权限和 PE 子区见 PE 文档 |
| Data Flash Area 0 | `0xFF200000` | `0xFF220000` | 128 KiB，链接器声明的硬件区域 |
| Data Flash Area 1 | `0xFF220000` | `0xFF240000` | 128 KiB，链接器声明的硬件区域 |
| ICUM Data Flash Area 2 | `0xFF280000` | `0xFF290000` | 64 KiB，链接器声明的硬件区域 |

Code Flash 定义了 64 KiB 启动块及 6 个 64 KiB 固件块；实际布局见 [icup_B.map](icum/target/u2cx/build/out/icup_B.map)。本次 map 中固件内容从 `0x005A0000` 到 `0x005E7B9C`，此数字会随构建变化。链接器的区域声明并不等于实际烧录内容或运行时访问权限。

### 3.2 当前数据集后端：RAM，不是持久 Data Flash

虽然链接脚本和底层 Flash 驱动仍定义真实 Data Flash，当前 [mm_fls.gpj](icum/generic/src/middle/mm_fls/mm_fls.gpj) 编译的是 [icum_mm_fls_ram.c](icum/generic/src/middle/mm_fls/icum_mm_fls_ram.c)，而不是旧的 `icum_mm_fls_mng_main.c`。因此当前 `MM_FLS_Init/Read/Write/Erase` 的安全数据集由 RAM 数组及槽表模拟：

- `MM_FLS_Init()` 清空模拟数据，**复位或重新初始化后内容不会持久保留**。
- 当前 `g_ram_fls_data` 位于 `0xFEDE7304`，大小 `0xF000`（60 KiB），区间 `[0xFEDE7304, 0xFEDF6304)`；其后还有槽表。以当前 map 为准。
- 代码有 RAM 数据压实 `ram_FLS_Compact()`；这不是对真实 Data Flash 的自动垃圾回收。当前后端中的 Code Flash 写入等接口返回不可用。
- 旧 Flash 管理源码、数据集声明、容量 API 和底层驱动仍在仓库中，**不能仅因文件存在就认为当前固件在写真实 Data Flash**。真实擦写路径、保护和持久化需要单独集成和验证。

### 3.3 ICUM Local RAM 与密钥区域

本次 [链接映射](icum/target/u2cx/build/out/icup_B.map)中，`.icum_middle.data` 从 `0xFEDE5008` 开始，`.icum_middle.bss` 从 `0xFEDE5C70` 开始；配置 RAM 为 `[0xFEDFABA8, 0xFEDFBFA8)`，大小 5 KiB；栈为 `[0xFEDFBFA8, 0xFEE00000)`。配置区地址不是永久 ABI，修改代码或工具链后会移动。

下面是在 `ML_CFG_GetKeyMappingData()` 成功执行、默认 VM 数量为 1 时，由本次 map 和目标编译器实际槽大小推算的布局：

```text
高地址在下；地址均为区间起点
0xFEDFABA8 ┌──────────────────────────────────────────┐
           │ AES RAM：5 × 52 B                       │
           │ ID 0/1/2/3：演示密钥；ID 4：ECDH 输出槽 │
0xFEDFACAC ├──────────────────────────────────────────┤
           │ AES buffer：0 槽，不占空间              │
0xFEDFACAC ├──────────────────────────────────────────┤
           │ ECC RAM：2 × 560 B，ID 0 P-256、ID 1 Ed25519 │
0xFEDFB10C ├──────────────────────────────────────────┤
           │ HMAC：1 × 80 B                          │
0xFEDFB15C ├──────────────────────────────────────────┤
           │ TLS master：0 槽；剩余 3660 B          │
0xFEDFBFA8 └──────────────────────────────────────────┘
```

5 个 `g_cfg_*_start_addr` 在 [定义处](icum/generic/src/middle/ml_cfg/icum_ml_cfg.c)都先赋相同的配置 RAM 基址；[ML_CFG_GetKeyMappingData](icum/generic/src/middle/ml_cfg/icum_ml_cfg.c) 后续按 4 字节对齐、槽大小和数量顺次更新它们，并检查末端。零槽区域与下一段同起点是正常的。上图是**分配结果**，不保证每个槽都已成功初始化；见第 10 节的已知问题。

`MH_KEY_*_DATA_SIZE` 是 `sizeof` 表达式，源码注释中的字节数可能过时；本次实际 AES=52、ECC=560、HMAC=80，可从 [编译列表](icum/target/u2cx/build/middle/obj/libs/icum_ml_cfg/icum_ml_cfg.lst)核对。默认槽数量在 [icum_ml_cfg.h](icum/generic/src/middle/ml_cfg/icum_ml_cfg.h)。

RSA 演示密钥数组不在配置 RAM 中：本次 map 中模数、私钥指数、公钥指数数组分别起于 `0xFEDE50A0`、`0xFEDE52A0`、`0xFEDE5090`；它们位于 `.icum_middle.data`，生成密钥会更新对应 RAM 内容。AES/ECC/HMAC 演示源数组也位于初始化数据区，并被复制到各工作槽。

## 4. 启动和服务生命周期

1. ICUM 进入 `ISSD_MODE_START`，调用 `MH_INIT_Init()`；它初始化通信、资源、上下文、Flash 管理和配置。
2. `ML_CFG_Init()` 填默认配置；`MH_SYSINFO_Init()` 经 `MM_FLS_Read()` 读取当前数据集后端并调用 `ML_CFG_SetIndexMappingVM()`。当前 RAM 后端在初始化时为空，因此通常使用默认值。
3. `MH_KEY_Init()` 顺序初始化 AES、RSA、ECC、HMAC 和 master secret；其返回值目前在启动调用处被忽略，详见第 10 节。
4. `k_ISSD_Run()` 在启动阶段还会检查数据集和设置映射，之后进入正常分发模式并设置服务可用状态。代码见 [icum_k_issd_modes.c](icum/generic/src/kernel/k_issd/icum_k_issd_modes.c)。
5. PE 提交请求后，通信驱动检查共享地址，取出 ISD 并调用 `K_ISSD_HandleRequest()`；它检查请求者和服务、按当前策略修改密钥选择，加入任务队列。
6. 调度器调用 `void S_xxx(icum_job_t *p_job)`。服务检查参数和地址、申请资源/上下文、执行算法，更新 ISD 结果及输出；完成后响应队列入队并通知对应 PE。

服务处理可能涉及中间态、上下文、硬件回调和资源仲裁；`SERV_REQUEST`/`SERV_IN_PROCESS` 不是最终成功。PE 必须等响应并检查 `service_result` 和算法专用验证结果。

## 5. 密钥管理

[MH_KEY_Init](icum/generic/src/middle/mh_key/icum_mh_key_common.c) 调用各类初始化函数；`MH_KEY_GetAesKey` 等按照 VM、组、ID、可用性及使用计数访问工作槽。密钥组、属性与 ID 类型在 [icum_mh_key_pub.h](icum/generic/include/middle/icum_mh_key_pub.h)，演示材料在对应的 `icum_mh_key_*.c`。

| 密钥 | 当前策略 |
|---|---|
| AES RAM 0/1/2 | 初始化为 128/192/256 位演示密钥；非 XTS AES 的组被强制为 RAM，ID 保留请求值 |
| AES RAM 3 | XTS 第二把 128 位密钥；XTS 强制使用 ID 0 和 3 |
| AES RAM 4 | 为 ECDH 派生结果预留，初始不作为已加载演示密钥 |
| RSA 0 | 公私钥数据来自 RAM 演示数组；签名、验签、加解密和生成均强制 ID 0 |
| ECC RAM 0 | P-256 演示密钥；ECDSA、ECDH 和 ECC 生成使用 ID 0 |
| ECC RAM 1 | Ed25519 演示密钥；EdDSA 使用 ID 1 |
| HMAC 0 | 代码意图使用 RAM 演示密钥；当前初始化风险见第 10 节 |

覆盖规则在 [k_ISSD_SetDemoKeySelection](icum/generic/src/kernel/k_issd/icum_k_issd_dispatch.c)；PE 指定的密钥组/ID **不一定被采纳**。ECDH 当前将同一 ECC 0 的私钥和公钥作为输入，并将结果写入 AES RAM 4；这只是演示，并不是两个独立参与方的密钥交换验证。RSA/ECC 密钥生成会改变各自的 0 号槽，影响后续测试。

公开分发表不提供旧的注册、导入、更新服务；这不意味着所有旧实现文件均已删除，也不意味着算法内部不会产生、复制或使用临时密钥。演示密钥材料和初始化数据可从镜像中恢复，**不能用于生产密钥或安全性评估**。

## 6. 算法接口与参数

PE 的统一调用入口是 `R_ICUMIF_ServiceRequest(r_icumif_isd_t *)`；下表是 ICUM 内部实际处理入口，签名均为 `void function(icum_job_t *p_job)`。服务 ID 的高半字节是组号、低半字节是组内索引；可用性还取决于分发表和运行模式。

| ID | ISD `prm` 成员 | ICUM 服务入口 | 功能与主要参数 |
|---:|---|---|---|
| `0x10` | `AES_CIPHER` | `S_01_CipherAes` | ECB/CBC/CFB/OFB/CTR/XTS；方向、模式、RAM key ID、IV、输入/输出；普通模式按块数，XTS 按 bit 长度 |
| `0x11` | `AES_AUTH_CIPHER` | `S_01_AuthenticateAes` | GCM/CCM；IV、AAD、数据、tag 及各自 bit 长度，解密检查 `verification_result` |
| `0x12` | `AES_CMAC` | `S_01_ProcessAesCmac` | CMAC 生成/验证；消息、MAC、bit 长度、key ID、操作类型 |
| `0x20` | `RSA_VERIFY` | `S_01_VerifyRsaSignature` | PSS/PKCS#1 v1.5 验签；消息、签名、哈希、编码、盐长度、验证结果 |
| `0x21` | `RSA_SIGN` | `S_01_MakeRsaSignature` | RSA 签名；输入消息、输出签名及尺寸、哈希、编码、盐长度 |
| `0x22` | `RSA_ENCRYPT` | `S_01_EncryptByRsa` | OAEP/PKCS#1 v1.5 加密；消息、输出、编码、哈希、可选 label |
| `0x23` | `RSA_DECRYPT` | `S_01_DecryptByRsa` | RSA 解密；密文输入、明文输出及容量/结果长度 |
| `0x24` | `RSA_KEY_GENERATE` | `S_03_GenerateRsaKey` | 指定模数 bit 长度、密钥 ID 和标志；本演示强制 ID 0 |
| `0x30` | `RAND_INIT` | `S_02_RAND_InitRng` | 初始化随机数源及在线测试选择 |
| `0x31` | `RAND_EXTEND_SEED` | `S_02_RAND_ExtendSeed` | 输入熵数据指针 |
| `0x32` | `RAND_GENERATE` | `S_02_RAND_GenerateRnd` | 输出缓冲区、块数和 job slice |
| `0x40`/`0x41` | `ECDSA_VERIFY`/`ECDSA_SIGN` | `S_01_VerifyEcdsaSignature`/`S_01_MakeEcdsaSignature` | 消息、两个签名分量、各自 bit 长度、哈希和密钥；验证时检查结果 |
| `0x42` | `ECDH` | `S_01_MakeEcdhSecretKey` | 公私钥组与 ID、输出密钥组/ID、KDF、哈希、info 和派生长度 |
| `0x43`/`0x44` | `EDDSA_VERIFY`/`EDDSA_SIGN` | `S_01_VerifyEddsaSignature`/`S_01_MakeEddsaSignature` | EdDSA 签名和验证；参数形态与相应头文件一致 |
| `0x45` | `ECC_KEY_GENERATE` | `S_03_GenerateEccKey` | 曲线、密钥组/ID、属性标志；演示固定 RAM 0 |
| `0x50` | `HASH` | `S_01_GenerateHash` | 消息、输出摘要、哈希类型、字节长度、分段作业信息 |
| `0x51` | `HMAC` | `S_01_ProcessHmac` | 哈希类型、消息、MAC、字节长度、生成/验证及验证结果 |

准确的字段类型、可选值、输入/输出语义和限制见 [r_icumif_api.h](pe/generic/icumif/r_icumif_api.h) 及其包含的 `r_icumif_api_*.h`，运行时限制应同时检查对应服务代码。表中的模式是接口/菜单涉及的模式，**不是声称所有模式均已在板上验证**。`SERVICE_20_CR` 是表中的内部项，不是 PE 公共算法菜单项。

## 7. 关键结构

| 结构 | 作用 |
|---|---|
| `r_icumif_isd_t` | 跨核共享描述符：服务 ID、请求者、VM、中断通道、请求/响应状态、优先级、服务结果、job ID、回调及 `prm` 联合体 |
| `icum_job_t` | ICUM 内部任务：指向 ISD，保存请求者、VM、优先级、阶段、取消状态和各算法私有 job 参数；不应作为 PE/ICUM 的共享 ABI |
| `service_t` | 分发表项，包含普通/RAM 执行函数、高优先级函数和许可掩码 |
| `cfg_sys_t` / `sys_cfg_key_mapping_t` | 系统及 VM 密钥槽配置，供配置管理和初始化使用 |
| `aes_key_t`、`ecc_key_t`、`hmac_key_t` | 密钥内容、属性、可用性及内部指针/计数；所在位置由配置 RAM 映射决定 |
| `ram_fls_slot_t` | RAM 数据集后端中的偏移、大小、数据集类型、槽号和使用标记 |

`r_icumif_isd_t` 的 `prm` 是联合体，必须按 `service_id` 选择同名成员，不能同时保存多种服务参数。指针联合体中的 `padding_sys64` 用于布局；PE 与 ICUM 必须使用匹配的头文件、编译配置和地址约定。

## 8. 核间通信和共享内存

```text
PE                                 ICUM
准备 ISD/数据（Cluster RAM）
R_ICUMIF_ServiceRequest
  请求队列入队，状态=1 ──中断────→ D_COMM 取请求，状态=2
                                    K_ISSD 建 job / 算法执行
等待中断                             写回结果
取响应，状态=4  ←────中断──────── 响应队列入队，状态=3
读取 service_result、输出和验证结果
```

请求队列及响应队列都存放 ISD **地址**，算法数据也经 ISD 指针引用共享缓冲区。ISD、输入、输出在整个异步生命周期内必须保持有效，不能过早复用。当前 U2C4 PE0 的普通响应中断为 INTC2 236，PE1 为 540；PE 工程名 `pe1`/`pe2` 与硬件 PE0/PE1 的对应见 [PE 工程说明](PE_PROJECT_GUIDE.md)。非普通单独中断路径不应与此混淆。

通信检查范围等由 [icum_d_comm.c](icum/target/u2cx/src/driver/d_comm/icum_d_comm.c)、[icum_d_flash.h](icum/target/u2cx/src/driver/d_fls/icum_d_flash.h) 和具体算法校验决定；当前 Cluster RAM 上界使用 `__ghsbegin_end_gRAM_BANK_A`。PE 端队列区存在重叠，见下一节和 PE 文档，不应将当前布局当作已验证的并发设计。

## 9. 测试与排查

ICUM 本身没有一套独立的串口算法测试；实际测试入口是 PE 的 [pe1_crypto_service_test.c](pe/generic/src/pe1/src/pe1_crypto_service_test.c)。该文件对 AES 的 8 种加密模式比较固定密文，GCM/CCM 也比较 tag，再进行解密；其他测试的检查强度见 PE 文档。

调试建议：

1. 检查 Option Bytes、时钟、ICUM 初始化结果和 `SERVAVAIL`。
2. 在 `ML_CFG_SetIndexMappingVM` 返回后观察各 `g_cfg_*_start_addr`、默认槽数和映射返回值；不要把变量本身的地址当作其保存的起始地址。
3. 请求无返回时依次查看 `req_res_status` 1→2→3→4、`service_result`、请求/响应队列及对应中断标志；状态 3 表示已入响应队列，尚不代表 PE 已取走。
4. 算法失败时核对共享内存范围、长度单位、对齐、密钥槽可用性、模式和结果缓冲区；`SERV_OK` 还需结合验证结果或期望向量判断。

## 10. 当前限制和已知风险

1. **PE 共享区重叠**：本次 PE1 的 `.que_isd_buf.bss` 为 `[0xFE00C000, 0xFE014D20)`，PE2 为 `[0xFE012000, 0xFE01AD20)`，重叠 `[0xFE012000, 0xFE014D20)`。PE2 队列/部分缓冲区落在 PE1 的 `strbin` 区间内，存在互相覆盖风险；多核并发前必须重新规划并验证链接地址。
2. **ECC/HMAC 初始化风险**：默认仅配置 VM 0，但 [mh_KEY_InitEccRamCfg](icum/generic/src/middle/mh_key/icum_mh_key_ecc.c) 遍历最大 VM 数，在无 ECC 槽的后续 VM 也执行 `< 2` 检查并返回错误；ECC 后续初始化和 `MH_KEY_Init` 中的 HMAC 初始化因错误被跳过，而 [MH_INIT_Init](icum/generic/src/middle/mh_init/icum_mh_init.c) 忽略其返回值。上述是当前代码路径分析，须在目标板复核并修复后才能依赖 ECC/HMAC 完整初始化。
3. 演示测试中的 `trigger_service_and_wait` 无超时，且对 `R_ICUMIF_ServiceRequest` 的非零返回没有实质处理，可能出现无限等待；不能作为生产接口错误处理模板。
4. `k_ISSD_CheckNVMWarning` 的调用当前被注释，不能假定容量警告一定随服务结果返回；当前数据集后端本身也是 RAM 模拟。
5. AES at-once 异步资源/完成路径仍需板级确认。本文只记录当前代码和风险，不将构建通过等同于算法或安全认证通过。
