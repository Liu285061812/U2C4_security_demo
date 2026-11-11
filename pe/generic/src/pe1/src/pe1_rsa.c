/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
/******************************************************************************/
/* Product      : ICU-M Firmware                                              */
/******************************************************************************/
/*******************************************************************************
* Copyright(C) 2021-2022 Renesas Electronics Corporation.                       
*                                                                               
*          RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY                     
*                                                                               
* These instructions, statements, and programs are the confidential information 
* of Renesas Electronics Corporation. They must be used and modified solely for 
* the purpose for which it was furnished by Renesas Electronics Corporation.    
* All part of them must not be reproduced nor disclosed to others in any form,  
* without the prior written permission of Renesas Electronics Corporation.      
*******************************************************************************/

// ********************************************************************************
// Headers
// ********************************************************************************

#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe1.h"
#include "shared.h"

#include "vrun.api.h"
#include "pe_target_device.h"

// ********************************************************************************
// Public constants
// ********************************************************************************

// ********************************************************************************
// Local constants
// ********************************************************************************
#include "pe1_rsa_keys.h"

const uint8_t *SIG = (void *)
    "7614839A495D8E96233454A0DF9FCEB8AC9A5BCA8E97430D11AC72330ACC708A"
    "A16B0B745A85CAA575685CF19D10C3815E6743408AFD8DD0E3AE4C561831A7A5"
    "1BE261BC5691306890C2CB70CD9913DE427DFC43BB8A1CA543F1B40C26570C7F"
    "3F7DF90DC0874334E1D0D2A943539B0D5BEEBB00C127105426AE7026B1BF680A";

const uint8_t *ZOO =(void *)
    "A56E4A0E701017589A5187DC7EA841D156F2EC0E36AD52A44DFEB1E61F7AD991"
    "D8C51056FFEDB162B4C0F283A12A88A394DFF526AB7291CBB307CEABFCE0B1DF"
    "D5CD9508096D5B2B8B6DF5D671EF6377C0921CB23C270A70E2598E6FF89D19F1"
    "05ACC2D3F0CB35F29280E1386B6F64C4EF22E1E1F20D0CE8CFFB2249BD9A2137"
    "7614839A495D8E96233454A0DF9FCEB8AC9A5BCA8E97430D11AC72330ACC708A"
    "A16B0B745A85CAA575685CF19D10C3815E6743408AFD8DD0E3AE4C561831A7A5"
    "1BE261BC5691306890C2CB70CD9913DE427DFC43BB8A1CA543F1B40C26570C7F"
    "3F7DF90DC0874334E1D0D2A943539B0D5BEEBB00C127105426AE7026B1BF680A"
    "A56E4A0E701017589A5187DC7EA841D156F2EC0E36AD52A44DFEB1E61F7AD991"
    "D8C51056FFEDB162B4C0F283A12A88A394DFF526AB7291CBB307CEABFCE0B1DF"
    "D5CD9508096D5B2B8B6DF5D671EF6377C0921CB23C270A70E2598E6FF89D19F1"
    "05ACC2D3F0CB35F29280E1386B6F64C4EF22E1E1F20D0CE8CFFB2249BD9A2137"
    "A56E4A0E701017589A5187DC7EA841D156F2EC0E36AD52A44DFEB1E61F7AD991"
    "D8C51056FFEDB162B4C0F283A12A88A394DFF526AB7291CBB307CEABFCE0B1DF"
    "D5CD9508096D5B2B8B6DF5D671EF6377C0921CB23C270A70E2598E6FF89D19F1"
    "05ACC2D3F0CB35F29280E1386B6F64C4EF22E1E1F20D0CE8CFFB2249BD9A2137"
    "7614839A495D8E96233454A0DF9FCEB8AC9A5BCA8E97430D11AC72330ACC708A"
    "A16B0B745A85CAA575685CF19D10C3815E6743408AFD8DD0E3AE4C561831A7A5"
    "1BE261BC5691306890C2CB70CD9913DE427DFC43BB8A1CA543F1B40C26570C7F"
    "3F7DF90DC0874334E1D0D2A943539B0D5BEEBB00C127105426AE7026B1BF680A"
    "A56E4A0E701017589A5187DC7EA841D156F2EC0E36AD52A44DFEB1E61F7AD991"
    "D8C51056FFEDB162B4C0F283A12A88A394DFF526AB7291CBB307CEABFCE0B1DF"
    "D5CD9508096D5B2B8B6DF5D671EF6377C0921CB23C270A70E2598E6FF89D19F1"
    "05ACC2D3F0CB35F29280E1386B6F64C4EF22E1E1F20D0CE8CFFB2249BD9A2137";

#define STATIC
#include "pe1_rsa_test_msg_PSS_11.h"
// from "message.c"
/* const uint8_t test_msg_PSS_11[] */
/* const uint8_t test_sig_PSS_11[] */
/* const uint8_t test_sig_PSS_11_plain[] */
/* test message for RSA VERIFY */

const uint8_t test_salt_PSS_11[] = {
0xde, 0xe9, 0x59, 0xc7, 0xe0, 0x64, 0x11, 0x36, 0x14, 0x20, 0xff, 0x80, 0x18, 0x5e, 0xd5, 0x7f,
0x3e, 0x67, 0x76, 0xaf};

// caution: to allow a comparison in ASCII, all letters should be in upper case
const uint8_t *test_sig_ENCODED_HASH_11 =(void *)
    "2009BCA4CB0EE7FDF35A762C488A3FEE0E4058130DDE835AE61EB830DEC8D50D"
    "3120EE732A1AE8A0C9E5E86A256B5C71D75BBC66735110CBDE3CA5BFA8120360"
    "7A0B8B8AB1F857452900D36002FA7A87003C79EE6069BB46CDEB8F281140F1D3"
    "E9F296E05BD56B26BCF707A6A14B02F4344FF91BD17F29B79B4606E70AFD99BC";

const uint8_t test_sig_ENCODED_HASH_11_plain[] = {
0x20, 0x09, 0xBC, 0xA4, 0xCB, 0x0E, 0xE7, 0xFD, 0xF3, 0x5A, 0x76, 0x2C, 0x48, 0x8A, 0x3F, 0xEE,
0x0E, 0x40, 0x58, 0x13, 0x0D, 0xDE, 0x83, 0x5A, 0xE6, 0x1E, 0xB8, 0x30, 0xDE, 0xC8, 0xD5, 0x0D,
0x31, 0x20, 0xEE, 0x73, 0x2A, 0x1A, 0xE8, 0xA0, 0xC9, 0xE5, 0xE8, 0x6A, 0x25, 0x6B, 0x5C, 0x71,
0xD7, 0x5B, 0xBC, 0x66, 0x73, 0x51, 0x10, 0xCB, 0xDE, 0x3C, 0xA5, 0xBF, 0xA8, 0x12, 0x03, 0x60,
0x7A, 0x0B, 0x8B, 0x8A, 0xB1, 0xF8, 0x57, 0x45, 0x29, 0x00, 0xD3, 0x60, 0x02, 0xFA, 0x7A, 0x87,
0x00, 0x3C, 0x79, 0xEE, 0x60, 0x69, 0xBB, 0x46, 0xCD, 0xEB, 0x8F, 0x28, 0x11, 0x40, 0xF1, 0xD3,
0xE9, 0xF2, 0x96, 0xE0, 0x5B, 0xD5, 0x6B, 0x26, 0xBC, 0xF7, 0x07, 0xA6, 0xA1, 0x4B, 0x02, 0xF4,
0x34, 0x4F, 0xF9, 0x1B, 0xD1, 0x7F, 0x29, 0xB7, 0x9B, 0x46, 0x06, 0xE7, 0x0A, 0xFD, 0x99, 0xBC};

const uint8_t *sig_key512_sha1_salt20=(void *)
"8b15f9a82fa5aa237ec84a3bda5af706"
"571eaa567e4bc836e891f2986cb07ae3"
"17dac46985ee0a170030b72421348d26"
"373dc19d4d48b6c184ce275d584c2144";

const uint8_t *sig_key1024_sha1_msg0=(void *)
"3274de9ace79b55e28d422563e88edc1"
"9085bd0d896a10a423442239c4a928f7"
"c6a89326d030e43f538c42ee9751e028"
"888a8c9b16fceeb63378ee168cb034cc"
"dd354fcb2c7f7780ad81ecf8716825b7"
"882cdee23f54be38c739afd18dcb5561"
"94d5865c2a1d8240ec62b45a01b428f7"
"7e327ed6f7164a37bf01cb73712311b6";

const uint8_t *sig_key1024_sha1_msg1=(void *)
"4f9f7dcda321c7b79471276ae1ad6414"
"c7b8b2a92a26650abd0465ab13f27ff7"
"2d4a8a72b28c9dedac84908a09ee4765"
"fbe7df03209f1f1e907b58d98676ce7c"
"c32d7af95030a0bc890ed95380a34b03"
"477fcff165554c143f1101e53f2c30c3"
"d082d255c29a9a44491d99aa874d4241"
"707eacef62150632f05a60fc2efca16e";

const uint8_t *sig_key1024_sha1_msg16=(void *)
"a4ff4dd1ea23405f5b3c046fd35430f0"
"435c522f04d464d47f310a2819561758"
"d6bb879237ac2459397e26210d538393"
"ee24b0d0869eacaea5f97c881ee7f128"
"6412e691ac098606c9028de69c22b0e2"
"33570fcfdc669057f14ed108373bc6ba"
"fb9e64c774b071f35224a6855de6a09a"
"a246d9c0279b086a74ec86badefa2e59";

