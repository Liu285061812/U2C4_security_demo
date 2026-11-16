# 当前算法测试输入与预期结果

本文按当前 [PE 测试代码](pe/generic/src/pe1/src/pe1_crypto_service_test.c)和 ICUM 演示密钥整理，适用于默认 VM 0、默认演示密钥及当前测试配置。代码中实际选择的 key 可能被 [ICUM 分发器](icum/generic/src/kernel/k_issd/icum_k_issd_dispatch.c)覆盖。下列值不代表目标板已实测通过；复位后初始化失败、生成新密钥或修改源码时，实际值可能不同。

**十六进制均按源码数组字节顺序从左到右排列**，每两位表示一个字节；长值分行连接。字符串使用 ASCII，不含结尾的 `00`。`—` 表示算法不使用该参数。表中“固定值比较”表示测试代码实际执行该比较；“软件参考值”仅供排查，并不是当前代码内置的判定值。

这些是公开、固定的演示密钥，禁止用于生产。已知 ECC 初始化返回错误可能导致 ECC 后续初始化及 HMAC 初始化被跳过，且启动处忽略该错误；见 [ICUM 工程说明](ICUM_PROJECT_GUIDE.md)。密钥生成测试请求写入 RSA/ECC 0 号槽，成功后会改变后续测试所用密钥；应检查实际返回值。

## 1. AES 共用输入

来源：[PE 测试代码](pe/generic/src/pe1/src/pe1_crypto_service_test.c)和 [AES 演示密钥](icum/generic/src/middle/mh_key/icum_mh_key_aes.c)。

| 标识 | 长度 | 值 |
|---|---:|---|
| `K0`，AES RAM ID 0 | 16 B | `000102030405060708090a0b0c0d0e0f` |
| `K3`，AES RAM ID 3 | 16 B | `202122232425262728292a2b2c2d2e2f` |
| `IV16` | 16 B | `000102030405060708090a0b0c0d0e0f` |
| `IV12` | 12 B | `000102030405060708090a0b` |
| `P32` | 32 B | `6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e51` |
| `P16`，`P32` 前 16 字节 | 16 B | `6bc1bee22e409f96e93d7e117393172a` |
| `AAD` | 16 B | `50452d4943554d2d746573742d414144`（`PE-ICUM-test-AAD`） |

`K0` 用于全部 AES 测试；XTS 还使用 `K3` 作为第二把密钥。下表 `IV16` 对 XTS 是 tweak 输入、对 CTR 是初始计数块；ECB 的测试虽然传入 `IV16` 指针，算法不使用 IV。所列 IV 是每次加密请求前复制到共享区的初始值。

## 2. AES 分组加解密

加密均输入 `P32`，随后将实际密文作为解密输入。测试先比较下表固定密文，再比较解密结果 `P32`；密文不符时提前返回，不再进行该模式的解密。

| 模式 | Key | IV / tweak | 加密输入 | 固定预期密文，32 B |
|---|---|---|---|---|
| ECB | `K0` | — | `P32` | `47c58d5e21caaf840d015b7d9b9109815c051c31e4a777747c38eba4dc62e073` |
| CBC | `K0` | `IV16` | `P32` | `e4ef93eb8ef9a7424709f8eaa953450ee74718c9bf0a2a6dccc406d686f4fce8` |
| CFB | `K0` | `IV16` | `P32` | `6155b5576f2e6fd318feea49b5c0fd70be25fd7147a3876066e7950530562a9b` |
| OFB | `K0` | `IV16` | `P32` | `6155b5576f2e6fd318feea49b5c0fd7000ca94f25fd402d775d7d1601c903832` |
| CTR | `K0` | `IV16` | `P32` | `6155b5576f2e6fd318feea49b5c0fd70ac4e66c3781bde0a046d92a30e0b818d` |
| XTS | `K0` + `K3` | `IV16` | `P32` | `fd296f99b87f2927edb5a7b383751dd84bd61d86daec381d9f67b14680d4d67e` |

普通模式的 ISD 长度为 `nb_blocks=2`；XTS 使用 `data_size_in_bits=256`。参考软件复现 CFB 时采用 128 位反馈段。

## 3. AES 认证加密和 CMAC

### 3.1 GCM、CCM

两个模式都以 `K0`、`P16` 和 `AAD` 加密，数据、AAD 和 tag 均为 128 bit；GCM IV 为 128 bit，CCM nonce 为 96 bit。测试实际比较密文和 tag，再将生成的密文/tag 用于解密，并检查 `VERIFICATION_PASS` 和解密结果 `P16`。

