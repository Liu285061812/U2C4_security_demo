# ICUM 工程说明（RH850/U2C4）

配套文档：[PE 工程说明](PE_PROJECT_GUIDE.md)、[工程首页](README.md)。

## 1. 简介与适用范围

ICUM 是运行在独立安全处理器 ICUP 上的固件；PE 是发起服务的应用处理器。
PE 通过共享 RAM 中的服务描述符 ISD、请求/响应队列和双向中断访问 ICUM，
不直接传递本演示所用的私钥。ICUM 根据服务参数中的密钥组、密钥索引及 VM 选择内部密钥。

本工程是 **U2C4 算法服务演示**，不是原始 U2A 固件的全部生命周期和密钥管理产品。
原手册中的服务编码、地址、Option Bytes、Flash 持久化和板级参数不能原样套用：

- 对外演示服务重排为 AES、RSA、RNG、ECC、HASH/HMAC 五组。
- 演示密钥与生成的新密钥在 ICUM RAM 中使用；没有开放密钥注册、导入、更新菜单服务。
- 数据集接口当前接入 RAM 后端，不提供掉电持久化；原 Data Flash 自动 GC 不在当前构建路径中。
- 固定演示向量可以存在于固件映像中，启动后建立 RAM 工作槽；**不是生产密钥保护方案**。
- 是否支持某种参数组合，以当前源码、编译配置和硬件实测为准；菜单回归不等于穷举全部库模式。

### 参考资料与对应章节

以下资料均位于工程的上一级目录，文档本身未复制进仓库：

| 资料 | 本说明参考内容 |
| --- | --- |
| [Security Services，R11UZ0176EJ0120](../r11uz0176ej0120-icumhfw-securityservices.pdf) | §1 安全域模型；§2.1 软件与内存结构；§2.3～2.5 初始化、调度及 job_slice；§2.7 密钥；§3 ISD；§5 算法服务 |
| [Startup Guide，R11UZ0177EJ0120](../r11uz0177ej0120-icumhfw-startupguide.pdf) | §2～§4 Option Bytes、构建和烧录；§5.8～§5.10 多核工程结构和调试 |
| [Security Service Handler，R11UZ0179EJ0110](../r11uz0179ej0110-icumhfw-securityservicehandler.pdf) | §4 请求/响应 API；§5 数据结构；§6 交互时序；§7 排他、同步和共享 RAM |

这些手册面向 U2A16/U2A8/U2A6。本说明中的 U2C4 地址来自本工程链接脚本和构建 map；
遇到差异时，应先区分“原始框架原理”和“当前目标平台实现”。

## 2. 代码框架与构建

| 层次 | 路径 | 职责 |
| --- | --- | --- |
| 顶层工程 | [top.U2C.gpj](top/u2c/top.U2C.gpj) | 器件、工具链外部路径、算法测试开关、多核归档配置 |
| 初始化/主循环 | [icum_k_issd_main.c](icum/generic/src/kernel/k_issd/icum_k_issd_main.c)、[mh_init](icum/generic/src/middle/mh_init) | 执行模式初始化、硬件/库/中间件初始化、持续运行调度器 |
| 内核 | [k_issd](icum/generic/src/kernel/k_issd) | 请求接收、作业队列、优先级、执行模式、完成响应 |
| 服务 | [service](icum/generic/src/service) | 参数检查、内存检查、密钥及 context 获取、算法分阶段调用、结果写回 |
| 中间件 | [middle](icum/generic/src/middle) | 配置、密钥、context、随机数、资源仲裁、数据集管理 |
| 算法适配层 | [SAL library](icum/sal/src/library) | AES/HASH/PKC 接口、编码、填充、KDF、大数及驱动适配 |
| U2C4 驱动 | [driver](icum/target/u2cx/src/driver) | 通信、AES/HASH/PKCC、TRNG、WDT、地址重映射等 |

