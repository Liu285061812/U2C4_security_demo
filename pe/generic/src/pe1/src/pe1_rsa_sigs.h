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

#ifndef _PE1_RSA_SIGS_H_
#define _PE1_RSA_SIGS_H_

/* Signature corresponding to test message test_msg_PSS_11 [] */

/******************************************/
/* Signature pattern according to various message length */
/******************************************/
// PSS signature: key length 512 bit, SHA1, SALT length 20
// from rsa_data/sig_key512_sha1_salt20.txt
static const uint8_t sig_key512_sha1_salt20[]=
"8b15f9a82fa5aa237ec84a3bda5af706"
"571eaa567e4bc836e891f2986cb07ae3"
"17dac46985ee0a170030b72421348d26"
"373dc19d4d48b6c184ce275d584c2144";

// PSS signature: key length 1024 bit, SHA1, message length 0
// from rsa_data/sig_key1024_sha1_msg0.txt
static const uint8_t sig_key1024_sha1_msg0[]=
"3274de9ace79b55e28d422563e88edc1"
"9085bd0d896a10a423442239c4a928f7"
"c6a89326d030e43f538c42ee9751e028"
"888a8c9b16fceeb63378ee168cb034cc"
"dd354fcb2c7f7780ad81ecf8716825b7"
"882cdee23f54be38c739afd18dcb5561"
"94d5865c2a1d8240ec62b45a01b428f7"
"7e327ed6f7164a37bf01cb73712311b6";

// PSS signature: key length 1024 bit, SHA1, message length 1
// from rsa_data/sig_key1024_sha1_msg1.txt
static const uint8_t sig_key1024_sha1_msg1[]=
"4f9f7dcda321c7b79471276ae1ad6414"
"c7b8b2a92a26650abd0465ab13f27ff7"
"2d4a8a72b28c9dedac84908a09ee4765"
"fbe7df03209f1f1e907b58d98676ce7c"
"c32d7af95030a0bc890ed95380a34b03"
"477fcff165554c143f1101e53f2c30c3"
"d082d255c29a9a44491d99aa874d4241"
"707eacef62150632f05a60fc2efca16e";

// PSS signature: key length 1024 bit, SHA1, message length 16
// from rsa_data/sig_key1024_sha1_msg16.txt
static const uint8_t sig_key1024_sha1_msg16[]=
"a4ff4dd1ea23405f5b3c046fd35430f0"
"435c522f04d464d47f310a2819561758"
"d6bb879237ac2459397e26210d538393"
"ee24b0d0869eacaea5f97c881ee7f128"
"6412e691ac098606c9028de69c22b0e2"
"33570fcfdc669057f14ed108373bc6ba"
"fb9e64c774b071f35224a6855de6a09a"
"a246d9c0279b086a74ec86badefa2e59";

/******************************************/
/* Signature pattern according to various salt_length  */
/******************************************/
// PSS signature: key length 1024 bit, SHA1, SALT length 0
// from rsa_data/sig_key1024_sha1_salt0.txt
static const uint8_t sig_key1024_sha1_salt0[]=
"25cbececadfb8d538c9cd4ef066b65cf"
"0418ff0b97a7995f0097452d3625a1fa"
"e74067e35b5822cd797ed6532f67564f"
"2b08c863bcf881be50632c873a54b414"
"41c340b2314c4e12bfc3ebf1229a4e36"
"502aee4aeb6e9f446419b3523b69671f"
"15fbb03f124e7fad9e64c037628b92d6"
"bf8872ed1d8beb28a590bc16a9bd2741";

// PSS signature: key length 1024 bit, SHA1, SALT length 1
// from rsa_data/sig_key1024_sha1_salt1.txt
static const uint8_t sig_key1024_sha1_salt1[]=
"0ad81e177435c69233799a77af62e2b0"
"d5e0da226e6c5ce74f0899ca8721cf15"
"88e3bca8b54c6b7e0b5d0d6014ffad47"
"e4bfcdefd9389a2024da95922145b4b0"
"e197b408b32e00e80f37d0887b47b9a7"
"6bd11e2061109f783ce05592b3965b53"
"d1e08bd58a959ec5c0d215e7f20aebd2"
"86e58ca68c440478c4011db5564f21ef";

// PSS signature: key length 1024 bit, SHA1, SALT length 106
// from rsa_data/sig_key1024_sha1_salt106.txt
static const uint8_t sig_key1024_sha1_salt106[]=
"775da66b7fd92155403bc3cd3ddbc56c"
"6b239a30af2442cf50843b4ec5b4fe1d"
"a3d44574b2f081c309558298ba995c93"
"6f01b800661ea778496a7c5674dfca47"
"cb9b64859d7d521d7dfc2475cc1f396e"
"fbe012fc95d7e55e445b9c5a6b691236"
"99695a6f1ffe9459628bf5e811bc0189"
"734f42b6afa03be9a3df44335b20cc8c";

/******************************************/
/* Pattern of signature by various Hash   */
/******************************************/
// PSS signature: key length 2048 bits, SHA1, SALT length 20
// from rsa_data/sig_key2048_sha1_salt20.txt
static const uint8_t sig_key2048_sha1_salt20[]=
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


// PSS signature: key length 2048 bits, SHA 256, SALT length 20
// from rsa_data/sig_key2048_sha256_salt20.txt
static const uint8_t sig_key2048_sha256_salt20[]=
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

// PSS signature: key length 2048 bits, SHA 512, SALT length 20
// from rsa_data/sig_key2048_sha512_salt20.txt
static const uint8_t sig_key2048_sha512_salt20[]=
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

// PSS signature: key length 3072 bits, SHA1, SALT length 20
// from rsa_data/sig_key3072_sha1_salt20.txt
static const uint8_t sig_key3072_sha1_salt20[]=
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

#endif /*_PE1_RSA_SIGS_H_*/
