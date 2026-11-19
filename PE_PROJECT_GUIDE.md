# PE 工程说明（RH850/U2C4）

配套文档：[ICUM 工程说明](ICUM_PROJECT_GUIDE.md)、[工程首页](README.md)。

## 1. 简介与参考资料

PE 工程负责板级初始化、串口命令交互、共享内存准备、ICUM 请求/响应处理、
结果比较和端到端耗时/吞吐率输出。算法工作密钥由 ICUM 管理，PE 只指定组和 ID。

工程名 `pe1` 对应主测试应用，沿用一基命名；芯片手册中的硬件核编号为 PE0、PE1。
不要直接将文件名中的 `pe1` 当作硬件 PE1，也不要把 CPU Local RAM alias 当作共享 RAM。

本说明结合当前 U2C4 代码，并参考：

| 资料 | 对应主题 |
| --- | --- |
| [R11UZ0176EJ0120 Security Services](../r11uz0176ej0120-icumhfw-securityservices.pdf) | §2.3～2.5 服务流程/分段；§3 ISD；§5 算法输入输出 |
| [R11UZ0177EJ0120 Startup Guide](../r11uz0177ej0120-icumhfw-startupguide.pdf) | §5.6～§5.10 串口应用、工程结构、构建与调试 |
| [R11UZ0179EJ0110 Security Service Handler](../r11uz0179ej0110-icumhfw-securityservicehandler.pdf) | §4 API；§5 类型；§6 请求/响应；§7 排他与同步 |

原资料的 U2A 地址、菜单编号、旧服务组不能直接套用到本工程。这里只概括框架原理，
具体枚举、内存布局和输出格式以当前源文件及构建结果为准。

## 2. 代码框架

| 位置 | 内容 |
| --- | --- |
| [top.U2C.gpj](top/u2c/top.U2C.gpj) | ICUM/PE 多核顶层工程、器件配置和测试编译开关 |
| [PE1 应用目录](pe/generic/src/pe1/src) | 通用服务封装、算法用例、串口命令、旧样例函数 |
| [vrun.cmd.c](pe/generic/src/pe1/src/vrun.cmd.c) | `icum` 命令、分组菜单、一键测试路由 |
| [pe1_crypto_service_test.c](pe/generic/src/pe1/src/pe1_crypto_service_test.c) | 当前演示的共享缓冲准备、结果断言、19 项服务入口和完整测试序列 |
| [pe1.c](pe/generic/src/pe1/src/pe1.c) | 系统接口初始化、服务名称、等待响应、时间及 MB/s 统计 |
| [r_icumif.c](pe/generic/src/icumif_lib/r_icumif.c) | 公开 I/F Library 实现 |
| [公开 API](pe/generic/include/r_icumif_pub.h)、[ISD/参数类型](pe/generic/icumif) | 与 ICUM 共用的接口契约 |
| [通信驱动](pe/target/u2cx/src/driver/comm_drv) | 请求/响应队列、ICUP 通知、PE 中断路由 |
| [shared](pe/target/u2cx/src/shared/src) | 队列、ISD 和消息缓冲区定义 |
| [sys.c](pe/target/u2cx/src/l_vrun/sys.c) | 平台系统和计时器配置，OSTM0 频率及时间转换 |
| [链接配置](pe/target/u2cx/config/u2c) | PE 局部 RAM、共享 Global RAM、Flash 及接口段布局 |

目录中仍可能存在没有进入当前菜单或构建的旧密钥导入/Flash/GC 样例；
请以顶层 `.gpj`、实际编译列表和当前命令路由判断是否启用，而不是仅依据文件名。

## 3. 构建、烧录与启动

在工程根目录：

```powershell
& 'C:\ghs\comp_202354\gbuild.exe' -top top\u2c\top.U2C.gpj
```