逻辑服务分组以
[分发表](icum/generic/src/kernel/k_issd/icum_k_issd_dispatch_tables.c)为准。
历史文件名 `s_01_*`、`s_02_*`、`s_03_*` 不再与当前服务组号一一对应，
例如 RSA keygen 仍由 `S_03_GenerateRsaKey` 实现，但服务编码是 `0x24`。

在工程根目录执行：

```powershell
& 'C:\ghs\comp_202354\gbuild.exe' -top top\u2c\top.U2C.gpj
```

需安装匹配的 GHS RH850 工具链，并核对顶层工程中的器件头文件和 RFD 路径。
输出包括 [多核固件包](top/u2c/U2C4_firm.mca)、
[ICUM HEX](icum/target/u2cx/build/_release/r_icumfw_dbg_U2C4_Alpha.hex)
及 PE 映像。构建不等于已更新芯片 Option Bytes，也不等于硬件算法验证。
修改共享接口后必须同步构建和更新 PE、ICUM，不能混用不同 ISD 布局的映像。

## 3. 功能与服务接口

编码定义见 [r_icumif_api.h](pe/generic/icumif/r_icumif_api.h)。
所有服务共用 `r_icumif_isd_t`，具体输入输出由 `prm` 联合体承载。

| 编码 | 功能 | 主要参数/结果 |
| --- | --- | --- |
| `0x01` | 演示测试边界通知 | `TEST_MARKER.event`：BEGIN/END；由 ICUM 翻转 P17_0，不是算法菜单入口 |
| `0x10` | AES cipher | `AES_CIPHER`：模式、方向、密钥组/ID、IV、输入输出、长度、job_cycle/job_slice |
| `0x11` | AES GCM/CCM | `AES_AUTH_CIPHER`：数据、AAD、IV、认证 tag、认证结果 |
| `0x12` | AES CMAC | `AES_CMAC`：消息、密钥、生成/校验、MAC 和校验结果 |
| `0x20` | RSA verify | `RSA_VERIFY`：消息、公钥、签名、编码及 HASH、校验结果 |
| `0x21` | RSA sign | `RSA_SIGN`：消息、私钥、编码、HASH、输出签名 |
| `0x22` | RSA encrypt | `RSA_ENCRYPT`：公钥、明文、OAEP/PKCS#1 v1.5、输出密文 |
| `0x23` | RSA decrypt | `RSA_DECRYPT`：私钥、密文、编码、输出明文和长度 |
| `0x24` | RSA keygen | `RSA_KEY_GENERATE`：模数位数等；完成后安装 RAM 密钥 |
| `0x30` | RNG 初始化 | `RAND_INIT`：在线测试配置 |
| `0x31` | RNG 扩展种子 | `RAND_EXTEND_SEED`：熵输入指针 |
| `0x32` | RNG 输出 | `RAND_GENERATE`：输出缓冲区、16 字节块数、job_slice |
| `0x40` | ECDSA verify | `ECDSA_VERIFY`：曲线对应密钥、消息、r/s、HASH、校验结果 |
| `0x41` | ECDSA sign | `ECDSA_SIGN`：消息、私钥、HASH、输出 r/s |
| `0x42` | ECDH | `ECDH`：不同的私钥/公钥索引、KDF 参数、目标 AES RAM 密钥槽 |
| `0x43` | EdDSA verify | `EDDSA_VERIFY`：消息、公钥、签名、变体及校验结果 |
| `0x44` | EdDSA sign | `EDDSA_SIGN`：消息、种子/私钥、变体、输出签名 |
| `0x45` | ECC keygen | `ECC_KEY_GENERATE`：曲线和目标 RAM 槽 |
| `0x50` | HASH | `HASH`：消息、HASH 类型、输出摘要、分段参数 |
| `0x51` | HMAC | `HMAC`：RAM key ID、消息、生成/校验、tag 及校验结果 |

