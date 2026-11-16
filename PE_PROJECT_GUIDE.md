# PE 工程说明

本文介绍当前 RH850/U2C4 演示工程的 PE 构建、内存、算法调用和串口测试。ICUM 内部架构、密钥布局和服务实现见 [ICUM 工程说明](ICUM_PROJECT_GUIDE.md)，各模式的密钥、输入和预期值见 [算法测试输入与预期结果](ALGORITHM_TEST_VECTORS.md)。本文基于当前源码及现有 map，不表示所有测试已在目标板通过；所有地址区间均为左闭右开。

> **命名注意**：工程和文件名 `pe1` 对应硬件 **PE0**，工程 `pe2` 对应硬件 **PE1**。本文说“PE1 工程”时指文件名；说“硬件 PE0”时指实际处理器编号。

## 1. 工程简介

PE 负责设备初始化、串口菜单、构造共享 RAM 中的服务描述符（ISD）、向 ICUM 提交请求、处理响应中断和打印结果。算法实际由 ICUM 完成；密钥由 ICUM 内部选择和保存，不是由 PE 在每次请求中传入原始密钥。

```text
串口命令 icum
  → 分类菜单 vrun.cmd.c
  → pe1_crypto_service_test.c 构造 ISD 和共享数据
  → R_ICUMIF_ServiceRequest
  → ICUM 执行 / 回写 → 响应中断 / ServiceResponse
  → OSTM0 计时、服务状态、结果比较、逐项打印
```

多核固件由 [top.U2C.gpj](top/u2c/top.U2C.gpj) 和 [U2C_firm.gpj](top/u2c/U2C_firm.gpj) 构建，包含 PE1、PE2、shared 和 ICUM。PE1 是当前交互式算法测试主体，PE2 主要用于第二核初始化、同步和通信演示，不能将其视为 PE1 全部算法测试的独立副本。

## 2. 环境、构建和运行

1. 准备目标芯片支持文件、Green Hills 工具链、调试器及烧录工具；检查 [top.U2C.gpj](top/u2c/top.U2C.gpj) 中设备头文件和 RFD 的本机绝对路径。
2. 选用 `cfg_build_fw=1`、`cfg_build_test=1` 的 U2C4 配置。当前 AES/RSA 相关 `PE1_TEST_*` 宏均为 1，详见构建配置和测试文件。
3. 构建多核镜像：

   ```powershell
   gbuild -top top\u2c\top.U2C.gpj
   ```

4. 按板卡流程烧录多核镜像及所需启动配置并复位。时钟取决于实际 Option Bytes；CPU 320 MHz、CLK_SBUS 160 MHz 的设置与校验见 [README](README.md)。不要假设构建或下载会自动编程 Option Bytes。
5. 串口为 RLIN31，P17_2 TX、P17_3 RX，配置波特率 115200；其时钟为 CLKC_HSB 80 MHz。上电进入控制台后输入 `icum`，或输入 `icum 308` 直接打开分类菜单。

本仓库中的 map 是构建快照，不一定与重新编译或目标板镜像一致。实际结果请检查 `pe\target\u2cx\build\out\pe1\pe1.map`、`pe2\pe2.map`、`shared\shared.map` 及多核最终镜像。

## 3. 源码与职责