const uint8_t *sig_key1024_sha1_salt0=(void *)
"25cbececadfb8d538c9cd4ef066b65cf"
"0418ff0b97a7995f0097452d3625a1fa"
"e74067e35b5822cd797ed6532f67564f"
"2b08c863bcf881be50632c873a54b414"
"41c340b2314c4e12bfc3ebf1229a4e36"
"502aee4aeb6e9f446419b3523b69671f"
"15fbb03f124e7fad9e64c037628b92d6"
"bf8872ed1d8beb28a590bc16a9bd2741";

const uint8_t *sig_key1024_sha1_salt1=(void *)
"0ad81e177435c69233799a77af62e2b0"
"d5e0da226e6c5ce74f0899ca8721cf15"
"88e3bca8b54c6b7e0b5d0d6014ffad47"
"e4bfcdefd9389a2024da95922145b4b0"
"e197b408b32e00e80f37d0887b47b9a7"
"6bd11e2061109f783ce05592b3965b53"
"d1e08bd58a959ec5c0d215e7f20aebd2"
"86e58ca68c440478c4011db5564f21ef";

const uint8_t *sig_key1024_sha1_salt106=(void *)
"775da66b7fd92155403bc3cd3ddbc56c"
"6b239a30af2442cf50843b4ec5b4fe1d"
"a3d44574b2f081c309558298ba995c93"
"6f01b800661ea778496a7c5674dfca47"
"cb9b64859d7d521d7dfc2475cc1f396e"
"fbe012fc95d7e55e445b9c5a6b691236"
"99695a6f1ffe9459628bf5e811bc0189"
"734f42b6afa03be9a3df44335b20cc8c";

const uint8_t *sig_key1024_128_sha1_salt20=(void *)
"3f5bd2b9d75e5cff4cb68c962a98243e"
"8155cd0f546e466e3bcc29f39d7d4a2b"
"ff84a08b776d6881308b87aca2ee4321"
"ab92681c77d3e1b3d7a53d11640952d3"
"29fb20e6e4b952697871f586b8fecc47"
"bf68a0a381b8ba15ef5ee45f1b29b4c9"
"449f06c18d5892b8ecd0040c01235b51"
"95389c6e7cb4feb10021992ed6043832";

const uint8_t *sig_key2048_sha1_salt20=(void *)
"64a8bdb962bedfdfd8cdc3fe81b8f6b1"
"a679055c9e1964ac7801385cf68e47a1"
"841d5ea5309bba59844120aa872519a9"
"42a87ec01b6adae9df799d15ebf2223b"
"891dcdb439bdb93eb2c0b707e861e4b0"
"3501fa3385d2a7cfb47ca54311ba0155"
"1a40b05908a9bc0fd799ddd0abfb8565"
"73ae06c836cf5f26f532fe14d9f01a7f"
"0a45a30ebf086d689475e38f2e303396"
"74b06351ba7b978103d11d5833c5e164"
"95e5886b33e6fcd6d380a08f10a50aaf"
"2a9a3e3971183412d7c4687b2fe97c82"
"7c5e7fb1a5fb5f3cc41895c615be359f"
"8b66e6cf8290a137b2b9f65a275aa619"
"045ce66396c9626be83977215398caed"
"b87d62529b7763757e6912570ae7637a";

const uint8_t *sig_key2048_sha256_salt20=(void *)
"b3eb804e08d2003c61402b67c1891972"
"cc796846b5f11d312cb3744672628766"
"f8a17e4ba164e07779555653593d48de"
"c72428401cc61b19d58b23810a30bb2d"
"767148f1590cf64cb4e0e77ef01e2e05"
"ec23531c38187077f106a6eacae0caac"
"237b70b0aa08b8537251fca7a9841cc1"
"574c0ac8606794bd551eab8a996c8833"
"7843e0a1e45154e230e614affb29114a"
"0215e555cf1cfe812413061da0c38054"
"5f8461eee95da083a2a9d254edc1f075"
"77262458b812cf28f45b1cc50f0cd9cc"
"14287bb3f9ed1c8b5f15d945024d39d3"
"b60617dd4735fb0c867ac3fbdba43c65"
"31bcec1c36ad9ec94843013ca074be29"
"a65c6612a543f81771a8744354506270";

const uint8_t *sig_key2048_sha512_salt20=(void *)
"7fd9578c0edc5734f9b4acfb77028f28"
"be9a940ff0977678b8ffe1b2216516de"
"eadc42102c16e2e2bae46c2ac36fb907"
"380e136995b2d6fa2cb69c87fcc21bbe"
"66e087c114ce6e9bebe57a7c82ad88a0"
"28b7457f493d2d563436ce7e215ebcf8"
"c8c7e5e4d0ae94067497f83591098b52"
"374a3de7ce54b795571889be1555a650"
"b64f02855b084ce56ddd3c2a1c72e9bf"
"0dab8df4bcc2d29f07e7343631c9514f"
"0a771f29bc67da10e7e9939289d5e0e2"
"014885ad45f7ce2c7f4d29add8b39834"
"22e147c08dd5faac0329c26022e26d4b"
"6605badbf3ed0b6e844a57d6345ea124"
"869b6a523b9215fa722944b0cacc691b"
"e60f5d7b8f4bab0b18dbe5a1c7b2fdb8";

const uint8_t *sig_key3072_sha1_salt20=(void *)
"583b85924d788c8fafffff8194c7e703"
"4314a803afa8dd97f8f97d258fec5294"
"673112bf04b688e49f06cd03a60e8c5a"
"0b01bad199c086f41b43cbfa5fe76419"
"59ab343cd55b06d4531fceb225220608"
"c26e3c5e31ee30129d1b1d00b709249a"
"474fb61392da479b65d227b3f43f33cd"
"908ed6ae080fd608067c57590b8f554b"
"4fe450957477ad5bc78804e432609f16"
"9c53c33529991c3ef6089e557dc1eee6"
"042358100b802c883fdb42d635857137"
"9769e33e2d959029f0b59390bd6a480d"
"7967fcd7dfcdba4fe93236c8b36e2e3f"
"31a60147106527fea36739f4e98ef113"
"b30bcaa89c80de017fdc8b707ce7726c"
"a8d6ca43fae5c0c9541401f6a94fe90e"
"5f0991323cacd36786d6a58f55bc8a1d"
"c6cd97ce74f045d670551ab93ac9098f"
"16d80e41aadc065670b120edb3e29bee"
"e344dec8d385070c6decd059d3577fae"
"8fcb09a8c57ada50a029174cbd54be96"
"b4d1cc0fd6db5f5fa681f672c8f6b73c"
"47b6d7d2664cc1c5056f352ce59a92fa"
"39ad3e0d544951d07a892d602e8dd7b9";

// ********************************************************************************
// Testing RSA Key Pair Import
// ********************************************************************************
const uint8_t pub_exp[] = 
{
    0x01, 0x00, 0x01, 
};

const uint8_t priv_exp[] = 
{
    0x4B, 0x3F, 0x68, 0x3B, 0xE0, 0x33, 0xBE, 0xB5, 0xA2, 0x1C, 0xD0, 0xAE, 0x6C, 0x9E, 0xAA, 0xFE, 
    0xCE, 0xF4, 0x53, 0x1A, 0x85, 0x8B, 0x14, 0x39, 0xD9, 0xDD, 0xC1, 0x32, 0x12, 0xF6, 0xDB, 0x8A, 
    0x6E, 0x2E, 0x18, 0x45, 0x49, 0xBC, 0x5D, 0xB8, 0x05, 0x9E, 0x9E, 0x2D, 0x1F, 0x35, 0xF8, 0x8D, 
    0xB7, 0x25, 0xFB, 0x8E, 0x50, 0xA4, 0x74, 0x2A, 0xE6, 0x1B, 0x76, 0xD5, 0xE5, 0x2F, 0xE6, 0x0C, 
    0x02, 0xFD, 0x69, 0xE5, 0x79, 0x6A, 0x7F, 0x81, 0xD5, 0xB3, 0x54, 0xFE, 0x94, 0xCE, 0x0A, 0x9B, 
    0x05, 0x6F, 0xB4, 0x0A, 0xA3, 0x42, 0xA8, 0x22, 0x78, 0xC1, 0x7C, 0x99, 0x70, 0xEA, 0xAA, 0x34, 
    0xC3, 0x3D, 0xD3, 0x29, 0xE3, 0x6E, 0xED, 0x12, 0x40, 0x61, 0x52, 0x04, 0x32, 0x1B, 0x02, 0xE7, 
    0x63, 0xFF, 0xD5, 0x61, 0xD1, 0x03, 0x83, 0x04, 0xFC, 0xFA, 0xFA, 0x04, 0x03, 0x61, 0xB3, 0xD9, 
    0xD7, 0x34, 0x19, 0xC9, 0x91, 0x88, 0xB1, 0xB9, 0xB0, 0x95, 0xD1, 0x17, 0x07, 0x1F, 0x0D, 0x83, 
    0x9C, 0x05, 0x87, 0x5D, 0xC7, 0x25, 0xE8, 0x1F, 0x44, 0x38, 0x46, 0xF2, 0x84, 0x3C, 0xBF, 0xF8, 
    0x1F, 0xC9, 0xE8, 0xA5, 0x1F, 0xC9, 0x62, 0xA1, 0x75, 0xC3, 0x6D, 0x48, 0xA2, 0xEA, 0x89, 0x18, 
    0x51, 0xAC, 0x50, 0x4F, 0xC0, 0xE5, 0x37, 0xD7, 0xA1, 0x46, 0x9E, 0x8F, 0xF5, 0x0C, 0x0C, 0x63, 
    0x1F, 0xFD, 0x54, 0x64, 0xC1, 0x86, 0x25, 0x95, 0xBB, 0xB1, 0xE0, 0xDF, 0x0F, 0xD0, 0xA3, 0x18, 
    0x9C, 0xE8, 0xDE, 0x35, 0xB9, 0xCD, 0x0A, 0xEC, 0x6D, 0x8C, 0x23, 0x65, 0x6B, 0x90, 0x36, 0xD2, 
    0xD5, 0x8D, 0x7F, 0x8F, 0xD0, 0xE5, 0xA4, 0x6A, 0x56, 0x3C, 0xF6, 0x01, 0x56, 0x70, 0x30, 0x29, 
    0xFA, 0x3C, 0x69, 0x29, 0xDD, 0x3E, 0xEB, 0x58, 0xCE, 0xA2, 0x01, 0x59, 0x3B, 0x86, 0x4F, 0x9D, 
};