本表覆盖当前菜单公开的 19 个服务入口。分发表中另外保留的 CR 扩展项不在这 19 项菜单回归内；
不能因为某段历史源码仍存在，就认为它属于公开演示接口。
模式的枚举定义位于 [PE/ICUM 共用接口头文件目录](pe/generic/icumif)。
例如当前 HASH 映射不支持 SHA-1，RSA 演示使用 SHA-256，而不是照搬手册旧示例的 SHA-1。

### 长度、格式和完成状态

- 非 XTS AES cipher 的 `size.nb_blocks` 单位为 16 字节块；**XTS 的同一个 union 字段按位数解释**。
- AES AUTH、CMAC 的数据长度使用位；RSA/ECC 消息一般使用字节，签名/密文长度字段可能使用位。
- RSA n/d/e、P-256 标量及坐标使用接口约定的大端二进制；不要把数组容量当作有效长度。
- Ed25519 保存原始 seed 与压缩公钥，不按 P-256 的 X/Y 格式解释。
- `SERV_OK` 表示服务执行成功；验签、CMAC/HMAC 校验还必须检查相应 `verification_result`。
  被篡改数据得到 `SERV_OK + VERIFICATION_FAIL` 是正常拒绝，不应误判为验证成功。

## 4. 地址分配与容量

地址区间统一采用 **起始包含、结束不包含**。主要依据：
[icup_B.ld](icum/target/u2cx/config/u2c/icup_B.ld)、
[icup_user.ld](icum/target/u2cx/config/u2c/icup_user.ld)、
[PE 链接脚本](pe/target/u2cx/config/u2c/pe1.ld)。

| 区域 | 地址 | 用途/注意事项 |
| --- | --- | --- |
| Code Flash A | `0x00000000～0x00200000` | PE 与安全固件预留区共同划分，不可把整个范围当作 PE 可用区 |
| Code Flash B | `0x00400000～0x00600000` | 当前 B 侧 ICUM 固件位于此 bank 尾部 |
| 固件 A 预留区 | `0x001A0000～0x00200000` | 6 个 64 KiB 块 |
| 固件 B 预留区 | `0x005A0000～0x00600000` | 6 个 64 KiB 块 |
| 当前 ICUM B 启动入口 | `0x00590000` | 本次 map 中 `.start.ROM`；不要仅下载后面的服务代码而漏掉启动区域 |
| ICUM Local RAM | `0xFEDE0000～0xFEE00000` | 128 KiB；含 RAM 执行代码、数据、context、密钥和栈 |
| Cluster RAM 0 | `0xFE000000～0xFE070000` | 448 KiB；共享缓冲区及 PE 分区需遵守 guard |
| ICUM CRAM guard | `0xFE06C000～0xFE070000` | 16 KiB；由用户链接配置指定 |
| ICUM Data Flash 物理窗口 | `0xFF280000～0xFF290000` | 地址定义仍保留，但当前数据集后端不是物理 Flash 存储 |

本次构建 `.icum_config_ram` 为 `0xFEDFAF94～0xFEDFC394`，容量 5 KiB。
它不是整个 ICUM RAM，也不包含全部 RSA 数组及所有 context。
该起始地址会随代码/数据布局变化；应查询
[ICUM map](icum/target/u2cx/build/out/icup_B.map)
中的 `.start.icum_config_ram` 与 `.end.icum_config_ram`，不要写死到应用。

`ML_CFG_SetIndexMappingVM()` 在计算地址前补足演示所需的 AES/ECC/HMAC 最低槽数；
`ML_CFG_GetKeyMappingData()` 汇总各 VM 数量、建立索引偏移并检查 RAM 边界。
槽结构大小和编译 ABI 影响容量，不能直接采用旧源码注释中的字节数。

## 5. 关键结构体

### 5.1 共享接口

定义：[r_icumif_api.h](pe/generic/icumif/r_icumif_api.h)。