| 位置 | 内容 |
|---|---|
| [pe1.c](pe/generic/src/pe1/src/pe1.c) | PE1 启动、ICUM 中断、服务请求等待和耗时打印 |
| [vrun.cmd.c](pe/generic/src/pe1/src/vrun.cmd.c) | 命令解析、算法分类及子服务菜单 |
| [pe1_crypto_service_test.c](pe/generic/src/pe1/src/pe1_crypto_service_test.c) | 测试数据、ISD 设置、算法调用和结果检查 |
| [r_icumif_api.h](pe/generic/icumif/r_icumif_api.h) | 服务枚举、公共状态、ISD 和算法参数联合体 |
| [r_icumif_pub.h](pe/generic/include/r_icumif_pub.h) / [r_icumif.c](pe/generic/src/icumif_lib/r_icumif.c) | PE 公共调用 API 及实现 |
| [que_isd_buf.c](pe/target/u2cx/src/shared/src/que_isd_buf.c) | 各 PE 的队列、描述符和数据共享缓冲区 |
| [pe1.ld](pe/target/u2cx/config/u2c/pe1.ld)、[pe2.ld](pe/target/u2cx/config/u2c/pe2.ld)、[shared.ld](pe/target/u2cx/config/u2c/shared.ld) | PE1、PE2、shared 链接脚本 |
| [icum_d_comm_pe.c](pe/target/u2cx/src/driver/comm_drv/icum_d_comm_pe.c) | PE 请求/响应队列和通信中断 |
| [sys.int.uart.c](pe/target/u2cx/src/l_vrun/sys.int.uart.c) | 串口配置和收发 |

## 4. Code Flash、Data Flash 和 RAM 分配

### 4.1 Code Flash 与 Data Flash

| 地址区间 | 用途和限制 |
|---|---|
| `0x00000000–0x00200000` | Code Flash Bank A；PE1 工程从 `0x00002000` 链接启动/向量和代码，PE2 工程启动向量在 `0x00000000` |
| `0x00400000–0x00600000` | Code Flash Bank B；PE2 工程的主要代码/常量在该 bank 链接；ICUM Side B 保留区也在此 bank |
| `0x08000000–0x08010000` | 扩展用户区 0；本次 shared map 的 `.intvect0` 位于 `0x08000000` |
| `0x08400000–0x08410000` | 扩展用户区 1，链接脚本声明 |
| `0xFF200000–0xFF220000` | Data Flash A，128 KiB |
| `0xFF220000–0xFF240000` | Data Flash B，128 KiB |
| `0xFF280000–0xFF290000` | ICUM Data Flash，64 KiB |

本次 [shared.map](pe/target/u2cx/build/out/shared/shared.map) 的 `.testconst` 从 `0x0015E000` 开始，大小 `0x101A4`。各链接脚本中的 `MEMORY` 是各自的可链接视图，**不是整个 bank 已独占或全部已烧录的证明**；合并镜像时还应检查 PE/ICUM 的实际 section、启动保留区和保护配置。

PE 的算法测试通过共享 RAM 与 ICUM 交互，不因此自动写入上述 Data Flash。虽然 Data Flash 驱动定义仍在，本次 ICUM 安全数据集后端实际是 RAM 模拟，复位不持久；详见 [ICUM 工程说明](ICUM_PROJECT_GUIDE.md)。

### 4.2 Local RAM 和 Cluster RAM

| 地址区间 | 大小 | 作用 |
|---|---:|---|
| `0xFDE00000–0xFDE20000` | 128 KiB | PE self RAM 地址别名；同一地址在不同核对应各自本地 RAM，不能看作共享区 |
| `0xFDC00000–0xFDC20000` | 128 KiB | 链接脚本命名的 PE1 物理 Local RAM |
| `0xFDA00000–0xFDA20000` | 128 KiB | 链接脚本命名的 PE2 物理 Local RAM |
| `0xFE000000–0xFE027800` | 158 KiB | 链接脚本的 Cluster RAM 公共区；当前队列及算法缓冲区位于此区 |
| `0xFE027800–0xFE028000` | 2 KiB | 互斥及核间同步区；`SYNC_PE` 使用其中的位置 |
| `0xFE028000–0xFE038000` | 64 KiB | PE1 预留 Cluster RAM 子区 |
| `0xFE038000–0xFE048000` | 64 KiB | PE2 预留 Cluster RAM 子区 |
| `0x12000000–0x12008000` | 32 KiB | 链接脚本声明的 Retention RAM |

ICUM 链接器将整个 Cluster RAM Bank A 定义到 `0xFE070000`；上表是 PE 链接脚本的部分划分，不等于其他地址全部不可访问。不同核的 Local RAM self 别名相同，但 Cluster RAM 地址是共享物理地址。

### 4.3 当前共享缓冲区与重叠风险