const uint8_t mod[] = 
{
    0xAE, 0x6F, 0xDD, 0x0A, 0x56, 0x00, 0xC4, 0xD6, 0x2E, 0x70, 0x6F, 0xCE, 0x91, 0xFF, 0x81, 0xED, 
    0xB5, 0x40, 0x1A, 0x36, 0xA5, 0x20, 0x96, 0x6F, 0x04, 0x8D, 0x1B, 0xA4, 0xDE, 0xA8, 0x77, 0xC2, 
    0x01, 0x66, 0x35, 0xD6, 0x33, 0x80, 0xC0, 0x4C, 0xD1, 0x86, 0x7E, 0xC9, 0xEC, 0xFE, 0x73, 0x0D, 
    0x7F, 0xA6, 0x0D, 0x47, 0x13, 0x80, 0xBA, 0x2B, 0x5B, 0xDE, 0x52, 0xB8, 0xFF, 0x8D, 0xEE, 0xFE, 
    0x55, 0x8C, 0x68, 0xBD, 0x9C, 0x28, 0x2D, 0x63, 0x43, 0xE1, 0x20, 0xC5, 0x21, 0x56, 0x7F, 0xC7, 
    0x59, 0xD7, 0xE6, 0x16, 0x49, 0xA1, 0x70, 0xA1, 0x43, 0x6A, 0x6B, 0x8C, 0x53, 0x04, 0xE8, 0xB6, 
    0xBF, 0x52, 0xEC, 0x1A, 0x64, 0x6E, 0x01, 0xE0, 0x43, 0xFB, 0x91, 0x08, 0x73, 0x1A, 0x4B, 0xE2, 
    0xE7, 0x9B, 0x76, 0xA1, 0x13, 0xE8, 0x05, 0xA7, 0xAC, 0xE8, 0xB5, 0xBF, 0x44, 0x92, 0x94, 0x33, 
    0x31, 0xA0, 0x58, 0x2E, 0xB8, 0x38, 0xCA, 0x1E, 0x62, 0x71, 0x8C, 0x42, 0xC6, 0xFD, 0x1E, 0xDD, 
    0x70, 0x34, 0xBD, 0x06, 0x42, 0x5E, 0xCD, 0x41, 0xA4, 0x08, 0x75, 0x62, 0xDB, 0x95, 0xC8, 0x76, 
    0xDA, 0x91, 0xF0, 0x02, 0xEF, 0xB3, 0xBB, 0xC3, 0x3D, 0x62, 0xEE, 0xA1, 0xB6, 0x4B, 0xAC, 0x80, 
    0x9D, 0x32, 0xAB, 0x9F, 0xCF, 0xA5, 0x31, 0x8D, 0xC5, 0xBB, 0x6B, 0xD0, 0x11, 0x4B, 0x26, 0x8E, 
    0x26, 0xFC, 0x6D, 0x18, 0x7C, 0xDB, 0x08, 0xAF, 0x52, 0xE3, 0xF2, 0xB2, 0x9B, 0xC1, 0x35, 0x9C, 
    0x14, 0x54, 0xE6, 0x76, 0x60, 0x8D, 0x70, 0x19, 0xC6, 0xEA, 0xB6, 0xD3, 0xE7, 0xEE, 0xE9, 0x6F, 
    0x39, 0x94, 0x71, 0x84, 0x19, 0x94, 0x53, 0xD7, 0x22, 0xE6, 0x91, 0x44, 0xEF, 0x0B, 0xD1, 0x35, 
    0xD1, 0x9C, 0x7B, 0xE4, 0x0B, 0x14, 0x0C, 0x3C, 0x0E, 0xB9, 0xC5, 0x0A, 0x95, 0x8E, 0x8D, 0xB1, 
};

void icum_rsa_sign_pss(void)
{
    r_icumif_isd_t *p_ISD;
    uint16_t sig_size;
    uint16_t salt_size;
    int n;

    // pause life signal management
    pause_life_signal();

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting RSA(PSS) signature generation...\n\n");

    printf("  key index?> ");
    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &n);

    p_ISD->service_id     = SERVICE_01_RSA_SIGN;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_SIGN.key_id                  = n;
    p_ISD->prm.RSA_SIGN.p_source                = (void *)test_msg_PSS_11;
    p_ISD->prm.RSA_SIGN.source_size_in_bytes    = sizeof(test_msg_PSS_11);
    p_ISD->prm.RSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.RSA_SIGN.hash_encoding_format    = ENCODING_FORMAT_EMSA_PSS;
    p_ISD->prm.RSA_SIGN.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_SIGN.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_SIGN.p_signature             = BIG_BUFFER[0];
    p_ISD->prm.RSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;

    printf("Job slice sha: %d block(s)\n", p_ISD->prm.RSA_SIGN.job_slice_hash);
    printf("Job slice rsa: %d block(s)\n", p_ISD->prm.RSA_SIGN.job_slice_mod_exp);

    printf("\nStart signature generation...\n\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    if(p_ISD->service_result != SERV_OK)
    {
        printf("Fail sinnature generation\n");
        goto end;
    }
    
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    
    sig_size  = p_ISD->prm.RSA_SIGN.signature_size_in_bits;
    salt_size = p_ISD->prm.RSA_SIGN.salt_size_in_bytes;
    
    printf("Signature size: %d bits\n", sig_size);
    printf("Salt size:      %d bytes\n", salt_size);
    printf("Signature size: %d bits\n", p_ISD->prm.RSA_SIGN.signature_size_in_bits);
    printf("Signature:      "); PMEM((const unsigned char *)p_ISD->prm.RSA_SIGN.p_signature, p_ISD->prm.RSA_SIGN.signature_size_in_bits / 8); NL(); FLUSH();
    printf("Processed in :  "); PCHRONO(); NL(); FLUSH();

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA;
    p_ISD->prm.RSA_VERIFY.key_id                  = n;
    p_ISD->prm.RSA_VERIFY.p_source                = (void *)test_msg_PSS_11;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes    = sizeof(test_msg_PSS_11);
    p_ISD->prm.RSA_VERIFY.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = ENCODING_FORMAT_EMSA_PSS;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.p_signature             = BIG_BUFFER[0];
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_BINARY_PLAIN;
    p_ISD->prm.RSA_VERIFY.verification_result     = VERIFICATION_FAIL;

    p_ISD->prm.RSA_VERIFY.signature_size_in_bits  = sig_size;
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = salt_size;

    printf("\nStart signature verification...\n\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
    printf("Verification result : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n\n");
    }
    else    
    {
        printf("*FAIL*!\a\n\n");
    }

end:
    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}

// ********************************************************************************
// Testing RSA 2048 bit key
// ********************************************************************************

const uint8_t t2048_msg[]={
    0x88, 0x31, 0x77, 0xe5, 0x12, 0x6b, 0x9b, 0xe2, 0xd9, 0xa9, 0x68, 0x03, 0x27, 0xd5, 0x37, 0x0c, 
    0x6f, 0x26, 0x86, 0x1f, 0x58, 0x20, 0xc4, 0x3d, 0xa6, 0x7a, 0x3a, 0xd6, 0x09};

const uint8_t *t2048_key=(void *)
    "a5dd867ac4cb02f90b9457d48c14a770"
    "ef991c56c39c0ec65fd11afa8937cea5"
    "7b9be7ac73b45c0017615b82d622e318"
    "753b6027c0fd157be12f8090fee2a7ad"
    "cd0eef759f88ba4997c7a42d58c9aa12"
    "cb99ae001fe521c13bb5431445a8d5ae"
    "4f5e4c7e948ac227d3604071f20e577e"
    "905fbeb15dfaf06d1de5ae6253d63a6a"
    "2120b31a5da5dabc9550600e20f27d37"
    "39e2627925fea3cc509f21dff04e6eea"
    "4549c540d6809ff9307eede91fff5873"
    "3d8385a237d6d3705a33e39190099207"
    "0df7adf1357cf7e3700ce3667de83f17"
    "b8df1778db381dce09cb4ad058a51100"
    "1a738198ee27cf55a13b754539906582"
    "ec8b174bd58d5d1f3d767c613721ae05";