| 类型/字段 | 说明 |
| --- | --- |
| `r_icumif_isd_t` | 一次请求的共享描述符，必须保持存活直到完成 |
| `service_id` / `prm` | 服务编码及对应参数；不能用错误的联合体成员解释数据 |
| `requester_id` / `vm_id` / `host_int_ch` | 请求来源、密钥命名空间及响应中断通道 |
| `service_priority` | 调度优先级，不是算法模式 |
| `req_res_status` | 请求入队、ICUM 取出、响应入队、PE 取出的队列交互状态 |
| `service_result` | 待处理、处理中或最终服务结果；与接口 API 的返回码不同 |
| `job_id` / `p_callbackfunc` | 作业标识及可选完成回调 |

### 5.2 内部作业与执行现场

定义：[icum_k_issd_pub.h](icum/generic/include/kernel/icum_k_issd_pub.h)。

- `icum_job_t`：持有共享 ISD 指针、VM、优先级、`process_phase`、取消/保持标志及算法私有参数。
- `icum_job_t.algorithm_active`：当前作业是否已进入算法窗口，避免异步重入重复产生开始沿；新作业分配时清零，不进入共享 ISD。
- `job_rsa_t` / `job_ecc_t`：持有密钥指针、RSA/ECC/HASH context、资源控制位和剩余 HASH 数据。
- `job_hash_t` / `job_hmac_t`：保存当前源指针、剩余字节数、每轮处理量和 context。
- `icum_job_queue_t`：队列链接索引。共享请求队列和 ICUM 内部作业队列不是同一个对象。

context 池见 [icum_ml_ctx.h](icum/generic/src/middle/ml_ctx/icum_ml_ctx.h)：

| 类别 | 数量 | 每个 context 有效工作区 |
| --- | --- | --- |
| AES | 8 | `28 × sizeof(uint32_t)` = 112 字节 |
| RSA/ECC 共用 | 1 | `2491 × sizeof(uint32_t)` = 9964 字节 |
| HASH/HMAC 共用 | 2 | `128 × sizeof(uint32_t)` = 512 字节 |

上表不含池对象自己的地址/保留字段。RSA 和 ECC 共享一个池，资源暂忙不等于算法失败。
`ML_CTX_Free()` 会清理现场；仍处于处理中或可恢复 pending 的服务不能释放后沿用旧 phase。

### 5.3 密钥与配置

定义：[icum_mh_key_pub.h](icum/generic/include/middle/icum_mh_key_pub.h)、
[icum_ml_cfg.h](icum/generic/src/middle/ml_cfg/icum_ml_cfg.h)。

- `aes_key_t`：密钥值、长度、用途 flags、组/ID/VM、可用状态。
- `rsa_key_t` / `rsa_key_prop_t`：n/d/e 指针和有效长度、位数及用途属性。
- `ecc_key_t` / `ecc_key_prop_t`：曲线、私有标量、公开点及标量位数。
- `hmac_key_t`：字节长度、密钥数据指针、VM/ID、可用状态。
- `cfg_sys_t` / `sys_cfg_key_mapping_t`：系统及每 VM 的各类槽位数量；不是直接存储密钥值的结构。

## 6. 密钥存储及管理逻辑

| 工作密钥 | 启动位置/用途 | 源码 |
| --- | --- | --- |
| AES RAM 0 | AES-128 主演示 key | [AES key manager](icum/generic/src/middle/mh_key/icum_mh_key_aes.c) |
| AES RAM 1、2 | AES-192、AES-256 演示 key | 同上 |
| AES RAM 3 | XTS 的第二把 AES-128 key | 同上 |
| AES RAM 4 | ECDH 派生结果；启动时不作为已初始化 demo key | 同上及 [ECDH 服务](icum/generic/src/service/s_01/icum_s_01_ecdh.c) |
| RSA ID 0 | 初始 2048 位 n/d 和 4 字节 e；RAM 数组供 cache 使用 | [RSA key manager](icum/generic/src/middle/mh_key/icum_mh_key_rsa.c) |
| ECC RAM 0、2 | 不同的 P-256 本端/对端密钥对 | [ECC key manager](icum/generic/src/middle/mh_key/icum_mh_key_ecc.c) |
| ECC RAM 1 | Ed25519 seed 与压缩公钥 | 同上 |
| HMAC RAM 0 | RFC 4231 case 1，20 字节 `0x0b` | [HMAC key manager](icum/generic/src/middle/mh_key/icum_mh_key_hmac.c) |