| 模式 | Key | IV / nonce | 输入 | AAD | 固定预期密文，16 B | 固定预期 tag，16 B |
|---|---|---|---|---|---|---|
| GCM | `K0` | `IV16` | `P16` | `AAD` | `c1692d7cb7c256551885697ff7ecbfd5` | `4bd711e31d01772007af1f568d368117` |
| CCM | `K0` | `IV12` | `P16` | `AAD` | `58d44d85f2c05b27982eb7f1717992aa` | `529708e2a8c3827d88b054f8016b2a68` |

### 3.2 CMAC

| 模式 | Key | IV | 消息 | 输出长度 | 当前测试判定 | 软件参考 tag |
|---|---|---|---|---:|---|---|
| AES-CMAC 生成/验证 | `K0` | — | `P32` | 16 B | 生成返回 `SERV_OK`；用刚生成的 tag 验证，要求 `VERIFICATION_PASS` | `2922def995eb803b1478f76bc7ad659c` |

软件参考 tag 用标准 AES-CMAC 独立计算；当前 PE 测试**没有**将生成结果与该固定值比较。

## 4. RSA-2048

### 4.1 默认演示密钥和消息

来源：[RSA 演示密钥](icum/generic/src/middle/mh_key/icum_mh_key_rsa.c)。测试通过 RSA ID 0 使用该槽，密钥数据不是由 PE 在服务请求中传入。

| 参数 | 默认源数组内容 |
|---|---|
| `N` 模数，前 256 B | `b10cfc21204e3874b94454b2c14cf10f641d91efcf5e8bb65551c9041cd643f5`<br>`8dff87a56b67cd3626e8717bfe19be936460b575f364bd4b39f7fa458b14ab71`<br>`dc976b1cc91b7fd1973a0f9c20ce337b6f0433852ffb8d0a77cf8a6dea718908`<br>`135013052ba7a7e88b5b6c60f5c90f5ebdeeac985cb2ed2eccc8fdc52f251268`<br>`b5158b98024174aa86e7478d7547be1fc8a8defa917755ad312e305c2a8dd76e`<br>`0d0d81f1043af4e66b097bbfa8d0f864428a720e62bd87a9a1eb18eef0849937`<br>`c8af5ebcc45230184a7d9cfcd1fa13b7886f6df13e7b75584516a8d1758ae61b`<br>`534b6e37b1902856b5325223d3a6ff00fabbcdf3fb97d0a6a95da348ee6c4635` |
| `D` 私钥指数，前 256 B | `184d93fe6814d4a3111b5415057bae4b528ecc8d828593dfff6e2e8da5f7b374`<br>`6d7d79fd36f514b22e3d11f6d736697689fccc6bd871279abfe64e30659afcbd`<br>`b06e0881f709134e8dd7a674e3fbea0d337bfc267823ab849855a0c6ddbb9ff7`<br>`bfceea0d77a497e3cfdd48cf366dd6f2465810d10948764e4a45fcafa87d0a3d`<br>`a2bd3890c894c6826ef8310d812c97cb10b2d660ea38de3b08213e5f7a5a5a07`<br>`ea528a83e7ec37ab920967233514c6030fff6727c78737bf55cdedde11839c50`<br>`67eadb510e048165ee129ab9971665f7dd4694934b22b36ba87d0d81df952fec`<br>`b5cc856ba94e62d6575f13bd13018193139266619c1fbf00cae3e08380a792b1` |
| `E` 公钥指数源数组 | `010001` 后跟 13 个 `00` 字节，数组共 16 B |
| 消息 `M_RSA`，21 B | `4943554d2052534120736572766963652074657374`（`ICUM RSA service test`） |

当前构建 `N`、`D` 的数组容量均为 512 B，源码只显式初始化上表前 256 B，其余 256 B 由 C 初始化为 `00`；`E` 同理补零到 16 B。初始化代码设置模数位数 2048，但也将相应大小字段设为数组容量，排查字节序和库接口时不能忽略这些实际字段。

### 4.2 测试模式和判定