// ********************************************************************************
// Testing RSA 3072 bit key
// ********************************************************************************
const uint8_t t3072_msg[]={
    0x9a, 0xf2, 0xfe, 0x8d, 0xfa, 0xd3, 0x98, 0x4c,
    0x5c, 0x19, 0xc7, 0xfb, 0xd8, 0x46, 0x95, 0xcd,
    0xe7, 0xc8, 0x4b, 0x3d, 0x2d, 0x7c, 0xe1, 0xaa,
    0x95, 0x70, 0xb8, 0x44, 0xa3, 0xed, 0x8c, 0x2c,
    0x06, 0xb5, 0xa4, 0xd7, 0x6d, 0x89, 0x4b, 0xb4,
    0x9c, 0xee, 0x84, 0x0e, 0xbf, 0x31, 0xfa, 0x33,
    0xfe, 0xa9, 0x36, 0xd6, 0x51, 0x3c, 0x14, 0xb2,
    0x96, 0x24, 0x78, 0x70, 0xe1, 0xf3, 0x3d, 0xcb,
    0x13, 0xd5, 0x6e, 0xf2, 0xae, 0x16, 0xb9, 0x2d,
    0xe7, 0xe4, 0x20, 0xc4, 0xfd, 0x55, 0xf2, 0x1d,
    0xca, 0xac, 0xfa, 0x24, 0x98, 0xd8, 0xf6, 0xef,
    0x35, 0x3d, 0x4b, 0xa3, 0x4a, 0xb7, 0xf6, 0x91,
    0xe8, 0xdc, 0xc6, 0xa5, 0x64, 0x82, 0xc8, 0x01,
    0x65, 0x82, 0x03, 0xe5, 0x3b, 0xbb, 0x66, 0xd2,
    0x72, 0x59, 0xff, 0xef, 0xa7, 0x57, 0x6e, 0xab,
    0x9b, 0x42, 0x98, 0xa4, 0x86, 0x40, 0xa0, 0xc8,
};

const uint8_t SMALL_E[] = "11";

const uint8_t *t3072_sig=(void *)
#if !defined(CAVP_TEST)
    "583b85924d788c8fafffff8194c7e703"
    "4314a803afa8dd97f8f97d258fec5294"
    "673112bf04b688e49f06cd03a60e8c5a"
    "0b01bad199c086f41b43cbfa5fe76419"
    "59ab343cd55b06d4531fceb225220608"
    "c26e3c5e31ee30129d1b1d00b709249a"
    "474fb61392da479b65d227b3f43f33cd"
    "908ed6ae080fd608067c57590b8f554b"
    "4fe450957477ad5bc78804e432609f16"
    "9c53c33529991c3ef6089e557dc1eee6"
    "042358100b802c883fdb42d635857137"
    "9769e33e2d959029f0b59390bd6a480d"
    "7967fcd7dfcdba4fe93236c8b36e2e3f"
    "31a60147106527fea36739f4e98ef113"
    "b30bcaa89c80de017fdc8b707ce7726c"
    "a8d6ca43fae5c0c9541401f6a94fe90e"
    "5f0991323cacd36786d6a58f55bc8a1d"
    "c6cd97ce74f045d670551ab93ac9098f"
    "16d80e41aadc065670b120edb3e29bee"
    "e344dec8d385070c6decd059d3577fae"
    "8fcb09a8c57ada50a029174cbd54be96"
    "b4d1cc0fd6db5f5fa681f672c8f6b73c"
    "47b6d7d2664cc1c5056f352ce59a92fa"
    "39ad3e0d544951d07a892d602e8dd7b9";
#else
    "86e072838ecdf97805761f425cf1ac7b"
    "e3d172ffd5166afb319f0fc6b38abc08"
    "d92192c06fed5a44927dff52eaa5f3fd"
    "935c69cc87b3e93e20cea0103e76c81e"
    "344c46e2b63d86088d142d69bbe70ea8"
    "9af0ada5812553b0b1cc2883787d1670"
    "33b079b14453e5ae4bbd3455cc6ce540"
    "c8a7df03173b7e62536cb58be98bd298"
    "23ae02568372c126dff3be1412ec86bd"
    "733348baf33b5a2a02775c4168343007"
    "b0a67748cd753f77f242ea1eb48e699b"
    "36d2bbfe7e3c868aa4e0570d0d6b6b84"
    "d98afcdebae98258662907f173bc2a4f"
    "72f177e7eb651c9f529a71e515770ed6"
    "703daf75010de481e36a0ebc4d03fa7d"
    "ced68c54a8327ffa3c29e0bfe8df80ba"
    "306e25dc673534be71317e5dd9feead2"
    "8b0714b9779c6befa23395a4a59d10bc"
    "237396b6eb71219a380d3e25213b31dd"
    "b3a91bf35c595e3e3dc3e9787a96353e"
    "70bf6f53ec4d0a55b88aff4456ace9f2"
    "b6b31d31cf7367b350d00464689e56b5"
    "ef2dea358db8c17091132fa3b91cceca"
    "f1d0a2e024d7825540d1e5a841ecacc0";
#endif

const uint8_t *t3072_key=(void *)
#if !defined(CAVP_TEST)
    "ba168fad96a9f2d6e32e01f44dff"
    "2e245cfe372aa751dc77ffa437d4e8"
    "ef68b63f985d3286ebd6bde8b01fa0"
    "e928ca316f989255bec6586fc81a8c"
    "f76210620d865796d9ff92bba56b25"
    "9d0918b88601fe86a122464da16709"
    "875dda7fe4c1f0c111af60564b4539"
    "2240ac24afa778e3e7fffeb945f07c"
    "5668c017ec073e7ce84e9b6c7efa18"
    "9048c422ed310f6de948d474ad6f43"
    "a92e70bcd6e310e8c576cae9f6dd3f"
    "97b37816580ea6977014e1c1f9b705"
    "eb3278666b5d71900c26f1e26dd0f9"
    "dffd4100d84b0d3ed00809edefdc40"
    "f8f11e5acba248656440200e80bed5"
    "ea244956a764f9760723743641e14b"
    "b3d93edc69e0e21d492ce3ac59ed2e"
    "cdda72c940c58168ee22cbcb399c73"
    "08a14ec6d9d15d7ceb14b031bca1c6"
    "0a34560133581233716a8755a0c9ac"
    "e8a24fa41c1da41ad381e4ccda88a8"
    "6d0af9a991049761d9d890168e75a8"
    "294c6a1a923adab5ab5ed087ade7e9"
    "d3ccd6bfa75dc1a7468eefbab60b02"
    "c141e059825942034fa16679d04376"
    "6904422272cae145b47f";
#else
    "a5f3da0aaf54b45f99a5d7085f213c37"
    "21cbe7e83b3e6c3fe0f5a84c7e387ba5"
    "13392c28a9010d3b618c03847e6b11bb"
    "bbe4d5e47fc97ea696250699e96ecd91"
    "1404f7b806957038a68bb59a520f2d90"
    "182d183e035204a914e6ac03c2bc6d3f"
    "9d7856b25f9041b56df310de3feb30aa"
    "468a0668a1e5da9cdb185956caa5d75e"
    "1cdcac2db823173495619105367231b7"
    "f2de7528a8a79ec9fdbbab601178a204"
    "a5aa4e19759eb16ea4bab87bf48bb179"
    "0f9fc6eb4d5674d3fbc11b922558d4e5"
    "68e454b26a7178f3e147beb0c8ca6ecf"
    "f5e52af248ac07d6a189393e17232adf"
    "f2f7423f56b94b9a7d61fde23a9558ac"
    "7a3bc7c06748a5da11759f92baf4e386"
    "bb0212565b5beecf31d063cfab71af89"
    "6b3d734750d9bca07343bfb3c2864522"
    "6e9dad3070fc247c71c078e974934941"
    "000a79d01abab14d21f5e608c4e4d13d"
    "eec1aef298e1247c50b47bfee6162f35"
    "2f41cdba8628d1d628848c876cfb102d"
    "acce7fa160c04d3aabc8667a142a710b"
    "7f495fd350c4862a653d15c33d9266fd";
#endif

// ********************************************************************************
// Testing RSA 2048 bit key with SHA2-256
// ********************************************************************************
const uint8_t t2048_msg_2[]={
    0x35, 0xa3, 0x79, 0x46, 0xe5, 0x26, 0x78, 0xee, 0x37, 0x8f, 0x5f, 0x17, 0x68, 0x38, 0xef, 0x08,
    0xf3, 0xc2, 0x13, 0x92, 0xb1, 0xad, 0x20, 0x46, 0x45, 0x25, 0x5b, 0xe5, 0xb7, 0x1f, 0xbc, 0x18,
    0x5f, 0xa5, 0xf1, 0x61, 0x05, 0x6e, 0xa6, 0x52, 0x46, 0xb2, 0x04, 0xfd, 0x39, 0x3c, 0x77, 0xab,
    0x53, 0xc1, 0xb5, 0xd1, 0x88, 0x70, 0xfc, 0x3f, 0xb3, 0xca, 0x9a, 0x9b, 0x38, 0xb4, 0xb3, 0x0e,
    0xe8, 0xcb, 0x3f, 0x3d, 0x25, 0xf7, 0x52, 0x7b, 0x46, 0x43, 0xa0, 0x3c, 0x3d, 0xec, 0x40, 0xcd,
    0x76, 0xb7, 0xb0, 0x43, 0x03, 0x88, 0x1a, 0xb2, 0xf7, 0x31, 0xd5, 0x9f, 0x0f, 0x88, 0x2f, 0xb7,
    0x98, 0xbc, 0x6a, 0xc1, 0x8c, 0xe9, 0x04, 0xd1, 0xff, 0xe9, 0x3c, 0xbe, 0xb9, 0x6e, 0xd1, 0xd7,
    0x25, 0x4d, 0x0d, 0xd2, 0x6a, 0x1d, 0x02, 0x05, 0xd7, 0x01, 0x14, 0xd9, 0x84, 0xc2, 0xb7, 0x7b,
};