每个 PE 的 [que_isd_buf.c](pe/target/u2cx/src/shared/src/que_isd_buf.c) 分配 `ICUM_QUEUE`（请求及响应各 4 项，共 32 B）、`BIG_BUFFER`（3 × 4096 B）、`ISD_BUFFER`（3200 B）、`COM_BUFFER`（512 B）和 `strbin`（20096 B）。总计 `0x8D20`（36128 B），用于不同接口和测试；不要将整个区域都当作算法输入缓冲区。

当前 [PE1 map](pe/target/u2cx/build/out/pe1/pe1.map)：

```text
0xFE00C000 ┌ ICUM_QUEUE     0x20 B
0xFE00C020 ├ BIG_BUFFER     0x3000 B
0xFE00F020 ├ ISD_BUFFER     0xC80 B
0xFE00FCA0 ├ COM_BUFFER     0x200 B
0xFE00FEA0 ├ strbin         0x4E80 B
0xFE014D20 └ 结束
```

当前 [PE2 map](pe/target/u2cx/build/out/pe2/pe2.map)中同一 section 是 `[0xFE012000, 0xFE01AD20)`；PE2 的 `ICUM_QUEUE` 从 `0xFE012000` 开始，`BIG_BUFFER` 从 `0xFE012020` 开始。两核重叠 **`[0xFE012000, 0xFE014D20)`，即 0x2D20（11552）字节**，也就是 PE2 队列和部分数据缓冲与 PE1 `strbin` 共用物理空间。初始化或写入时可能互相覆盖；这不是 Local RAM 的 self 别名。使用多核或依赖这些缓冲区前，应调整链接布局、重新构建并检查所有 map；单核某次测试成功不能证明并发安全。

## 5. 公共 API 和调用生命周期

声明见 [r_icumif_pub.h](pe/generic/include/r_icumif_pub.h)：

| 接口 | 作用 |
|---|---|
| `R_ICUMIF_Init(uint32_t *p_request_queue)` | 初始化本 PE 队列及 ICUM 接口；队列需使用满足要求的共享 RAM |
| `R_ICUMIF_ServiceRequest(r_icumif_isd_t *p_ISD)` | 提交异步服务。返回值是接口的**即时提交结果**，不是算法结果 |
| `R_ICUMIF_ServiceResponse(void)` | 从本 PE 的响应队列取出 ISD 并在注册时执行回调；通常由中断路径调用 |
| `R_ICUMIF_IsServiceCompleted(r_icumif_isd_t *p_ISD)` | 查询接口状态；须区分错误返回并结合中断模式理解“完成”。`req_nointerrupt` 时请求入队即可能返回完成，不能据此认定算法已完成；最终算法状态仍检查 ISD |
| `R_ICUMIF_IRQ_Handler(void)` | 处理 ICUM 发往 PE 的普通响应中断 |

一次正常请求的操作顺序：

1. ICUM 初始化并可接收服务，PE 完成自身队列和中断配置。
2. 在 ICUM 可访问、满足对应服务对齐要求的 Cluster RAM 中准备 ISD、输入、输出、IV、tag 等。可参考 `test_isd()` 和现有 `BIG_BUFFER` 的使用；ISD 重用前必须确认前一请求已经完成。
3. 清理 ISD，设置 `service_id` 和匹配的 `prm` 联合体成员、模式、长度、缓冲区、密钥选择及作业参数。部分密钥字段会被 ICUM 覆盖，见第 8 节。
4. 调用 `R_ICUMIF_ServiceRequest()` 并检查返回值；如未成功提交，不应等待一个不会到来的响应。
5. 正常中断路径等待响应；完成之前 ISD 和所有引用的缓冲区都必须保持有效且不被其他核改写。超时和提交失败要显式处理。
6. 完成后检查 `service_result == SERV_OK`；对签名、MAC、认证解密等还要检查对应 `verification_result`，再读取输出或比较已知期望值。

这些是面向接入者的正确使用要求。当前测试封装 `trigger_service_and_wait()` 对即时失败没有实质处理且无超时，不能直接当作健壮生产实现。