管理路径：

1. 初始化系统配置，计算 VM 索引和 RAM 地址；AES/ECC/HMAC 每 VM 至少保留 5/3/1 槽。
2. `MH_KEY_Init()` 调用各类初始化，建立值、属性、可用标志及引用计数。
3. 服务通过 `MH_KEY_Get*Key()` 检查 VM、索引、可用性和用途，并取得引用。
4. 完成或不可恢复失败时，通过 `MH_KEY_FreeKeyCache()` 等配对释放引用。
5. RSA/ECC keygen 成功后走 ICUM 内部 setter 更新 RAM，**不等于向 PE 重新开放导入/更新服务**。

RSA keygen 使用独立对齐工作区而不是借用唯一的活动 key cache；生成失败不应把半成品当作有效 key。
RSA/ECC 生成会改变后续使用的 RAM 密钥；断电/冷启动后重新从固件初值开始。
RSA 演示的初始绑定是 VM 0 / ID 0，不应推断它与 AES/ECC/HMAC 的多 VM 槽初始化完全相同。

当前 [RAM 数据集后端](icum/generic/src/middle/mm_fls/icum_mm_fls_ram.c)
保留 `MM_FLS_*` 兼容接口及内部 RAM 空间压紧；这不是物理 Data Flash 的后台 GC，
也不能用于承诺密钥持久化。初始化会清空这一后端。

## 7. 功能调用与执行流程

```text
ICUP reset
  -> 启动代码/运行模式初始化
  -> 系统配置、硬件、context/仲裁/随机数相关模块、密钥初始化
  -> main 持续调用 k_ISSD_Run

PE 填写共享 ISD 和数据
  -> R_ICUMIF_ServiceRequest
  -> 请求队列 + 通知 ICUP
  -> ICUM 检查来源/服务权限/参数及内存
  -> 建立 icum_job_t，按优先级调度
  -> 服务获取密钥/context，调用 SAL -> 驱动/硬件
  -> pending / in-process：保留或按约定重新初始化，后续继续调度
  -> terminal：写结果，释放资源，响应入队并通知 PE
```

关键内部接口：

| 接口 | 作用与调用约束 |
| --- | --- |
| `ML_CFG_SetIndexMappingVM()` | 在密钥初始化前完成槽数量和布局计算 |
| `ML_CTX_Get()` / `ML_CTX_Free()` | 分配/释放算法现场，必须配对且遵守异步生命周期 |
| `MH_KEY_Get*Key()` / `MH_KEY_FreeKeyCache()` | 取得密钥引用并最终释放，避免运行中覆盖 |
| `MM_FLS_CheckMemory()` | 检查算法源/目标区间；指针位于 RAM 不代表该范围一定允许访问 |
| `D_REMAP_GetLogicalAddrs()` | 服务访问前按目标平台重映射 PE 传入地址 |
| `s_01_ConvertErr()` | 将内部错误转换成共享服务状态 |
| `D_WDT_StartIfNeeded()` | 按 WDT 窗口条件服务看门狗；不是关闭 WDT |

原手册的 job_slice、job_cycle 与硬件内部异步是不同层面的机制，不能混为“函数调用一次一定完成”。
当前初始化配置仍使用 WDT；长耗时 PKCC polling 通过已有回调/窗口机制服务它。
发生 MAEX/MPU 后再出现 WDT NMI，必须先检查第一现场，不应先禁用 WDT 掩盖问题。