const uint8_t *t2048_sig_2=(void *)
    "56279ccd2d37e8113625732cd3f3b61b"
    "4ef9325160c7f6af7077c25049d32742"
    "607ae3f845bd66cd6752813c26067fdc"
    "23f08008cf6a531124e9ebc9264f7cfd"
    "6d6eeff15daf97dad22565ec36b69125"
    "e7b27fc93892f6ff42ce8f265dc2cf2e"
    "5758ba0d67968e800e73fd47131008f5"
    "adc863919ea0cc153cf7efff134b0bcb"
    "d0af5505ab49af7b75d63a8aa7976b8f"
    "e77baf5a699a7e38a60eb7ca64e834f3"
    "e0f89da5b6a343a01f7657fd842c091a"
    "6208503bc75de8f95de0d871a6fc114b"
    "594ff99d615825fd3b89693338145253"
    "6d68d9e034f65abf3412e8e32002689e"
    "102a8bb69991e04a7ff681b62e48ee68"
    "7badf8690b2ccee4bf245cd0a25dcf21";

const uint8_t *t2048_key_2=(void *)
    "c6e0ed537a2d85cf1c4effad6419884d"
    "824ceabf5200e755691cb7328acd6a75"
    "5fe85798502ccaec9e55d47afd0cf325"
    "8ebe920b50c5fd9d72897462bd0e459b"
    "bdf902b63d17195b2ef54908980be12a"
    "a7489f8af274b92c0cbc16aed2fa46f7"
    "82d5517b666edfb2e5e5efeaff7e2496"
    "5e26472e51980b0cfe457d297e6aa5da"
    "cb8e728dc6f58130f925a13275c3cace"
    "62f820db1e13cc5274c58ff4d7837671"
    "a1bf5f80d6ad8699c568df8d24dd0f15"
    "2ded36ef4861f59b354bba96a076913a"
    "25facf4722737e6deed95b69a00fb2bc"
    "ed0feeedea4ff01a92605cfe26a6b395"
    "53d0c74e5650eb3779705e135c4b2fa5"
    "18a8d4339c53efab4bb0058238def555";

static const uint8_t test_msg_emsa_pkcsv15[] =
{
    0x85, 0x13, 0x84, 0xcd, 0xfe, 0x81, 0x9c, 0x22, 0xed, 0x6c, 0x4c, 0xcb, 0x30, 0xda, 0xeb, 0x5c,
    0xf0, 0x59, 0xbc, 0x8e, 0x11, 0x66, 0xb7, 0xe3, 0x53, 0x0c, 0x4c, 0x23, 0x3e, 0x2b, 0x5f, 0x8f,
    0x71, 0xa1, 0xcc, 0xa5, 0x82, 0xd4, 0x3e, 0xcc, 0x72, 0xb1, 0xbc, 0xa1, 0x6d, 0xfc, 0x70, 0x13,
    0x22, 0x6b, 0x9e
};