1. 核对 GHS、器件定义和 RFD 外部路径；这些依赖不是普通 C 头文件可替代的。
2. 构建 [U2C4_firm.mca](top/u2c/U2C4_firm.mca)，同步下载 PE 和 ICUM 对应映像。
3. 核对目标板的 Option Bytes，冷复位并等待固件和接口初始化完成。
4. 按实际 UART 配置连接串口，进入命令提示符，执行 `icum` 或 `icum 6`。

目标时钟为 CPU 320 MHz、`CLK_SBUS` 160 MHz；OSTM0 和 UART 使用的时钟配置仍为 80 MHz。
详细 Option Bytes、寄存器检查和注意事项见 [README](README.md)。
修改项目中的时钟常量并不等于已经改写芯片的 Option Bytes。

## 4. 地址分配与共享数据

以下区间均不包含结束地址。地址来自
[pe1.ld](pe/target/u2cx/config/u2c/pe1.ld)、
[shared.ld](pe/target/u2cx/config/u2c/shared.ld)
和 [PE1 map](pe/target/u2cx/build/out/pe1/pe1.map)。

| 区域 | 范围 | 说明 |
| --- | --- | --- |
| PE1 ROM 脚本区域 | `0x00002000～0x00200000` | 不代表整个区域均可写 PE；须同时避让 ICUM、安全/测试固定段及归档布局 |
| Code Flash B | `0x00400000～0x00600000` | 同样不能覆盖 B 侧 ICUM 启动/固件 |
| shared 启动/常量区域 | 从 `0x08000000` 起，64 KiB 窗口 | 由 shared 子工程链接 |
| 主核 Local RAM 物理区 | `0xFDC00000～0xFDC20000` | 128 KiB |
| 次核 Local RAM 物理区 | `0xFDA00000～0xFDA20000` | 128 KiB |
| 当前核 Local RAM alias | `0xFDE00000～0xFDE20000` | 代码/栈等核私有数据，不用于跨域消息缓冲 |
| Global RAM 通用区 | `0xFE000000～0xFE027800` | 链接脚本划分为 158 KiB |
| 核间同步区 | `0xFE027800～0xFE028000` | 2 KiB |
| PE1 Global RAM 分区 | `0xFE028000～0xFE038000` | 64 KiB |
| PE2 Global RAM 分区 | `0xFE038000～0xFE048000` | 64 KiB |
| ICUM guard 区 | `0xFE06C000～0xFE070000` | 不能任意扩展 PE 数据覆盖该区域 |

当前队列和测试缓冲区由
[que_isd_buf.c](pe/target/u2cx/src/shared/src/que_isd_buf.c)
定义，位于 `.que_isd_buf.bss`：

| 符号 | 本次 map 地址 | 容量/作用 |
| --- | --- | --- |
| `ICUM_QUEUE` | `0xFE003000` | 32 字节：请求/响应队列各 4 个 `uint32_t` 条目 |
| `BIG_BUFFER` | `0xFE003EA0` | `3 × 16384` 字节，放消息、密文、签名、tag 等 |
| `ISD_BUFFER` | `0xFE003020` | 3200 字节，存放服务描述符 |
| `COM_BUFFER` | `0xFE003CA0` | 512 字节 |
| `strbin` | `0xFE00FEA0` | 20096 字节，其他样例使用的转换缓冲 |

本段总容量 72992 字节。表中符号地址为当前构建快照，不是公开 ABI 常量。
PE1 的三个数据缓冲区分别从 `0xFE003EA0`、`0xFE007EA0`、`0xFE00BEA0` 开始。
PE1 段范围为 `[0xFE003000, 0xFE014D20)`，PE2 独立段范围为
`[0xFE015000, 0xFE026D20)`；两段互不覆盖，且均在同步区 `0xFE027800` 之前。
扩容同时调整了两核的链接位置，必须同步更新完整多核固件，不能混用旧布局的 PE 镜像。
变更数组、对齐或链接布局后重新查 map；同时检查 ICUM 内存检查和 guard 允许范围。
ISD 和接口要求对齐的数据应满足 4 字节对齐；结构内部的 64 位 padding 用于双方布局一致，
不意味着可以把任意主机 64 位结构直接传入 RH850。