| 模式 / 服务 | Key | IV | 输入和参数 | 当前测试预期结果 |
|---|---|---|---|---|
| RSA-OAEP 加密 `0x22` | RSA 0 公钥 | — | `M_RSA`，SHA-1，label 为空，密文容量 2048 bit | 仅检查 `SERV_OK`；不比较固定密文 |
| RSA-OAEP 解密 `0x23` | RSA 0 私钥 | — | 先用同一消息和公钥加密，再用生成密文解密；SHA-1，label 为空 | `SERV_OK`，明文长度 21 B，内容等于 `M_RSA` |
| RSA-PKCS#1 v1.5 加密 `0x22` | RSA 0 公钥 | — | `M_RSA`，编码 EME-PKCS1-v1_5，密文容量 2048 bit | 仅检查 `SERV_OK`；不比较固定密文 |
| RSA-PKCS#1 v1.5 解密 `0x23` | RSA 0 私钥 | — | 先加密，再输入生成密文解密 | `SERV_OK`，明文长度 21 B，内容等于 `M_RSA` |
| RSA-PSS 签名 `0x21` | RSA 0 私钥 | — | `M_RSA`，SHA-1，salt 长度 20 B，签名容量 2048 bit | 仅检查 `SERV_OK`；不比较固定签名 |
| RSA-PSS 验签 `0x20` | RSA 0 公钥 | — | 先对 `M_RSA` 签名，再验证生成的签名，其他参数同上 | `SERV_OK` 且 `VERIFICATION_PASS` |
| RSA-PKCS#1 v1.5 签名 `0x21` | RSA 0 私钥 | — | `M_RSA`，SHA-1，EMSA-PKCS1-v1_5，salt 长度 0 | 仅检查 `SERV_OK`；不比较固定签名 |
| RSA-PKCS#1 v1.5 验签 `0x20` | RSA 0 公钥 | — | 先对 `M_RSA` 签名，再验证生成的签名 | `SERV_OK` 且 `VERIFICATION_PASS` |
| RSA 密钥生成 `0x24` | 目标 RSA 0 | — | 模数 2048 bit，其余参数为清零后的默认值 | 仅检查 `SERV_OK`；没有固定生成密钥 |

OAEP、PKCS#1 v1.5 加密及 PSS 包含随机材料，不能仅凭固定消息和 key 指定唯一密文/签名。PKCS#1 v1.5 签名在固定标准参数下可确定，但当前测试没有固定签名断言。

### 4.3 PKCS#1 v1.5 签名的软件参考

将上表 `N`、`D` 的前 256 B 按大端整数、`E` 按 `010001`、消息按 `M_RSA` 解释，独立软件计算的 SHA-1 / RSASSA-PKCS1-v1_5 签名为：

```text
6c6143d3dd18b498e76da6387b3e2c3f2e64e1032a40b1e6945d645af735cba8
2d77dfa328f1e0b3d0f327c07eb51eba9c196c00dfe665f2b775b22693121b62
7ea44bdf64ab3550248cdcfd480cc6ed9d0eced2f0fbbbb23c18d506fa75b7e1
ff56b357b2f6b5de5b8d046ea77d859b842f2e6383175706e223598f8405b926
8133da09dc37a706756f3d2b5cd99d298ce589e2f1fd7dd8a68cf274100ba5a9
7a47d5432926807057ca0befe060af9e5b15b181844c582b6a0e30a70e7776d4
2eb568eac951faa01199304e28ddad109b777faa059007049ca0cecd4fc7d4d1
46ab6ea1bc1a1852f1b6ddbfe657e9480070769ff7e5475729c25eb2fab8872d
```

此值不是 PE 代码的预期数组，也不是板级实测。工程里的数组容量、补零、大小字段及输出格式可能影响固件接口如何解释参数，比较前必须先核对这些约定。

## 5. 随机数

| 服务 | Key / IV | 输入 | 当前测试预期结果 |
|---|---|---|---|
| RNG 初始化 `0x30` | — / — | `online_test=RAND_DIAG_P1_TEST` | `SERV_OK` |
| RNG 扩展种子 `0x31` | — / — | 先初始化，再输入 16 B 熵 `ae2d8a571e03ac9c9eb76fac45af8e51` | `SERV_OK` |
| RNG 输出 `0x32` | — / — | 先初始化和扩展上述种子，`nb_blocks=1`、`job_slice=0` | `SERV_OK`；请求 1 个 128-bit 块，不比较固定随机字节 |

输出受随机源和内部状态影响；该测试不进行统计质量检验。

## 6. ECC、ECDSA、EdDSA 和 ECDH

### 6.1 默认演示密钥和消息

来源：[ECC 演示密钥](icum/generic/src/middle/mh_key/icum_mh_key_ecc.c)。下面是源码数组，不保证初始化失败时对应槽已可用。

| 参数 | 长度 | 值 |
|---|---:|---|
| ECC RAM 0，P-256 私钥 | 32 B | `0000000000000000000000000000000000000000000000000000000000000001` |
| ECC RAM 0，P-256 公钥 X | 32 B | `6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296` |
| ECC RAM 0，P-256 公钥 Y | 32 B | `4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5` |
| ECC RAM 1，Ed25519 私钥数组 | 32 B | `9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60` |
| ECC RAM 1，Ed25519 公钥数组 | 32 B | `d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a` |
| `M_ECC` | 21 B | `4943554d2045434320736572766963652074657374`（`ICUM ECC service test`） |