void icum_rsa_sign_pkcs1_v15(void)
{
    r_icumif_isd_t *p_ISD;
    uint16_t sig_size;
    int n;

    // pause life signal management
    pause_life_signal();

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting RSA(PKCS#1 v1.5) signature generation...\n\n");

    printf("  key index?> ");
    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &n);

    p_ISD->service_id     = SERVICE_01_RSA_SIGN;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_SIGN.key_id                  = n;
    p_ISD->prm.RSA_SIGN.p_source                = (void *)test_msg_emsa_pkcsv15;
    p_ISD->prm.RSA_SIGN.source_size_in_bytes    = sizeof(test_msg_emsa_pkcsv15);
    p_ISD->prm.RSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.RSA_SIGN.hash_encoding_format    = ENCODING_FORMAT_EMSA_PKCS1_V15;
    p_ISD->prm.RSA_SIGN.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_SIGN.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_SIGN.p_signature             = BIG_BUFFER[0];
    p_ISD->prm.RSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;

    printf("Job slice sha: %d block(s)\n", p_ISD->prm.RSA_SIGN.job_slice_hash);
    printf("Job slice rsa: %d block(s)\n", p_ISD->prm.RSA_SIGN.job_slice_mod_exp);

    printf("\nStart signature generation...\n\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    if(p_ISD->service_result != SERV_OK)
    {
        printf("Fail sinnature generation\n");
        goto end;
    }
    
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    
    sig_size  = p_ISD->prm.RSA_SIGN.signature_size_in_bits;
    
    printf("Signature size: %d bits\n", sig_size);
    printf("Signature size: %d bits\n", p_ISD->prm.RSA_SIGN.signature_size_in_bits);
    printf("Signature:      "); PMEM((const unsigned char *)p_ISD->prm.RSA_SIGN.p_signature, p_ISD->prm.RSA_SIGN.signature_size_in_bits / 8); NL(); FLUSH();
    printf("Processed in :  "); PCHRONO(); NL(); FLUSH();

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA;
    p_ISD->prm.RSA_VERIFY.key_id                  = n;
    p_ISD->prm.RSA_VERIFY.p_source                = (void *)test_msg_emsa_pkcsv15;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes    = sizeof(test_msg_emsa_pkcsv15);
    p_ISD->prm.RSA_VERIFY.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = ENCODING_FORMAT_EMSA_PKCS1_V15;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.p_signature             = BIG_BUFFER[0];
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_BINARY_PLAIN;
    p_ISD->prm.RSA_VERIFY.verification_result     = VERIFICATION_FAIL;

    p_ISD->prm.RSA_VERIFY.signature_size_in_bits  = sig_size;

    printf("\nStart signature verification...\n\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
    printf("Verification result : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n\n");
    }
    else    
    {
        printf("*FAIL*!\a\n\n");
    }

end:
    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}



// ********************************************************************************
// Testing RSA Encrypt OAEP
// ********************************************************************************
static const uint8_t pub_exp_oaep[] = 
{
    0x01, 0x00, 0x01, 
};
static const uint8_t modulus_oeap[] =
{
    0xa8, 0xb3, 0xb2, 0x84, 0xaf, 0x8e, 0xb5, 0x0b, 0x38, 0x70, 0x34, 0xa8, 0x60, 0xf1, 0x46, 0xc4,
    0x91, 0x9f, 0x31, 0x87, 0x63, 0xcd, 0x6c, 0x55, 0x98, 0xc8, 0xae, 0x48, 0x11, 0xa1, 0xe0, 0xab,
    0xc4, 0xc7, 0xe0, 0xb0, 0x82, 0xd6, 0x93, 0xa5, 0xe7, 0xfc, 0xed, 0x67, 0x5c, 0xf4, 0x66, 0x85,
    0x12, 0x77, 0x2c, 0x0c, 0xbc, 0x64, 0xa7, 0x42, 0xc6, 0xc6, 0x30, 0xf5, 0x33, 0xc8, 0xcc, 0x72,
    0xf6, 0x2a, 0xe8, 0x33, 0xc4, 0x0b, 0xf2, 0x58, 0x42, 0xe9, 0x84, 0xbb, 0x78, 0xbd, 0xbf, 0x97,
    0xc0, 0x10, 0x7d, 0x55, 0xbd, 0xb6, 0x62, 0xf5, 0xc4, 0xe0, 0xfa, 0xb9, 0x84, 0x5c, 0xb5, 0x14,
    0x8e, 0xf7, 0x39, 0x2d, 0xd3, 0xaa, 0xff, 0x93, 0xae, 0x1e, 0x6b, 0x66, 0x7b, 0xb3, 0xd4, 0x24,
    0x76, 0x16, 0xd4, 0xf5, 0xba, 0x10, 0xd4, 0xcf, 0xd2, 0x26, 0xde, 0x88, 0xd3, 0x9f, 0x16, 0xfb
};
static const uint8_t modulus_oeap_2048[] =
{
    0xc4,0xb1,0x17,0x8c,0xab,0x0d,0xd2,0x08,0x4e,0x27,0xb8,0x86,0x84,0x6e,0xd7,0xb4,
    0x1b,0x3c,0xa8,0x77,0xc3,0x06,0xc1,0x11,0x68,0x0b,0xc9,0x2a,0x09,0xf1,0x66,0x90,
    0x1e,0xad,0xd4,0x30,0x9a,0x4f,0x15,0xf1,0x9d,0xa8,0xac,0xbb,0x84,0xeb,0xb7,0x6e,
    0xe5,0x6e,0xeb,0xa1,0x86,0xc3,0x41,0x90,0x65,0x70,0x42,0xc1,0x75,0xac,0xc8,0x21,
    0x45,0x4c,0x44,0x9e,0xa6,0xb5,0xcd,0x7f,0xea,0x1c,0xfc,0xce,0xd6,0xa0,0x13,0x7d,
    0x89,0x42,0xc0,0xbe,0x8a,0x04,0xc7,0x9d,0xe2,0x89,0xf0,0xd5,0x40,0x5c,0x09,0xee,
    0x03,0x21,0x2f,0xf1,0xbe,0x61,0x25,0x55,0xd7,0xe3,0x00,0x89,0x8f,0x30,0x92,0x52,
    0xa4,0x04,0x5b,0x3f,0x6c,0x24,0x7e,0x3e,0x13,0x23,0xd9,0x68,0xd3,0x80,0xb4,0x4b,
    0x67,0x28,0x07,0x18,0xf2,0xaa,0xce,0xaf,0x51,0xce,0xbc,0x96,0x16,0xce,0x48,0xc3,
    0x51,0x17,0xe2,0x30,0xa4,0xe5,0xb9,0x9a,0xa5,0x72,0xa6,0xcb,0xb8,0xce,0xc5,0xb8,
    0x4d,0x1d,0x80,0x2e,0x47,0xa1,0x7d,0x1a,0xdd,0x4a,0xe0,0x0f,0x87,0xb1,0xc7,0xd1,
    0xd3,0x85,0x3c,0x19,0xa3,0x57,0xe7,0xf1,0xe8,0x36,0xe6,0x66,0x7d,0x6b,0x4e,0x79,
    0x91,0xcd,0x4e,0x5a,0xf5,0x90,0xcc,0xab,0x00,0x79,0x76,0xdb,0xef,0x53,0xc6,0x09,
    0x8a,0xbd,0x96,0x64,0xc8,0x05,0x28,0xa9,0xb7,0x97,0x13,0xa5,0x92,0x54,0x1a,0xfb,
    0x2c,0x3f,0x4d,0xd9,0x03,0xbf,0x96,0xd8,0xd3,0x2a,0x79,0x5e,0x7e,0x7f,0x1d,0x41,
    0x05,0x8e,0x93,0x23,0x6b,0x9e,0x67,0x24,0x62,0x93,0xfd,0x61,0xab,0x5c,0x23,0x95
};
static const uint8_t priv_exp_oaep[] =
{ 
    0x53, 0x33, 0x9c, 0xfd, 0xb7, 0x9f, 0xc8, 0x46, 0x6a, 0x65, 0x5c, 0x73, 0x16, 0xac, 0xa8, 0x5c,
    0x55, 0xfd, 0x8f, 0x6d, 0xd8, 0x98, 0xfd, 0xaf, 0x11, 0x95, 0x17, 0xef, 0x4f, 0x52, 0xe8, 0xfd,
    0x8e, 0x25, 0x8d, 0xf9, 0x3f, 0xee, 0x18, 0x0f, 0xa0, 0xe4, 0xab, 0x29, 0x69, 0x3c, 0xd8, 0x3b,
    0x15, 0x2a, 0x55, 0x3d, 0x4a, 0xc4, 0xd1, 0x81, 0x2b, 0x8b, 0x9f, 0xa5, 0xaf, 0x0e, 0x7f, 0x55,
    0xfe, 0x73, 0x04, 0xdf, 0x41, 0x57, 0x09, 0x26, 0xf3, 0x31, 0x1f, 0x15, 0xc4, 0xd6, 0x5a, 0x73,
    0x2c, 0x48, 0x31, 0x16, 0xee, 0x3d, 0x3d, 0x2d, 0x0a, 0xf3, 0x54, 0x9a, 0xd9, 0xbf, 0x7c, 0xbf,
    0xb7, 0x8a, 0xd8, 0x84, 0xf8, 0x4d, 0x5b, 0xeb, 0x04, 0x72, 0x4d, 0xc7, 0x36, 0x9b, 0x31, 0xde,
    0xf3, 0x7d, 0x0c, 0xf5, 0x39, 0xe9, 0xcf, 0xcd, 0xd3, 0xde, 0x65, 0x37, 0x29, 0xea, 0xd5, 0xd1
};
static const uint8_t priv_exp_oaep_2048[] =
{ 
    0x58,0x53,0x86,0x55,0xbf,0x02,0xc7,0xcc,0xbe,0xfc,0x0b,0x5b,0x2c,0xe1,0x71,0xb0,
    0x09,0x83,0x25,0xcc,0x12,0x22,0x58,0x54,0xee,0xba,0x4c,0x2d,0xba,0xa5,0x52,0xff,
    0x69,0x27,0x7a,0x60,0x11,0x0b,0x6e,0xb9,0x3f,0xa9,0x5d,0x58,0x20,0x0a,0x92,0x27,
    0x14,0x94,0xc8,0xef,0x92,0x4a,0xe0,0x88,0x6d,0x65,0x0a,0x35,0x42,0x18,0xc3,0x0e,
    0x5d,0xc6,0x30,0x12,0x66,0x9e,0xc1,0x97,0xad,0x94,0x32,0xda,0xd7,0x7a,0xaf,0x05,
    0xf2,0xbb,0xad,0x3e,0x63,0x50,0x5e,0x2b,0xc2,0x6d,0x5b,0x5f,0x08,0x35,0xcc,0xa7,
    0xbb,0x74,0x0d,0xc9,0xab,0x4a,0x88,0xaf,0x91,0xf0,0xf8,0x6e,0xcd,0x84,0x6c,0x50,
    0xfc,0x22,0x90,0xaf,0x5c,0x6a,0xce,0xde,0x25,0x4e,0x35,0xf5,0x62,0xf5,0x2b,0x00,
    0xdf,0x42,0x23,0xa5,0x63,0x39,0x71,0xfd,0xdd,0x3b,0x40,0xd6,0xc6,0xb8,0x26,0x58,
    0x16,0x0a,0x7d,0x5a,0xbd,0x4b,0x4a,0x9b,0x5a,0x1d,0x23,0x84,0x45,0x82,0x35,0x12,
    0x5e,0x9a,0x2b,0x5e,0x54,0xe0,0x61,0x4b,0xe5,0xee,0x8f,0x08,0x20,0x88,0xbf,0x76,
    0x5b,0x77,0xd2,0x75,0xcc,0xc7,0x85,0xa2,0xf5,0xf0,0xd4,0xb4,0xf2,0x2f,0xf2,0x74,
    0xaa,0x51,0xdb,0xe5,0xee,0x9d,0xe3,0x47,0xa7,0xa1,0x33,0xd3,0xdc,0x02,0x7a,0xb4,
    0x9e,0xf9,0x6e,0x02,0x56,0xe1,0x55,0x41,0x68,0xef,0x6c,0xa2,0x45,0xae,0xc1,0x54,
    0x1f,0x32,0x96,0xc0,0x43,0xee,0xe8,0x3a,0x21,0x48,0x5f,0xea,0x0e,0xd0,0xbf,0x08,
    0x2f,0x74,0x5d,0x13,0x88,0xc1,0xa7,0xfe,0xbf,0x0c,0xd1,0xa7,0xdf,0xb6,0x25,0x81
};
static const uint8_t message_oaep[] =
{
    0x66, 0x28, 0x19, 0x4e, 0x12, 0x07, 0x3d, 0xb0, 0x3b, 0xa9, 0x4c, 0xda, 0x9e, 0xf9, 0x53, 0x23,
    0x97, 0xd5, 0x0d, 0xba, 0x79, 0xb9, 0x87, 0x00, 0x4a, 0xfe, 0xfe, 0x34
};

static void icum_rsa_encrypt_oaep_parm(r_key_group_t parm_key_group, int parm_key_id, r_hash_primitive_t param_hash_primitive, uint32_t parm_modulus_bit_len)
{
    r_icumif_isd_t *p_ISD;
    uint16_t encMsg_size;
    uint32_t decMsg_size;

    // pause life signal management
    pause_life_signal();

    if( parm_key_group != KEY_GRP_RSA_ROM_PUB )
    {
        // initialize the global icum service header
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

        printf("\nPrepare RSA Decryption ##RSA PUB key import index #0 (pub)...\n\n");

        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        p_ISD->prm.PK_IMPORT.key_group                = parm_key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = parm_key_id;
        p_ISD->prm.PK_IMPORT.p_pub_exp                = pub_exp_oaep;
        p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = sizeof(pub_exp_oaep) * 8;
        if( parm_modulus_bit_len == 1024 )
        {
            p_ISD->prm.PK_IMPORT.p_modulus                = modulus_oeap;
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(modulus_oeap) * 8;
        }
        else    // 2048bit
        {
            p_ISD->prm.PK_IMPORT.p_modulus                = modulus_oeap_2048;
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(modulus_oeap_2048) * 8;
        }
        p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
        p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
        p_ISD->prm.PK_IMPORT.flags.write_protected    = 0;
        p_ISD->prm.PK_IMPORT.skip_verify              = 1;

        // trigger the security service
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            goto end;
        }
        printf("\nPub Key Import Done!\n");
    }

    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    printf("\nTesting RSA(OAEP) encryption...\n\n");
    p_ISD->service_id     = SERVICE_01_RSA_ENCRYPT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.RSA_ENCRYPT.p_message                = (void *)message_oaep;
    p_ISD->prm.RSA_ENCRYPT.message_size_in_bytes    = sizeof(message_oaep);
    p_ISD->prm.RSA_ENCRYPT.p_label                  = NULL;
    p_ISD->prm.RSA_ENCRYPT.label_size_in_bytes      = 0;
    p_ISD->prm.RSA_ENCRYPT.hash_primitive           = param_hash_primitive;
    p_ISD->prm.RSA_ENCRYPT.message_encoding_format  = ENCODING_FORMAT_EME_OAEP;
    p_ISD->prm.RSA_ENCRYPT.key_group                = parm_key_group;
    p_ISD->prm.RSA_ENCRYPT.key_id                   = parm_key_id;
    p_ISD->prm.RSA_ENCRYPT.encrypted_message_format = FORMAT_BINARY_PLAIN;
    p_ISD->prm.RSA_ENCRYPT.job_slice_mod_exp        = job_slice[JOB_RSA];
    p_ISD->prm.RSA_ENCRYPT.p_encrypted_message      = BIG_BUFFER[0];

    printf("Job slice rsa: %d block(s)\n", p_ISD->prm.RSA_ENCRYPT.job_slice_mod_exp);

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    if(p_ISD->service_result != SERV_OK)
    {
        printf("Fail encryption\n");
        goto end;
    }
    encMsg_size  = p_ISD->prm.RSA_ENCRYPT.encrypted_message_size_in_bits;
    
    printf("Message:           "); PMEM((const uint8_t *)message_oaep, sizeof(message_oaep)); NL(); FLUSH();
    printf("Encrypted Message Size: %d bytes\n", encMsg_size / 8);
    printf("Encrypted Message: "); PMEM((const uint8_t *)p_ISD->prm.RSA_ENCRYPT.p_encrypted_message, encMsg_size / 8); NL(); FLUSH();
    printf("Processed in :  "); PCHRONO(); NL(); FLUSH();

    if(( parm_key_group == KEY_GRP_RSA_PUB )&&( parm_key_id == 0 ))
    {
        // initialize the global icum service header
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

        printf("\nPrepare RSA Decryption ##RSA key import index #0 (priv)...\n\n");

        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA;
        p_ISD->prm.PK_IMPORT.key_id                   = 0;
        p_ISD->prm.PK_IMPORT.p_pub_exp                = pub_exp_oaep;
        p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = sizeof(pub_exp_oaep) * 8;
        if( parm_modulus_bit_len == 1024 )
        {
            p_ISD->prm.PK_IMPORT.p_priv_exp               = priv_exp_oaep;
            p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = sizeof(priv_exp_oaep) * 8;
            p_ISD->prm.PK_IMPORT.p_modulus                = modulus_oeap;
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(modulus_oeap) * 8;
        }
        else    // 2048bit
        {
            p_ISD->prm.PK_IMPORT.p_priv_exp               = priv_exp_oaep_2048;
            p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = sizeof(priv_exp_oaep_2048) * 8;
            p_ISD->prm.PK_IMPORT.p_modulus                = modulus_oeap_2048;
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(modulus_oeap_2048) * 8;
        }
        p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
        p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
        p_ISD->prm.PK_IMPORT.flags.write_protected    = 0;
        p_ISD->prm.PK_IMPORT.skip_verify              = 1;

        // trigger the security service
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            goto end;
        }
        printf("\nPriv Key Import Done!\n");

        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

        p_ISD->service_id     = SERVICE_01_RSA_DECRYPT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.RSA_DECRYPT.encrypted_message_size_in_bits   = encMsg_size;
        p_ISD->prm.RSA_DECRYPT.p_encrypted_message              = BIG_BUFFER[0];
        p_ISD->prm.RSA_DECRYPT.p_label                          = NULL;
        p_ISD->prm.RSA_DECRYPT.label_size_in_bytes              = 0;
        p_ISD->prm.RSA_DECRYPT.hash_primitive                   = param_hash_primitive;
        p_ISD->prm.RSA_DECRYPT.message_encoding_format          = ENCODING_FORMAT_EME_OAEP;
        p_ISD->prm.RSA_DECRYPT.key_id                           = 0;
        p_ISD->prm.RSA_DECRYPT.encrypted_message_format         = FORMAT_BINARY_PLAIN;
        p_ISD->prm.RSA_DECRYPT.job_slice_mod_exp                = job_slice[JOB_RSA];

        p_ISD->prm.RSA_DECRYPT.p_message                        = BIG_BUFFER[1];
        p_ISD->prm.RSA_DECRYPT.message_size_in_bytes            = sizeof(BIG_BUFFER[1]);

        printf("\nStart message decryption...\n\n");

        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        if(p_ISD->service_result != SERV_OK)
        {
            printf("Fail decryption\n");
            goto end;
        }
        
        // fetch execution time in vrun_chrono_xxx variables
        vrun_chrono_gettimestamp();
        
        decMsg_size  = p_ISD->prm.RSA_DECRYPT.message_size_in_bytes;
        
        printf("Decrypted Message Size: %d bytes\n", decMsg_size);
        printf("Decrypted Message:      "); PMEM((const unsigned char *)p_ISD->prm.RSA_DECRYPT.p_message, decMsg_size); NL(); FLUSH();
        printf("Processed in :  "); PCHRONO(); NL(); FLUSH();
    }