## 5. 关键结构体与功能接口

### 5.1 描述符与结果

`r_icumif_isd_t` 定义见 [r_icumif_api.h](pe/generic/icumif/r_icumif_api.h)。
常用字段：

- `service_id`：选择 `0x10～0x51` 中的目标服务。
- `prm`：算法参数 union，仅填写与当前服务对应的成员。
- `vm_id`：ICUM 密钥命名空间；不等于 PE 核号。
- `service_priority`、`job_id`、回调字段：调度及完成通知信息。
- `req_res_status`、`service_result`：队列阶段和服务执行阶段，不能互相代替。
- `req_nointerrupt`、`res_nointerrupt`：批量请求/响应控制；不是算法加速开关。

`r_icumif_sts_t` 表示接口可查询的 ICUM 状态；
`R_ICUMIF_CB_REGIST_t` 保存回调函数地址，不包含算法消息或密钥。
内部 `icum_job_t` 和 context 仅供 ICUM 使用，PE 不应伪造其地址。

`pe1_service_response_t` 定义于 [pe1.h](pe/generic/src/pe1/src/pe1.h)，是 PE 栈上的报告快照，
不放入共享描述符：提交前保存 payload 字节数及其有效标记，响应后保存 OSTM ticks，
提交失败则保存原始接口错误。提交/计时与报告逻辑共用这份快照，不改变 OSTM 计时口径；
GPIO 已由 ICUM 算法执行层独立控制，不依赖 PE 报告函数。

### 5.2 I/F Library

定义：[r_icumif_pub.h](pe/generic/include/r_icumif_pub.h)。

| 接口 | 用途 | 注意事项 |
| --- | --- | --- |
| `R_ICUMIF_Init(queue)` | 初始化当前 PE 的请求/响应通信区域 | 服务调用前完成；队列需位于双方可访问区域 |
| `R_ICUMIF_ServiceRequest(isd)` | 校验并提交请求 | 返回 0 仅表示提交成功，不表示算法通过 |
| `R_ICUMIF_ServiceResponse()` | 接收/处理服务响应 | 通常配合通信中断 |
| `R_ICUMIF_IsServiceCompleted(isd)` | 查询指定服务是否完成 | 适合应用自行组织非阻塞流程 |
| `R_ICUMIF_GetStatus()` | 查询状态结构 | 不替代单个请求的结果字段 |
| `R_ICUMIF_SetSystemCallBackFunc()` | 注册系统事件回调 | 与算法完成回调不同 |
| Flash 状态/回调 API | 原框架的 Flash 排他协议 | 当前 RAM 密钥测试无需把它当作导入步骤 |

`R_ICUMIF_ServiceRequest()` 会填写 requester、中断通道和初始请求状态，并进行提交。
入队失败必须处理 API 返回值；不能进入“等一个从未提交的请求”的死循环。

### 5.3 本工程封装

| 接口 | 职责 |
| --- | --- |
| `trigger_service_and_wait()` / `trigger_service_and_wait_quiet()` | 普通封装保持原行为；quiet 不打印正常统计，但仍报告错误 |
| `trigger_service_and_wait_deferred()` | 提交并等待最终响应，保存报告快照，不做串口输出；调用者随后必须报告结果 |
| `print_service_response()` | 根据快照输出统计或接口/服务错误；保留 RSA 的 ms/ops 及无 payload 不打印 throughput 的规则 |
| `icum_crypto_service_test_service(id)` | 执行指定服务入口对应的所有现有用例 |
| `icum_crypto_service_test()` | 按服务 ID 升序运行全部 19 个入口并汇总 |
| `test_isd()`（文件内部） | 清空并取得共享描述符，避免残留参数 |
| `prepare_rng()`（文件内部） | 初始化 RNG、扩展种子；失败不继续 RSA/ECC 准备 |

## 6. 请求流程与资源所有权