### 7.1 RSA DMA 数据搬运

依据本地 U2C ICUMHB 硬件手册 `R01UH1024EJ0100` §1.10（尤其表 1.300，
第 645～646 页）实现，不能套用 AES 的 DTF 寄存器或 PE 的 sDMAC 地址：

- ICUM_DMAC 基址 `0xFF1FA000`，4 个通道；通道寄存器起点为基址加 `0x2000`，
  通道步长 `0x80`。普通/安全模幂、CRT 的大块操作数及最终结果使用 DMA。
- `TRS=0` 自动请求，源/目标地址递增；PKCC RAM 侧使用 64 字节事务，
  Local RAM 侧使用 4 字节事务。PKCC 地址和长度满足 8 字节对齐/倍数约束。
- 每次搬运前确认 PKCC 未运行，并将 `ICUM_PKCCA0CR.PKCRSEL`（bit 20）由 0 切为 1，
  将 PKCC RAM 访问权从 ICUP 交给 DMA（§1.9.4.2，第 374 页）；仅在 DMA 停止且在途传输结束后
  恢复原控制寄存器，再进行 CPU 访问或启动 PKCC 运算。缺少此切换会使 DMA 无法正确访问 PKCC RAM。
- RSA 使用 512 字节对齐暂存区，由 CPU 转换每个 64 位 limb 的字节序，再通过 DMA 搬入 PKCC RAM；
  读回按相反顺序进行。4 字节指数/标志、PKCC 内部移动以及其余软件算术复制保留 CPU 路径。
  DMA 不代替 OAEP/PSS 填充、HASH 或 PKCC 数学运算，也不会突破单次 RSA 消息上限。
- 新增公共 DMA 通道租用机制：HASH 成对占用 `0/1` 或 `2/3`，RSA 临时占用剩余空闲通道。
  软件租用与硬件 `DE/BUSY` 同时检查；不会重置其他调用者的通道，也不关闭全局 DMA。
  通道不足返回错误，不静默降级成 CPU 搬运。
- RSA 搬运使用轮询，不打开完成/地址错误中断，避免依赖未注册的 DMA ISR。
  同时检查 `CAE`、`TE`、剩余长度和 `BUSY`，轮询有上限并调用 `D_WDT_StartIfNeeded()`。
  DMA 超时映射为 PKCC timeout，其余 DMA 故障映射为 PKCC error，经现有 SAL/service 路径回传。
- 失败或完成后停止新请求、等待在途传输结束，再清标志、释放通道并擦除暂存区。
  若停止后仍无法结束，锁存故障并保留 DMA 的 PKCC RAM 访问权、通道和暂存区，
  拒绝继续复用 PKCC；此时需要复位排查总线故障。

AES DTF 不占用这四路 DMA。当前 SHA/HMAC 测试是否启用自身 DMA 仍由原来的参数和驱动条件决定，
本次没有强制改变其搬运方式，也没有将所有 RSA/keygen 内部软件复制替换成 DMA。
上板时检查 RSA 请求期间通道的 SAR/DAR、TSR、TE/CAE 和实际输出；仿真不能证明总线或性能收益。

### 7.2 P17_0 逐操作标记

P17_0 使用 `SECOUT0/ICUMGPIO0`，由 ICUM 的 `ICUM_GPIO` 控制，不由 PE 写普通端口输出。
依据硬件手册 §1.11.3.3、§1.15.4.1，启动时先设置 `DOUT0=0`、`CTLEN0=1`，
同步后再设置 `EOUT0=1`；仅修改这三个 bit，保留其余 ICUM GPIO。
芯片实际 `ICUM_OPBT4.ICUMOPGPIO`（bit 17）必须为 1，P17_0 保持 `PM17_0=1`、
`PSFC17_0=0`，不能再分配其他端口功能或 Safe State。现有 UART 使用 P17_2/P17_3，不占用 P17_0。
本功能不会自动重写 Option Bytes；配置未启用时，通知返回 `SERV_UNAVAILABLE`，PE 明确报错。