## 6. ISD 关键字段与数据约定

`r_icumif_isd_t` 是 PE 和 ICUM 共享的描述符，双方必须使用一致头文件与编译 ABI：

| 字段 | 说明 |
|---|---|
| `service_id` | 选择服务；决定 `prm` 联合体中有效的参数成员 |
| `requester_id`、`host_int_ch` | 接口层按当前处理器和通信配置设置的请求者及响应中断通道 |
| `vm_id` | 密钥及配置所属 VM；当前演示默认 VM 0 |
| `service_priority`、`job_id` | 任务优先级及作业标识，取值见 API；分段操作应保持正确序列 |
| `req_res_status` | 请求/响应队列状态，供两侧跟踪 |
| `service_result` | ICUM 服务执行结果；`SERV_REQUEST`、`SERV_IN_PROCESS` 为中间态 |
| `req_nointerrupt`、`res_nointerrupt` | 控制请求/响应中断行为；改变后需按对应通信模式处理 |
| `p_callbackfunc` | 可选的 PE 响应回调；同一联合体的 `padding_sys64` 仅用于 ABI 布局 |
| `prm` | 19 种算法参数结构的联合体；只按当前服务 ID 使用一种 |

参数指针不是数据本身：ISD 提交时传的是地址，输入和输出应在正确的共享区；PE 栈、本地 RAM 或普通 ROM 常量不能默认被 ICUM 访问。检查每个服务实现的地址范围、对齐、容量及允许的输入/输出重叠。`volatile` 状态字段也不能替代正确的核间同步和缓冲区所有权。

## 7. 服务、参数和串口菜单

菜单实现位于 [vrun.cmd.c](pe/generic/src/pe1/src/vrun.cmd.c)。输入 `icum` 后选组 1–5，再选子项；`0` 或 `q` 返回。某些子项内部会继续依次测试多种算法模式，因此“单个子服务”不一定只产生一次请求。表中是实际对外服务及主要参数，具体合法值和地址限制以 `r_icumif_api_*.h` 和 ICUM 实现为准。

| 组/子项 | ID | 描述符成员 | 操作和需要准备的参数 |
|---|---:|---|---|
| 1/1 | `0x10` | `AES_CIPHER` | AES ECB、CBC、CFB、OFB、CTR、XTS；方向、模式、key、IV、输入/输出、数据长度和 job |
| 1/2 | `0x11` | `AES_AUTH_CIPHER` | AES GCM、CCM；方向、key、IV、AAD、输入/输出、认证 tag、各段长度 |
| 1/3 | `0x12` | `AES_CMAC` | 生成/验证；key、消息、MAC、长度和验证结果 |
| 2/1 | `0x20` | `RSA_VERIFY` | RSA PSS/PKCS#1 v1.5 验签；消息、签名、编码、哈希、盐长、验证结果 |
| 2/2 | `0x21` | `RSA_SIGN` | RSA PSS/PKCS#1 v1.5 签名；消息、输出签名及尺寸、编码、哈希 |
| 2/3 | `0x22` | `RSA_ENCRYPT` | RSA OAEP/PKCS#1 v1.5 加密；消息、输出密文、编码、哈希、可选 label |
| 2/4 | `0x23` | `RSA_DECRYPT` | RSA 解密；输入密文、输出明文和相应尺寸 |
| 2/5 | `0x24` | `RSA_KEY_GENERATE` | 生成 RSA 密钥；key ID、模数 bit 长度、属性；测试用 2048 bit |
| 3/1 | `0x30` | `RAND_INIT` | 初始化随机数源及在线测试选择 |
| 3/2 | `0x31` | `RAND_EXTEND_SEED` | 提交附加熵数据 |
| 3/3 | `0x32` | `RAND_GENERATE` | 随机数输出、块数和 job slice |
| 4/1 | `0x40` | `ECDSA_VERIFY` | 消息、签名 r/s、长度、哈希、key，检查验证结果 |
| 4/2 | `0x41` | `ECDSA_SIGN` | 消息、输出 r/s、容量、哈希、key |
| 4/3 | `0x42` | `ECDH` | 输入公私钥 ID、输出密钥 ID、KDF、哈希、info 和派生长度 |
| 4/4 | `0x43` | `EDDSA_VERIFY` | 消息、签名分量、长度、key 和验证结果 |
| 4/5 | `0x44` | `EDDSA_SIGN` | 消息、输出签名分量、容量、key |
| 4/6 | `0x45` | `ECC_KEY_GENERATE` | 选择曲线和密钥槽；测试使用 P-256 |
| 5/1 | `0x50` | `HASH` | 哈希类型、输入消息、输出摘要、字节长度和 job |
| 5/2 | `0x51` | `HMAC` | 哈希类型、key、消息、MAC、字节长度、生成/验证操作和验证结果 |