```text
串口命令
  -> 选择服务/算法测试
  -> 如有需要，先准备 RNG
  -> 准备 BIG_BUFFER，再清空并填写 ISD_BUFFER
  -> trigger_service_and_wait（准备加密使用 quiet）
       -> 保存 payload 大小，记录 OSTM0 起始值
       -> R_ICUMIF_ServiceRequest
       -> ICUM 判断服务、检查参数、读取密钥、获取资源和准备输入
       -> ICUM 算法执行层拉高 P17_0
       -> 执行完整密码学操作，异步/分阶段计算保持高电平
       -> ICUM 算法执行层拉低 P17_0
       -> ICUM 转换/封装结果、提交派生密钥、最终释放资源
       -> 响应队列 / PE 中断 / R_ICUMIF 响应处理
       -> 确认最终状态后读取 OSTM0
       -> 输出耗时、速率或错误
  -> 对比输出数据和 verification_result
  -> 输出该测试 PASS/FAIL
```

注意事项：

- 请求未完成时，不可重填共享 ISD 或覆盖消息、输出及签名区。
- RNG 准备本身也使用共享区，所以必须在构造最终 RSA/ECC 消息和描述符之前进行。
- 校验类服务的 `SERV_OK` 不表示数据合法；仍须检查 `VERIFICATION_PASS/FAIL`。
- 原库支持延迟发中断的请求合批，但当前同步等待封装不应当作一般异步批处理调度器。
- U2C4 通信驱动配置主核/次核响应通道为 INTC2 236/540；
  以 [通信驱动](pe/target/u2cx/src/driver/comm_drv/icum_d_comm_pe.c)和实际目标核为准。
- 用于队列的排他和流水线同步遵循接口库约定；`volatile` 本身不能替代同步和内存访问权限配置。

## 7. 密钥存储及使用规则

PE 不需要先注册或导入密钥。冷启动后 ICUM 建立演示密钥：

| 测试 | ICUM 侧选择 |
| --- | --- |
| AES cipher、GCM/CCM、CMAC | AES RAM 0；其他预留演示 key 位于 1/2 |
| AES-XTS | AES RAM 0 与 3，两把不同 key |
| RSA | ID 0，初始 2048 位；`0x24` 后使用生成的新 RAM key |
| ECDSA | ECC RAM 0，P-256 |
| EdDSA | ECC RAM 1，Ed25519 |
| ECDH | ECC RAM 0 与 2，派生到 AES RAM 4 |
| HMAC | HMAC RAM 0，RFC 4231 演示 key |

详细 key 数据结构、引用计数、RAM 配置及生成安装路径见 [ICUM 说明](ICUM_PROJECT_GUIDE.md)。
一键测试包含 `0x24` 与 `0x45`，会替换对应的 RAM key；这不是只读测试。
冷启动恢复演示初值，不提供密钥掉电保存保证。所有固定 demo key 均不得用于生产。

## 8. 菜单与一键测试

在命令行直接输入：

```text
icum 6
```

也可以输入 `icum` 后选择 `6`；或者进入 `icum 308` 的分组菜单后选择 `6`。
原有 `1～5` 分组及各单项菜单保持不变。

顶层测试入口严格按以下顺序：

```text
0x10 0x11 0x12
0x20 0x21 0x22 0x23 0x24
0x30 0x31 0x32
0x40 0x41 0x42 0x43 0x44 0x45
0x50 0x51
```

**这是测试入口的顺序，不是全部底层请求日志按数值递增。**
例如 `0x20` 验签测试需要先准备 RNG 并生成合法签名，ECDH 用例会额外执行 AES，
签名测试也会执行正常与篡改验签。这些依赖步骤保持原有行为。

覆盖范围与判定：