不再由 PE 的独立 BEGIN/END 通知驱动，原演示控制服务 `0x01` 已移除。
各服务在通过外围校验、取钥、地址映射及资源准备后，在密码算法核心处调用
`K_ISSD_BeginAlgorithm(p_job)`，完成时调用 `K_ISSD_EndAlgorithm(p_job)`。
最终结果封装、服务层输出格式转换、密钥安装、最终资源释放及核间通知均不属于算法窗口。
如果实际 GPIO Option Byte 未启用，调度器在执行算法服务前返回 `SERV_UNAVAILABLE`。

| 算法 | BEGIN | END |
| --- | --- | --- |
| AES 普通模式、XTS、GCM、CCM、CMAC | 参数准备完毕后调用 AES 驱动前 | all-at-once 的最终 AES 回调入口；同步完成/启动失败则在驱动返回后结束 |
| AES 分片/流式接口 | 第一次实际 AES Init/Update/Finish 调用前 | 最终 Finish 返回后；完成或失败时在最终清理前兜底结束 |
| RSA 签名/验签 | 消息 HASH 开始；预计算 HASH 输入则在 RSA Init 前 | RSA Final 返回后，服务层签名格式转换、结果填写和资源释放前 |
| RSA 加密/解密 | RSA Init 前（包含内部填充等必要步骤） | RSA Final 返回后，服务层输出转换/长度填写及资源释放前 |
| RSA/ECC 密钥生成 | KeyGen Init 前 | KeyGen Final 完成后、RAM 密钥安装/属性设置/最终释放前 |
| ECDSA 签名/验签 | 消息 HASH 开始；预计算 HASH 输入则在 ECDSA Init 前 | ECDSA Final 返回后、签名格式转换/结果填写及最终释放前 |
| EdDSA 签名/验签 | EdDSA Init 前 | EdDSA Final 返回后、签名复制/编码或验证结果填写前 |
| ECDH | ECDH/X Init 前 | Final 完成必要的 KDF 后、派生 AES 密钥安装前 |
| RNG 初始化/扩展种子/生成 | 种子、secret key、输入地址及资源准备好后，进入相应 `ML_PRNG_*SHE` 前 | 相应 PRNG 调用返回后、错误转换、清钥及释放前 |
| HASH | HASH Init/Update 前 | HASH Finish 返回后，结果长度填写及 context 释放前 |
| HMAC | HMAC Init 前 | Finish/验证比对完成后，服务层最终清理前 |

测量单位是完整密码学操作，内部必要的 HASH、填充、KDF、驱动初始化和搬运等仍计入，
不是逐次底层硬件 BUSY 脉冲。AES 生成出的认证标签在服务层的比对属于结果后处理，不延长 AES 核心窗口。
同一作业的开始/结束操作幂等，异步 pending/重入保持电平；普通失败在清理前结束，
调度器仅保留异常返回/取消的结束兜底，不在服务入口产生开始沿。
所有 GPIO 更新在保留中断状态的临界区完成，并保留其他 GPIO 位。
内部活动计数避免并发作业相互提前拉低；单引脚在作业重叠时表示算法活动窗口的并集。

默认 PE 串行菜单中，加/解密、签/验签、MAC 生成/校验以及 RNG、RSA 准备加密分别标记。
顶层一键测试仍为 29 项结果，但只有实际进入算法核心的操作才产生脉冲，前置检查失败没有脉冲。
GPIO 不包含服务层处理、核间握手或串口打印；OSTM 保持 PE 提交到最终响应的端到端口径。
若硬件挂死且核心未报告完成，不能保证结束沿；保持高电平可辅助定位，此时应先排查再复位。

## 8. 测试逻辑与验收