**长度单位特别注意**：

- AES 普通分组模式使用 `nb_blocks`，XTS 使用 `data_size_in_bits`；GCM/CCM 的 IV、AAD、数据和 tag 字段使用 bit。CMAC 数据和 MAC 长度也是 bit。
- RSA 消息和 OAEP label 使用字节；签名/密文相关尺寸字段使用 bit。ECDSA/EdDSA 的签名长度字段为 bit。
- ECDH 的 info 和目标密钥长度使用字节，派生结果写入 ICUM 内部密钥槽，**不通过 ISD 返回明文密钥**。
- HASH/HMAC 的消息和输出长度字段使用字节。RNG 输出使用块数，块的大小和种子格式须以相应头文件和实现为准。

例如现有 AES CBC 测试从 `ISD_BUFFER` 清出 ISD，指定 `SERVICE_01_AES_CIPHER`、`CIPHER_DIR_ENCRYPTION`、CBC、RAM key 0、共享输入/输出和 IV，设置 `nb_blocks=2`、`JOB_ALL_AT_ONCE`；服务结束并检查状态后，才将 32 字节密文与固定期望值比较。完整可运行写法以 [test_aes_cipher_mode](pe/generic/src/pe1/src/pe1_crypto_service_test.c) 为准。

## 8. 密钥选择与测试之间的关系

PE 只指定密钥组和索引；当前 [ICUM 分发规则](icum/generic/src/kernel/k_issd/icum_k_issd_dispatch.c)会强制：

- AES 普通、认证、CMAC 使用 RAM 组并保留请求的 key ID；XTS 固定使用 AES RAM 0 和 3。
- RSA 相关服务使用 ID 0；ECDSA 使用 ECC RAM 0；EdDSA 使用 ECC RAM 1；ECDH 使用 ECC RAM 0 并将派生密钥写入 AES RAM 4；HMAC 使用 ID 0。
- 生成 RSA 或 ECC 密钥会改变各自 0 号槽，从而影响随后的相关测试。复位后演示初始化会重新运行，但实际可用性需检查初始化返回值。

演示槽的物理位置和初始化风险见 [ICUM 工程说明](ICUM_PROJECT_GUIDE.md)。固定演示密钥会进入构建镜像，不适用于生产。

## 9. 测试实现、期望结果与局限

测试集中在 [pe1_crypto_service_test.c](pe/generic/src/pe1/src/pe1_crypto_service_test.c)；`run_service()` 检查单次服务的 `SERV_OK`，`print_test_result()` 分别输出各测试结果，`check_aes_output()` 在固定向量不一致时打印第一个不同字节。