| 入口 | 现有菜单测试范围 |
| --- | --- |
| `0x10` | ECB/CBC/CFB/OFB/CTR/XTS；前 32 字节固定密文比较与完整 10KB 解密恢复 |
| `0x11` | GCM/CCM；前 16 字节密文、完整 tag 及 10KB 解密/认证检查 |
| `0x12` | CMAC 生成与校验 |
| `0x20/0x21` | RSA-PSS 和 PKCS#1 v1.5 签名/验签；使用 SHA-256 |
| `0x22/0x23` | RSA-OAEP 和 PKCS#1 v1.5 加解密与消息比较 |
| `0x24` | 2048 位 RSA keygen |
| `0x30～0x32` | RNG 初始化、扩展种子、生成数据及现有基本检查 |
| `0x40/0x41` | P-256 ECDSA 签名、正常验签、修改消息后拒绝 |
| `0x42` | 双向 ECDH，将两次派生 key 用于同一 AES 块并比较结果，不导出 shared secret |
| `0x43/0x44` | Ed25519 签名、正常验签、修改消息后拒绝 |
| `0x45` | P-256 ECC keygen |
| `0x50` | SHA-256 对 10KB 消息的完整参考摘要比较 |
| `0x51` | RFC 4231 演示 key 对 10KB 消息的 HMAC-SHA256、参考 tag 比较、正常校验及修改 tag 后拒绝 |

带消息的菜单用例使用同一份 **10KB = 10240 字节**二进制数据作为数据源：将
`s_test_message_pattern[32]` 重复 320 次写入 `BIG_BUFFER[0]`，不计字符串终止符。
AES 普通模式输入为 640 个块，XTS、GCM/CCM、CMAC 的消息长度为 81920 位；
GCM/CCM 的 AAD 仍为 16 字节，IV/tag 不计入这 10KB。
RSA/ECDSA/EdDSA 签名验签和 SHA-256/HMAC-SHA256 均处理完整 10240 字节。
ECDSA/EdDSA 的篡改测试修改消息最后一个字节，以检查消息尾部也参与认证。
RNG、密钥生成、ECDH 及其辅助 AES 单块比较保持原有长度。

RSA-2048 加解密只处理一个合法填充块，不再遍历全部 10KB；ICUM 服务接口不变：

| RSA 编码 | 从 `BIG_BUFFER[0]` 首地址取明文 | 密文/解密输入 |
| --- | --- | --- |
| OAEP-SHA256 | `256 - 2 × 32 - 2 = 190` 字节 | 256 字节 |
| PKCS#1 v1.5 | `256 - 11 = 245` 字节 | 256 字节 |

上述是当前 2048 位密钥及填充模式的最大合法明文长度，并不是所有 RSA 模式共用固定长度。
明文直接引用 `BIG_BUFFER[0]` 首地址，密文放在 `BIG_BUFFER[1]` 首地址，
解密结果放在 `BIG_BUFFER[2]` 首地址，无 PE 分块/重组暂存区。
加密入口只执行一次加密；解密入口先静默准备一份密文，再执行一次解密，
检查返回长度为 190/245 字节，并与原始消息对应前缀比较。
PSS/PKCS#1 v1.5 签名、验签仍从同一首地址处理完整 10KB 消息；
签名标准未规定固定的原始消息长度，不把 SHA-256 的 32 字节摘要长度当成消息长度。

本次一键入口复用现有用例，不自动扩展到所有 HASH/HMAC 变体、曲线、密钥长度和分段组合。
默认所有编译开关开启时，19 个入口产生 29 个顶层用例结果。日志示例：

```text
[1/19] service 0x10
...
[19/19] service 0x51
ICUM test summary: passed=29 failed=0 skipped-services=0 (PASS)
```

- 某个用例失败后继续后面的模式/服务，最终汇总为 `FAIL`。
- `passed/failed` 统计 `print_test_result` 输出的用例，不是内部请求数量。
- 关闭 AES/RSA 编译开关的入口计入 `skipped-services`，不会假报为 PASS。
- 没有失败但存在禁用入口时，汇总为 `INCOMPLETE`。
- 这一继续执行机制不处理硬件挂死：若 ICUM 不响应，现有同步等待仍需调试定位。