### 6.2 测试模式和判定

| 模式 / 服务 | Key | IV | 输入和参数 | 当前测试预期结果 |
|---|---|---|---|---|
| ECDSA P-256 签名 `0x41` | ECC RAM 0 私钥 | — | 先准备 RNG；`M_ECC`，SHA-256，签名两分量各 256 bit | 仅检查 `SERV_OK`；不比较固定签名 |
| ECDSA P-256 验签 `0x40` | ECC RAM 0 公钥 | — | 先对 `M_ECC` 签名，再用生成的两分量验证 | `SERV_OK` 且 `VERIFICATION_PASS` |
| EdDSA Ed25519 签名 `0x44` | ECC RAM 1 私钥 | — | 先准备 RNG；`M_ECC`，ISD 选择 SHA-512、非预计算哈希，两分量各 256 bit | 仅检查 `SERV_OK`；不比较固定签名 |
| EdDSA Ed25519 验签 `0x43` | ECC RAM 1 公钥 | — | 先对 `M_ECC` 签名，再用生成的两分量验证 | `SERV_OK` 且 `VERIFICATION_PASS` |
| ECDH + KDF1 `0x42` | ECC RAM 0 的私钥和公钥 | — | KDF1、SHA-256、`counter=4`、info 为空、输出长度 16 B，目标 AES RAM 4 | 仅检查 `SERV_OK`；不读取或比较派生 key |
| ECC 密钥生成 `0x45` | 目标 ECC RAM 0 | — | 先准备 RNG，曲线 P-256，`disable_on_debug=0`、`write_protected=0` | 仅检查 `SERV_OK`；不比较固定生成密钥 |

ECDH 示例在本核同时使用同一个演示槽的公私钥，并非两个独立参与方之间的交换。ECDSA 签名涉及随机性；Ed25519 的标准签名可确定，但现有测试也没有固定签名断言。

### 6.3 Ed25519 的软件参考

以 ECC RAM 1 的私钥数组为 RFC 8032 Ed25519 seed、消息为 `M_ECC`，独立软件计算得到 64 B 签名：

```text
63e1219cac593a13f451a2aed0b6540be040b59762496c1e9d66b8cb7c7dc75d
8d9fdc2628485628f5fb89746b0ea922afa42dd42f57a8b7eb413ec0a32dd507
```

这是标准签名字节串的软件参考，不是 PE 测试的固定期望数组；与固件两个签名分量比较前须核对固件的数据格式、字节顺序和算法实现，并在板上验证。

## 7. SHA-256 和 HMAC-SHA256

| 模式 | Key | IV | 输入 | 预期结果和当前判定 |
|---|---|---|---|---|
| SHA-256 `0x50` | — | — | `616263`（`abc`，3 B） | 固定摘要 `ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad`，测试实际逐字节比较 |
| HMAC-SHA256 `0x51` | HMAC 0，见下表 | — | `4943554d20484d414320736572766963652074657374`（`ICUM HMAC service test`，22 B） | 先生成 32 B tag，再验证刚生成的 tag；要求 `SERV_OK` 和 `VERIFICATION_PASS`，不比较固定 tag |

| HMAC 0 演示源数组 | 长度 | 值 |
|---|---:|---|
| 前 32 B 显式初始化 | 32 B | `4d9ab12763e8057c11f236a0c5489d72830bd641fa256e19b7503cd28f64a90e` |
| 后 32 B | 32 B | 由于数组长度为 64 B 且未显式给值，自动初始化为全 `00` |

[HMAC 初始化代码](icum/generic/src/middle/mh_key/icum_mh_key_hmac.c)使用整个 **64 B** 数组作为密钥；如果初始化成功，标准 HMAC-SHA256 软件参考值为 `18523a296c4586e02dc86d42653854ca945b349f92c66da70822e32de565e16e`。此值目前没有被 PE 测试比较，也不保证当前固件初始化路径能够成功加载该密钥。

## 8. 如何使用这些表

1. 先确认 ICUM 启动、密钥槽可用，且未执行改变 RSA/ECC 0 号槽的生成操作；参考 [PE 工程说明](PE_PROJECT_GUIDE.md)中的内存及测试限制。
2. 选择菜单子服务并记录 `service_result`、验证结果和输出；加密服务的成功状态不等于已通过数据比较。
3. AES 和 SHA-256 可直接对照上表中源码内置的固定值。CMAC、HMAC、RSA 确定性签名和 Ed25519 的软件参考仅供额外人工核对；当前测试不会因与这些参考值不符而自动失败。
4. RSA 随机填充、ECDSA、RNG 和密钥生成无固定输出；应核对参数、验证结果或往返结果，不能拿某次运行结果当固定向量。