end:
    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}

void icum_rsa_encrypt_oaep(void)
{
    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA        (ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE=SHA2 256, Modulus bit length=1024\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA,         0, HASH_PRIMITIVE_SHA2_256, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_PUB    (ENCRYPT/DECRYPT), KEY_ID=0, HASH PRIMITIVE=SHA2 256, Modulus bit length=1024\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA_PUB,     0, HASH_PRIMITIVE_SHA2_256, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_ROM_PUB(ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE=SHA2 256, Modulus bit length=1024\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA_ROM_PUB, 0, HASH_PRIMITIVE_SHA2_256, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA        (ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE=SHA2 384, Modulus bit length=1024\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA,         0, HASH_PRIMITIVE_SHA2_384, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_PUB    (ENCRYPT/DECRYPT), KEY_ID=0, HASH PRIMITIVE SHA2 384, Modulus bit length=1024\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA_PUB,     0, HASH_PRIMITIVE_SHA2_384, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_ROM_PUB(ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE SHA2 384, Modulus bit length=1024\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA_ROM_PUB, 0, HASH_PRIMITIVE_SHA2_384, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA        (ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE=SHA2 512, Modulus bit length=2048\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA,         0, HASH_PRIMITIVE_SHA2_512, 2048 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_PUB    (ENCRYPT/DECRYPT), KEY_ID=0, HASH PRIMITIVE SHA2 512, Modulus bit length=2048\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA_PUB,     0, HASH_PRIMITIVE_SHA2_512, 2048 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_ROM_PUB(ENCRYPT only   ), KEY_ID=1, HASH PRIMITIVE SHA2 512, Modulus bit length=2048\n");
    icum_rsa_encrypt_oaep_parm( KEY_GRP_RSA_ROM_PUB, 1, HASH_PRIMITIVE_SHA2_512, 2048 );
}

// ********************************************************************************
// Testing RSA Encrypt PKCS#1 v15
// ********************************************************************************

static const uint8_t test_msg_eme_PKCSv15_11[] =
{
    0x66, 0x28, 0x19, 0x4e, 0x12, 0x07, 0x3d, 0xb0, 0x3b, 0xa9, 0x4c, 0xda, 0x9e, 0xf9, 0x53, 0x23,
    0x97, 0xd5, 0x0d, 0xba, 0x79, 0xb9, 0x87, 0x00, 0x4a, 0xfe, 0xfe, 0x34
};

static void icum_rsa_encrypt_pkcs1v15_parm(r_key_group_t parm_key_group, int parm_key_id, r_hash_primitive_t param_hash_primitive, uint32_t parm_modulus_bit_len)
{
    r_icumif_isd_t *p_ISD;
    uint16_t encMsg_size;
    uint32_t decMsg_size;

    // pause life signal management
    pause_life_signal();

    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );

    if( parm_key_group != KEY_GRP_RSA_ROM_PUB )
    {
         // initialize the global icum service header
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

        printf("\nPrepare RSA Encryption ##RSA PUB key import index #0 (pub)...\n\n");

        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        p_ISD->prm.PK_IMPORT.key_group                = parm_key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = parm_key_id;
        p_ISD->prm.PK_IMPORT.p_pub_exp                = pub_exp_oaep;
        p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = sizeof(pub_exp_oaep) * 8;
        if( parm_modulus_bit_len == 1024 )
        {
            p_ISD->prm.PK_IMPORT.p_modulus                = modulus_oeap;
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(modulus_oeap) * 8;
        }
        else    // 2048bit
        {
            p_ISD->prm.PK_IMPORT.p_modulus                = modulus_oeap_2048;
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(modulus_oeap_2048) * 8;
        }
        p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
        p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
        p_ISD->prm.PK_IMPORT.flags.write_protected    = 0;
        p_ISD->prm.PK_IMPORT.skip_verify              = 1;

        // trigger the security service
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            goto end;
        }
        printf("\nPub Key Import Done!\n");
    }

    printf("\nTesting RSA(PKCS#1 v15) encryption...\n\n");

    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    p_ISD->service_id     = SERVICE_01_RSA_ENCRYPT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.RSA_ENCRYPT.p_message                = (void *)test_msg_eme_PKCSv15_11;
    p_ISD->prm.RSA_ENCRYPT.message_size_in_bytes    = sizeof(test_msg_eme_PKCSv15_11);
    p_ISD->prm.RSA_ENCRYPT.p_label                  = NULL;
    p_ISD->prm.RSA_ENCRYPT.label_size_in_bytes      = 0;
    p_ISD->prm.RSA_ENCRYPT.hash_primitive           = param_hash_primitive;
    p_ISD->prm.RSA_ENCRYPT.message_encoding_format  = ENCODING_FORMAT_EME_PKCS1_V15;
    p_ISD->prm.RSA_ENCRYPT.key_group                = parm_key_group;
    p_ISD->prm.RSA_ENCRYPT.key_id                   = parm_key_id;
    p_ISD->prm.RSA_ENCRYPT.encrypted_message_format = FORMAT_BINARY_PLAIN;
    p_ISD->prm.RSA_ENCRYPT.job_slice_mod_exp        = job_slice[JOB_RSA];
    p_ISD->prm.RSA_ENCRYPT.p_encrypted_message      = BIG_BUFFER[0];

    printf("Job slice rsa: %d block(s)\n", p_ISD->prm.RSA_ENCRYPT.job_slice_mod_exp);

    printf("\nStart message encryption...\n\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    if(p_ISD->service_result != SERV_OK)
    {
        printf("Fail encryption\n");
        goto end;
    }
    
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    
    encMsg_size  = p_ISD->prm.RSA_ENCRYPT.encrypted_message_size_in_bits;
    
    printf("Message:           "); PMEM((const uint8_t *)test_msg_eme_PKCSv15_11, sizeof(test_msg_eme_PKCSv15_11)); NL(); FLUSH();
    printf("Encrypted Message Size: %d bytes\n", encMsg_size / 8);
    printf("Encrypted Message:      "); PMEM((const unsigned char *)p_ISD->prm.RSA_ENCRYPT.p_encrypted_message, encMsg_size / 8); NL(); FLUSH();
    printf("Processed in :  "); PCHRONO(); NL(); FLUSH();

    if(( parm_key_group == KEY_GRP_RSA_PUB )&&( parm_key_id == 0 ))
    {
        // initialize the global icum service header
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

        printf("\nPrepare RSA Encryption ##RSA key import index #0 (pri)...\n\n");

        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA;
        p_ISD->prm.PK_IMPORT.key_id                   = 0;
        p_ISD->prm.PK_IMPORT.p_pub_exp                = pub_exp_oaep;
        p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = sizeof(pub_exp_oaep) * 8;
        if( parm_modulus_bit_len == 1024 )
        {
            p_ISD->prm.PK_IMPORT.p_priv_exp               = priv_exp_oaep;
            p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = sizeof(priv_exp_oaep) * 8;
            p_ISD->prm.PK_IMPORT.p_modulus                = modulus_oeap;
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(modulus_oeap) * 8;
        }
        else    // 2048bit
        {
            p_ISD->prm.PK_IMPORT.p_priv_exp               = priv_exp_oaep_2048;
            p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = sizeof(priv_exp_oaep_2048) * 8;
            p_ISD->prm.PK_IMPORT.p_modulus                = modulus_oeap_2048;
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(modulus_oeap_2048) * 8;
        }
        p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
        p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
        p_ISD->prm.PK_IMPORT.flags.write_protected    = 0;
        p_ISD->prm.PK_IMPORT.skip_verify              = 1;

        // trigger the security service
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            goto end;
        }
        printf("\nPriv Key Import Done!\n");

        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        
        p_ISD->service_id     = SERVICE_01_RSA_DECRYPT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.RSA_DECRYPT.encrypted_message_size_in_bits   = encMsg_size;
        p_ISD->prm.RSA_DECRYPT.p_encrypted_message              = BIG_BUFFER[0];
        p_ISD->prm.RSA_DECRYPT.p_label                          = NULL;
        p_ISD->prm.RSA_DECRYPT.label_size_in_bytes              = 0;
        p_ISD->prm.RSA_DECRYPT.hash_primitive                   = param_hash_primitive;
        p_ISD->prm.RSA_DECRYPT.message_encoding_format          = ENCODING_FORMAT_EME_PKCS1_V15;
        p_ISD->prm.RSA_DECRYPT.key_id                           = 0;
        p_ISD->prm.RSA_DECRYPT.encrypted_message_format         = FORMAT_BINARY_PLAIN;
        p_ISD->prm.RSA_DECRYPT.job_slice_mod_exp                = job_slice[JOB_RSA];

        p_ISD->prm.RSA_DECRYPT.p_message                        = BIG_BUFFER[1];
        p_ISD->prm.RSA_DECRYPT.message_size_in_bytes            = sizeof(BIG_BUFFER[1]);

        printf("\nStart message decryption...\n\n");

        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        if(p_ISD->service_result != SERV_OK)
        {
            printf("Fail decryption\n");
            goto end;
        }
        
        // fetch execution time in vrun_chrono_xxx variables
        vrun_chrono_gettimestamp();
        
        decMsg_size  = p_ISD->prm.RSA_DECRYPT.message_size_in_bytes;
        
        printf("Decrypted Message Size: %d bytes\n", decMsg_size);
        printf("Decrypted Message:      "); PMEM((const unsigned char *)p_ISD->prm.RSA_DECRYPT.p_message, decMsg_size); NL(); FLUSH();
        printf("Processed in :  "); PCHRONO(); NL(); FLUSH();
    }

end:
    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}

void icum_rsa_encrypt_pkcs1v15(void)
{
    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA        (ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE=SHA2 256, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA,         0, HASH_PRIMITIVE_SHA2_256, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_PUB    (ENCRYPT/DECRYPT), KEY_ID=0, HASH PRIMITIVE=SHA2 256, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA_PUB,     0, HASH_PRIMITIVE_SHA2_256, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_ROM_PUB(ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE=SHA2 256, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA_ROM_PUB, 0, HASH_PRIMITIVE_SHA2_256, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA        (ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE=SHA2 384, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA,         0, HASH_PRIMITIVE_SHA2_384, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_PUB    (ENCRYPT/DECRYPT), KEY_ID=0, HASH PRIMITIVE SHA2 384, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA_PUB,     0, HASH_PRIMITIVE_SHA2_384, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_ROM_PUB(ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE SHA2 384, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA_ROM_PUB, 0, HASH_PRIMITIVE_SHA2_384, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA        (ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE=SHA2 512, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA,         0, HASH_PRIMITIVE_SHA2_512, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_PUB    (ENCRYPT/DECRYPT), KEY_ID=0, HASH PRIMITIVE SHA2 512, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA_PUB,     0, HASH_PRIMITIVE_SHA2_512, 1024 );

    printf("\n>>>>>>>>KEY GRP=KEY_GRP_RSA_ROM_PUB(ENCRYPT only   ), KEY_ID=0, HASH PRIMITIVE SHA2 512, Modulus bit length=1024\n");
    icum_rsa_encrypt_pkcs1v15_parm( KEY_GRP_RSA_ROM_PUB, 0, HASH_PRIMITIVE_SHA2_512, 1024 );
}

#include "rsa_pubkey_for_verify_test.h"

static const uint8_t *priv_exp_2048 =(void *)
    "58538655bf02c7ccbefc0b5b2ce171b0"
    "098325cc12225854eeba4c2dbaa552ff"
    "69277a60110b6eb93fa95d58200a9227"
    "1494c8ef924ae0886d650a354218c30e"
    "5dc63012669ec197ad9432dad77aaf05"
    "f2bbad3e63505e2bc26d5b5f0835cca7"
    "bb740dc9ab4a88af91f0f86ecd846c50"
    "fc2290af5c6acede254e35f562f52b00"
    "df4223a5633971fddd3b40d6c6b82658"
    "160a7d5abd4b4a9b5a1d238445823512"
    "5e9a2b5e54e0614be5ee8f082088bf76"
    "5b77d275ccc785a2f5f0d4b4f22ff274"
    "aa51dbe5ee9de347a7a133d3dc027ab4"
    "9ef96e0256e1554168ef6ca245aec154"
    "1f3296c043eee83a21485fea0ed0bf08"
    "2f745d1388c1a7febf0cd1a7dfb62581";

void icum_rsa_verify_test_KEY_GRP_RSA(struct _rsa_verify_testdata *p, uint8_t skip_verify)
{
    uint8_t *pwork;
    int i;

    r_icumif_isd_t *p_ISD;

    if(2048 != ((strlen(p->p_modulus)/2)*8)) return;

    pause_life_signal();

    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );

    /* initialize the global icum service header */
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  /* clear the ISD_BUFFER */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\n%s", p->title);
    printf("\nImporting RSA key pair #0...\n");

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA;
    p_ISD->prm.PK_IMPORT.key_id                   = 0;
    p_ISD->prm.PK_IMPORT.p_modulus                = (uint8_t *)(p->p_modulus);
    p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = p->modulusBits ? p->modulusBits : (strlen(p->p_modulus) /2) * 8;
    p_ISD->prm.PK_IMPORT.p_pub_exp                = (uint8_t *)(p->p_pub_e);
    p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = p->pub_eBits;
    p_ISD->prm.PK_IMPORT.p_priv_exp               = priv_exp_2048;
    p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = 2048;
    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_STRING_HEXA;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    p_ISD->prm.PK_IMPORT.skip_verify              = skip_verify;
    
    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result != SERV_OK) goto end;

    printf("\nVerify signature over test message...\n\n");

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA;
    p_ISD->prm.RSA_VERIFY.key_id                  = 0;
    p_ISD->prm.RSA_VERIFY.p_source                = (void *)p->bin_message;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes    = p->messageBytes;
    p_ISD->prm.RSA_VERIFY.hash_primitive          = p->hash_primitive;
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = p->salt_size_in_bytes;
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = p->hash_encoding_format;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.p_signature             = (void *)p->p_signature;
    p_ISD->prm.RSA_VERIFY.signature_size_in_bits  = p->signatureBits ? p->signatureBits : (strlen(p->p_signature) /2) * 8;
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_STRING_HEXA;

    /* trigger the security service */
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
        
    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        
    if(p_ISD->service_result != SERV_OK) goto end;
    
    printf("Verification result (FORMAT_STRING_HEXA)  : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n");
    }
    else    
    {
        printf("*FAIL*!\a\n");
    }

    /* FORMAT_BINARY_PLAIN */
    pwork = BIG_BUFFER[0];

    for( i=0; i<(strlen(p->p_signature) /2); i++ )
    {
        char wrk[3];

        wrk[0] = p->p_signature[(2*i) + 0];
        wrk[1] = p->p_signature[(2*i) + 1];
        wrk[2] = '\0';
        sscanf( wrk, "%x", &pwork[i] );
    }

    p_ISD->prm.RSA_VERIFY.p_signature             = pwork;
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_BINARY_PLAIN;

    /* trigger the security service */
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
        
    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        
    if(p_ISD->service_result != SERV_OK) goto end;
    
    printf("Verification result (FORMAT_BINARY_PLAIN) : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n\n");
    }
    else    
    {
        printf("*FAIL*!\a\n\n");
    }

end:
    /* restart life signal */
    restart_life_signal();

    printf("\nDone!\n");
}