## 9. 耗时、ms/ops 与 MB/s 口径

每条服务结果不再打印原始 ticks，但内部仍使用 OSTM0 原始计数作精确换算。
保留 `elapsed=... us`。RSA 服务（含 keygen）另外打印 `latency=... ms/ops`，
每个请求为一次 operation，采用该次服务耗时毫秒数，不计算 MB/s。
没有 payload 的服务均不打印 `throughput` 字段；RSA keygen 仍可显示操作耗时。
其他有 payload 的算法打印 `throughput=... MB/s`，单位为十进制 MB：

```text
MB/s = 有效载荷字节数 / 耗时秒数 / 1,000,000
     = 有效载荷字节数 × VTIMER0_CLK(MHz) / 计数差
```

输出保留 6 位小数，使用 64 位整数定点计算并截断小数尾数，不依赖浮点打印。
例如 32 字节、80 MHz 时钟、160 个内部计数，对应 2 微秒和 `16.000000 MB/s`。

| 服务 | 计量数据量 |
| --- | --- |
| AES 非 XTS | 输入块数 × 16 字节 |
| XTS、AES AUTH、CMAC | 相应 payload 位数向上取整到字节 |
| AES GCM/CCM | 仅计算 payload；不累计 AAD、IV 和 tag |
| ECDSA/EdDSA 签名、验签 | 原始输入消息字节数，不累计签名、密钥或填充 |
| RSA sign/verify/encrypt/decrypt/keygen | 不计算 MB/s；`ms/ops = 请求耗时秒数 × 1000` |
| HASH/HMAC | 输入消息字节数，不累计摘要/tag/key |
| RNG generate | 输出块数 × 16 字节 |
| ECC keygen、RNG init/extend-seed、ECDH | 只打印结果和 elapsed，不打印 throughput |

计量长度在提交前读取，避免服务写回输出长度后改变统计口径。耗时范围为
**提交前到同步等待收到最终响应**，包含排队、调度、中断及接口开销，不包含结果打印时间。
因此它是端到端服务吞吐率，不是纯硬件引擎吞吐率。
RSA 每个选中方向/填充模式仅执行并统计一次操作；
解密前的准备加密不打印正常统计，也不计入解密的 ms/ops。
统计不包含 PE 数据校验和打印耗时，因此不是整个测试函数的墙钟耗时。
ms/ops 保留 6 位小数，采用 64 位商/余数换算，避免直接乘大计数造成溢出。

异常情况：

- RSA 服务未成功：`latency=N/A (service not successful)`；
  其他有 payload 的服务为 `throughput=N/A (service not successful)`，无 payload 的服务不打印 throughput。
- OSTM0 时钟未初始化：耗时与速度均为 `N/A`。
- 计数差为 0：速度显示 `N/A (timer resolution)`，不会除零。
- 合法空消息：数据量为 0，速度显示 `0.000000 MB/s`，不意味着服务未执行。
- 32 位计数差允许跨过一次边界，但单次测量须短于完整计数周期；
  80 MHz 时约 53.7 秒，超过该长度无法由现有两次采样可靠恢复。

非 RSA 签名、验签、小消息的 MB/s 易受固定开销影响；不能把不同消息长度、不同准备流程或
调试暂停后的测量值当作统一性能基准。真实速度必须在目标板、正确时钟和连续运行条件下采集。

### 9.1 P17_0 逐操作波形