测试数据、断言和菜单都在 PE，详见 [PE 测试说明](PE_PROJECT_GUIDE.md)。

当前 PE 在共享 RAM 中构造 10240 字节数据。AES/CMAC、RSA 签名/验签、
ECDSA/EdDSA、SHA-256/HMAC-SHA256 处理完整 10KB。
RSA-2048 加密从此数据首地址取一个最大合法填充块：OAEP-SHA256 为 190 字节，
PKCS#1 v1.5 为 245 字节；解密输入一份 256 字节密文，检查恢复长度及对应消息前缀，不再分块。
RSA 统计为每次服务的 ms/ops，不再计算 MB/s；无 payload 的服务不打印 throughput。
ICUM 的单次输入上限、服务接口、DMA 搬运及密钥长度没有改变。
RNG、密钥生成、ECDH 及其辅助 AES 单块检查保持原状。
PE 每个共享数据缓冲区扩为 16KiB；两核独立队列/缓冲段重排到 Cluster RAM 通用区内，
具体地址见 [PE 地址分配](PE_PROJECT_GUIDE.md)。更新时应烧录完整多核镜像，避免新旧布局混用。
ICUM 侧应重点观察：

1. 启动后的映射、密钥 availability 和有效长度是否正确。
2. 请求 ID 是否进入正确 service 表项，是否在算法前就返回索引/内存错误。
3. pending 时作业、key 引用、context 与 phase 是否一致。
4. 完成后资源是否释放，重复调用是否仍可运行。
5. 验证类操作的 `verification_result`，而不是仅检查 `SERV_OK`。

一键测试按菜单入口 `0x10～0x51` 的顺序运行；内部为生成合法签名、准备 RNG 等可能先调用其他 ID。
其中 `0x24` 和 `0x45` 分别生成 RSA/ECC RAM 密钥，会影响后续及下一轮测试。

板上至少执行：

- 同步更新两个域的映像，核对 Option Bytes/CPU 320 MHz/SBUS 160 MHz，冷复位。
- 运行 `icum 6`，默认应覆盖 19 个入口并输出 29 个 PASS 测试结果，无 FAIL/skip。
- 再运行一轮，检查生成新密钥后的重复使用和资源释放。
- 单独运行生成密钥后的 RSA sign/verify、enc/dec，以及 ECDSA/ECDH。
- 确认消息/tag 篡改被拒绝，且没有 MAEX/MPU/WDT 异常。

仿真中的外设替代结果只能证明软件契约、状态机、错误清理和格式检查；
不能作为真实 PKCC/HASH/AES/TRNG 数学运算、随机质量或 MB/s 性能的验收数据。

## 9. 调试定位

| 现象 | 优先检查 |
| --- | --- |
| `SERV_INVALID_KEY_INDEX` | 组/ID/VM、槽数量；ECDH 双方是否错误地选择同槽 |
| `SERV_KEY_SLOT_EMPTY` | 密钥初始化是否执行、availability、key 长度、映像版本 |
| `SERV_NO_INIT_RNG` / `SERV_NEED_RESEED` | RNG init/seed 的真实返回值和调用顺序 |
| `SERV_SRC_MEM_ERROR` / `SERV_DEST_MEM_ERROR` | 共享区域、区间长度/单位、guard 和重映射 |
| PE 等待不返回 | 请求是否成功提交、ICUM 是否完成/陷异常、响应是否入队、中断路由 |
| 首次成功、后续失败 | key 引用未释放、context 重复释放、PKCL 状态和作业 phase 不一致 |
| HASH/PKC 对齐异常 | 顶层缓冲及内部子缓冲；不能仅靠对齐 PE 数组掩盖内部字节访问问题 |

维护新增服务时，需同步更新枚举、ISD union、ICUM 分发表、PE 名称/长度计量映射、测试入口与本文，
并复核两个域的 ABI、RAM 容量和冷启动行为。