| 测试 | 当前检查 | 注意事项 |
|---|---|---|
| AES ECB/CBC/CFB/OFB/CTR/XTS | 加密固定密文逐字节比较，然后解密并比较原始明文 | 使用预置 AES-128 key；XTS 使用两把 key；固定输入为 32 B |
| AES GCM/CCM | 加密密文和 tag 与固定值比较；解密检查验证结果和明文 | 数据为 16 B；向量及 IV/AAD 见源码 |
| AES CMAC | 生成后调用服务验证 | 不是独立固定 MAC 向量 |
| RSA sign/encrypt | 单次服务状态 | 不能仅据 `SERV_OK` 判定签名/密文正确 |
| RSA verify/decrypt | 先签名/加密，再验证/解密并检查结果 | 往返测试不是独立标准向量 |
| RSA/ECC keygen | 服务返回状态 | 不独立检验生成密钥数学性质；会改变槽 0 |
| RNG init/seed/generate | 服务返回状态 | 不包含随机性统计或熵源认证 |
| ECDSA/EdDSA | sign 单独检查服务状态；verify 先签名再检查验证结果 | 未以独立已知签名向量证明正确性 |
| ECDH | 服务返回状态 | 未对两个独立参与方进行密钥一致性验证 |
| SHA-256 | `abc` 摘要与固定期望摘要比较 | 不覆盖所有哈希模式和边界 |
| HMAC-SHA256 | 生成后调用验证 | 不是独立固定向量；还受当前 HMAC 初始化问题影响 |

`service status: PASS` 只说明对应 ICUM 调用报告 `SERV_OK`；该行**不是**整个算法测试最终通过。后面的密文/tag/明文比较、验证结果以及 `return=... (PASS/FAIL)` 才反映当前测试实际检查项。不同测试项分别输出，不要只看第一行或把多个返回值当作一个结果。

## 10. 中断、状态与耗时

普通通信过程：

```text
PE 准备 ISD → 请求入队 (1) → ICUM 取出 (2)
            → ICUM 完成并响应入队 (3) → PE 响应出队 (4)
```

PE0 普通响应使用 INTC2 EIINT236，硬件 PE1 使用 EIINT540；中断处理转到 `R_ICUMIF_ServiceResponse()`，从队列取 ISD 并调用可选回调。`service_result` 和队列状态是两套信息，必须分别判断。`req_nointerrupt` 的特殊路径在状态 1 即退出当前等待封装，并不表示算法已经执行完成。

`trigger_service_and_wait()` 在提交前读取 OSTM0，返回后读取计数并打印算法/模式名、服务结果、tick 和 `elapsed=... us`。目标配置下 OSTM0 使用 80 MHz：

```text
elapsed_us = ticks / 80
1 tick = 12.5 ns
```

输出包含小数部分；测得的是从提交前到等待返回的时间，包含队列、调度及响应中断开销，不是纯算法硬件执行时间。32 位差值计算只适用于少于一个完整计数周期的间隔，即 80 MHz 下约 53.7 秒；实际时钟与设置不符会直接影响换算。

## 11. 排查与接手注意事项

| 现象 | 建议检查 |
|---|---|
| 没有串口输出 | 烧录镜像、复位、P17_2/P17_3、115200、Option Bytes、RLIN 时钟和启动核 |
| 请求后一直等待 | `R_ICUMIF_ServiceRequest` 返回值、服务可用标志、请求状态 1→2→3→4、队列地址、EIINT236/540、中断使能；当前测试没有超时 |
| 有 `service status: PASS` 但测试 FAIL | 比较后续密文、tag、明文或验证结果；确认输入、key、IV 和长度单位 |
| `SERV_SRC_MEM_ERROR`/目标地址错误 | ISD 及所有缓冲区是否位于可访问 Cluster RAM、对齐和容量是否符合该服务 |
| AES 输出不符 | 确认模式、数据长度、IV、密钥选择、共享区是否被覆盖，并在板上核查异步完成路径 |
| HMAC 失败或槽不可用 | 检查 `MH_KEY_Init` 实际返回值；当前默认配置下 ECC 初始化可能提前返回并跳过 HMAC，详见 ICUM 文档 |
| 第二核启用后结果异常 | 首先检查第 4.3 节共享区重叠；两个 map 的同一 Cluster RAM 地址不是不同核的独立副本 |

当前实现是演示工程，既有共享区重叠、RAM 数据集不持久和初始化错误未上报等限制。修改内存布局或密钥配置后应重新构建、检查 map，并在目标板验证每项算法和中断，不应直接投入生产。