P17_0 完全由 ICUM 的算法执行路径调用 `K_ISSD_BeginAlgorithm` / `K_ISSD_EndAlgorithm` 控制，
PE 不再发送 BEGIN/END 通知，也不操作引脚；原演示控制服务 `0x01` 已移除。
启动空闲低，服务层判断、参数校验、输入准备及资源获取期间保持低，进入密码算法核心后拉高，
核心计算完成后、结果封装/转换及核间响应前拉低。
AES-ECB 加密和解密各一个脉冲；CMAC/HMAC 生成、正常校验、负向校验（如有）各一个脉冲。
RSA 解密测试的准备加密、正式解密也各一个脉冲，准备 RNG 的请求同样独立标记。
签名、正常验签、篡改数据验签及 ECDH 的各次请求均分别标记。
同一请求内部的异步 pending/重入不增加脉冲。默认 `icum 6` 的 29 项顶层结果保持不变，
脉冲数按实际进入算法核心的操作数计；禁用测试、提交失败或前置检查失败不产生脉冲。
标记状态只存放于 ICUM 内部作业，不再使用共享 ISD 的第 1 个槽，不改变共享物理布局。

算法正常完成或已返回错误后回低；异步计算真正完成前不得结束脉冲或释放其现场。
未启用芯片实际 `ICUM_OPBT4.ICUMOPGPIO` 时，ICUM 拒绝算法服务并返回 `SERV_UNAVAILABLE`，
PE 通过现有错误输出报告，不会跳过标记后仍报告 PASS。
须保留 P17_0 的 `PM17_0=1`、`PSFC17_0=0`，不能与其他端口功能/Safe State 共用。
GPIO 不包含核间握手、服务层前后处理或串口打印。OSTM 仍统计 PE 提交到最终响应的端到端耗时，
包含服务层开销，不能直接与 GPIO 等同。
测量口径是一个完整算法操作，而非每次底层硬件启动：RSA/ECC 内部必要的 HASH、填充、KDF、
驱动初始化、内部搬运和多阶段运算计入同一个脉冲；不是纯硬件 BUSY 时间。
RNG 初始化从种子/secret key 准备完成后执行 `ML_PRNG_InitSHE` 时开始标记，不计前置 TRNG 种子收集。
AES all-at-once 在驱动调用前开始，异步结束沿在最终 AES 回调入口产生；
算法输出的服务层比对/转换、context/资源释放、PE 校验均在之后。
具体各算法边界见 [ICUM 标记说明](ICUM_PROJECT_GUIDE.md#72-p17_0-逐操作标记)。
默认 PE 串行测试每项操作一个脉冲；若不同作业并发重叠，单引脚显示活动窗口的并集。
硬件挂死/失去响应时不会伪造完成沿，P17_0 可能一直保持高电平。

## 10. 验收与故障定位

1. 冷启动后执行 `icum 6`，检查入口按顺序出现，默认 29 PASS，无 skipped。
2. 确认服务行没有 ticks，RSA 有 `ms/ops`、无 `MB/s`；其他有 payload 的服务有 `MB/s`，无 payload 的服务无 throughput。
3. 再运行一遍，验证生成新 key 后的复用、context 及引用释放。
4. 单独复测 `0x24` 后的 RSA 各模式、`0x45` 后的 ECDSA/ECDH。
5. 对于负向验签/HMAC，确认是 `verification_result=FAIL`；不要把“执行成功”误认为“数据可信”。
6. 示波器观察 P17_0：加密/解密、签名/验签、MAC 生成/校验及准备请求分别产生脉冲，普通失败后仍应回低。

定位时先区分三层结果：

| 层次 | 例子 | 含义 |
| --- | --- | --- |
| 提交接口 | `R_ICUMIF_ER_REQ_QUEUE_FULL` | 请求未正常提交，不能等待算法返回 |
| 服务执行 | `SERV_KEY_SLOT_EMPTY`、`SERV_CRYPT_ERROR` | ICUM 参数/密钥/算法执行失败 |
| 算法判定 | `VERIFICATION_FAIL`、密文/摘要比对不符 | 服务可能执行成功，但测试或数据认证未通过 |

若 PE 等待不返回，应同时查看 ICUM 第一异常现场、请求/响应队列、描述符状态和 INTC2 路由。
若收到正常响应却测试失败，应比较实际输出、数据长度/单位、选用的 key、HASH 和编码模式。
仿真测试可以验证格式、统计和流程；含外设替代的结果不能替代板上算法正确性及吞吐率验收。
