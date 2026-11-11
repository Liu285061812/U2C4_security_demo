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

#ifndef _PE1_RSA_KEYS_H_
#define _PE1_RSA_KEYS_H_

// in "key1024.pem", "key512.pem", "key2048.pem", "key3072.pem"
static const uint8_t *strE10001 =(void *)
    "010001";


// key 512pem module : in "key512.pem"
static const uint8_t *N_SIG_VERF_512=(void *)
    "00c0ba230e0d1fd571923acdc4012a"
    "87aa4332c0170b643eb32c4796a1dc"
    "fb41ef251ba09e9bbbb888dc4ae1d6"
    "ae660eaa8f07d7f8643bc5cda1fd52"
    "b1134446e1";
#ifdef NEED_PRIV_KEY
//privateExponent:
//  00:a8:b4:47:6b:4d:61:c8:5d:9a:3b:8f:53:3c:12:
//  16:9f:9c:23:18:b3:13:6d:b4:d4:f2:b5:d7:cd:e8:
//  ba:a8:83:41:34:67:a9:1c:54:10:fc:6a:e6:fe:87:
//  45:b1:37:bc:44:9d:a0:47:a5:20:e8:65:9c:d2:45:
//  c6:10:af:a2:01
static const uint8_t *D_SIG_VERF_512=(void *)
    "00a8b4476b4d61c85d9a3b8f533c12"
    "169f9c2318b3136db4d4f2b5d7cde8"
    "baa883413467a91c5410fc6ae6fe87"
    "45b137bc449da047a520e8659cd245"
    "c610afa201";
#endif

// key1024pem modules : in "key1024.pem"
static const uint8_t *N_SIG_VERF_1024=(void *)
    "00c665dd3f014b764679439fc13700"
    "52827839d93936a228243a8b831183"
    "de11bbcb413b1836bf229ae6c0f7bc"
    "e31315cba1f1146d8a61fedb03091c"
    "7d24c902bf2add2e9a4e93f18e9a2f"
    "470e8b94f5e126e0fd51a39c2563a0"
    "387daec7d3da50352da8d92a790b9a"
    "2c8d27dd01470ebfec798e64b743a3"
    "2832da8b37dceb8ccf";
#ifdef NEED_PRIV_KEY
//privateExponent:
//    5a:9e:40:55:fc:d3:68:ec:7b:8a:e5:b7:2c:d6:1a:
//    d9:b8:ec:43:49:9c:a8:a8:49:13:ec:91:2b:38:dd:
//    ff:9f:95:10:f0:5a:f8:f6:88:69:d4:28:23:7f:c4:
//    ff:fc:9e:8c:13:3f:4f:61:3b:ad:5a:7f:a2:73:be:
//    bd:44:8a:d9:d3:ec:94:bd:7f:7a:93:41:60:38:1d:
//    c2:d1:52:90:76:47:a2:69:7a:49:a2:a7:fb:41:c4:
//    54:79:4d:09:f0:19:48:00:57:16:c1:aa:58:4f:fd:
//    4a:86:4f:87:ee:17:ba:75:1f:3f:7c:99:8b:b4:ab:
//    a0:bd:47:b7:88:82:4a:01
static const uint8_t *D_SIG_VERF_1024=(void *)
    "5a9e4055fcd368ec7b8ae5b72cd61a"
    "d9b8ec43499ca8a84913ec912b38dd"
    "ff9f9510f05af8f68869d428237fc4"
    "fffc9e8c133f4f613bad5a7fa273be"
    "bd448ad9d3ec94bd7f7a934160381d"
    "c2d152907647a2697a49a2a7fb41c4"
    "54794d09f01948005716c1aa584ffd"
    "4a864f87ee17ba751f3f7c998bb4ab"
    "a0bd47b788824a01";
#endif

// key 2048pem module : in "key2048.pem"
static const uint8_t *N_SIG_VERF_2048=(void *)
    "00c4b1178cab0dd2084e27b886846e"
    "d7b41b3ca877c306c111680bc92a09"
    "f166901eadd4309a4f15f19da8acbb"
    "84ebb76ee56eeba186c34190657042"
    "c175acc821454c449ea6b5cd7fea1c"
    "fcced6a0137d8942c0be8a04c79de2"
    "89f0d5405c09ee03212ff1be612555"
    "d7e300898f309252a4045b3f6c247e"
    "3e1323d968d380b44b67280718f2aa"
    "ceaf51cebc9616ce48c35117e230a4"
    "e5b99aa572a6cbb8cec5b84d1d802e"
    "47a17d1add4ae00f87b1c7d1d3853c"
    "19a357e7f1e836e6667d6b4e7991cd"
    "4e5af590ccab007976dbef53c6098a"
    "bd9664c80528a9b79713a592541afb"
    "2c3f4dd903bf96d8d32a795e7e7f1d"
    "41058e93236b9e67246293fd61ab5c"
    "2395";
#ifdef NEED_PRIV_KEY
//privateExponent:
//    58:53:86:55:bf:02:c7:cc:be:fc:0b:5b:2c:e1:71:
//    b0:09:83:25:cc:12:22:58:54:ee:ba:4c:2d:ba:a5:
//    52:ff:69:27:7a:60:11:0b:6e:b9:3f:a9:5d:58:20:
//    0a:92:27:14:94:c8:ef:92:4a:e0:88:6d:65:0a:35:
//    42:18:c3:0e:5d:c6:30:12:66:9e:c1:97:ad:94:32:
//    da:d7:7a:af:05:f2:bb:ad:3e:63:50:5e:2b:c2:6d:
//    5b:5f:08:35:cc:a7:bb:74:0d:c9:ab:4a:88:af:91:
//    f0:f8:6e:cd:84:6c:50:fc:22:90:af:5c:6a:ce:de:
//    25:4e:35:f5:62:f5:2b:00:df:42:23:a5:63:39:71:
//    fd:dd:3b:40:d6:c6:b8:26:58:16:0a:7d:5a:bd:4b:
//    4a:9b:5a:1d:23:84:45:82:35:12:5e:9a:2b:5e:54:
//    e0:61:4b:e5:ee:8f:08:20:88:bf:76:5b:77:d2:75:
//    cc:c7:85:a2:f5:f0:d4:b4:f2:2f:f2:74:aa:51:db:
//    e5:ee:9d:e3:47:a7:a1:33:d3:dc:02:7a:b4:9e:f9:
//    6e:02:56:e1:55:41:68:ef:6c:a2:45:ae:c1:54:1f:
//    32:96:c0:43:ee:e8:3a:21:48:5f:ea:0e:d0:bf:08:
//    2f:74:5d:13:88:c1:a7:fe:bf:0c:d1:a7:df:b6:25:
//    81
static const uint8_t *D_SIG_VERF_2048=(void *)
    "58538655bf02c7ccbefc0b5b2ce171"
    "b0098325cc12225854eeba4c2dbaa5"
    "52ff69277a60110b6eb93fa95d5820"
    "0a92271494c8ef924ae0886d650a35"
    "4218c30e5dc63012669ec197ad9432"
    "dad77aaf05f2bbad3e63505e2bc26d"
    "5b5f0835cca7bb740dc9ab4a88af91"
    "f0f86ecd846c50fc2290af5c6acede"
    "254e35f562f52b00df4223a5633971"
    "fddd3b40d6c6b82658160a7d5abd4b"
    "4a9b5a1d2384458235125e9a2b5e54"
    "e0614be5ee8f082088bf765b77d275"
    "ccc785a2f5f0d4b4f22ff274aa51db"
    "e5ee9de347a7a133d3dc027ab49ef9"
    "6e0256e1554168ef6ca245aec1541f"
    "3296c043eee83a21485fea0ed0bf08"
    "2f745d1388c1a7febf0cd1a7dfb625"
    "81";
#endif

// key 3072pem module : in "key3072.pem"
static const uint8_t *N_SIG_VERF_3072=(void *)
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
#ifdef NEED_PRIV_KEY
//privateExponent:
//    08:bf:90:c7:9f:62:a3:f8:f8:3b:2c:7a:58:c4:5b:
//    5d:f8:45:37:40:b9:16:ff:1f:61:14:4e:4f:a3:17:
//    c6:3b:73:27:ed:91:b7:88:14:d3:5e:c2:10:ea:96:
//    ab:85:3d:e7:ba:2b:7e:fe:76:02:a8:56:a6:f0:e3:
//    9c:67:9a:c4:7a:01:5a:25:2b:7f:b1:74:a8:61:df:
//    4d:13:2d:71:97:5c:68:e5:e0:5f:9f:bc:ab:c4:65:
//    e9:c2:a0:ea:97:37:83:4c:1a:8b:93:e2:fc:01:20:
//    9c:1b:bb:a9:ca:5c:c3:92:1f:95:d1:0d:f9:ad:4e:
//    be:a3:f1:3b:f6:67:38:a6:0f:33:fd:58:c6:d7:2a:
//    c9:50:f7:37:61:6c:11:45:bb:57:c9:98:71:b7:a3:
//    97:7c:53:54:62:de:63:a3:0c:df:a3:3a:28:d2:99:
//    86:25:6a:1f:7f:a8:d3:4b:c0:9f:90:67:f5:f3:c0:
//    47:e2:23:26:7f:01:1c:bd:85:83:2f:9a:d7:7e:1f:
//    63:e1:78:98:87:d3:13:8a:53:2e:a4:40:cd:f2:1d:
//    0d:a7:db:4f:e9:04:ef:d4:93:80:d7:60:e9:f6:f5:
//    e6:75:de:4f:74:35:09:bc:cd:e0:96:3c:c4:95:1a:
//    8d:cf:2e:4b:f1:e3:ef:ef:50:f9:42:97:e3:66:3e:
//    65:e9:af:7e:5e:8c:da:95:3f:50:be:be:73:6f:28:
//    ee:dd:1f:99:58:19:f9:12:23:65:3e:9d:29:80:0a:
//    35:86:c8:8a:39:8d:44:2b:62:7f:69:f9:40:81:5c:
//    78:87:ab:50:c4:73:21:cb:fc:d0:aa:c2:b4:4d:ba:
//    e1:dd:88:45:c7:81:7e:08:22:f3:ba:6c:e3:c2:7b:
//    16:49:4a:4b:2d:4f:72:59:ae:46:a2:b8:ff:3d:1b:
//    81:a2:47:6c:88:24:77:fa:b1:07:0d:45:69:f4:e5:
//    67:e1:c4:f5:9b:91:5e:e6:22:a9:46:7d:4a:ee:c1:
//    5a:e7:81:df:39:2b:be:a2:a1
static const uint8_t *D_SIG_VERF_3072=(void *)
    "08bf90c79f62a3f8f83b2c7a58c45b"
    "5df8453740b916ff1f61144e4fa317"
    "c63b7327ed91b78814d35ec210ea96"
    "ab853de7ba2b7efe7602a856a6f0e3"
    "9c679ac47a015a252b7fb174a861df"
    "4d132d71975c68e5e05f9fbcabc465"
    "e9c2a0ea9737834c1a8b93e2fc0120"
    "9c1bbba9ca5cc3921f95d10df9ad4e"
    "bea3f13bf66738a60f33fd58c6d72a"
    "c950f737616c1145bb57c99871b7a3"
    "977c535462de63a30cdfa33a28d299"
    "86256a1f7fa8d34bc09f9067f5f3c0"
    "47e223267f011cbd85832f9ad77e1f"
    "63e1789887d3138a532ea440cdf21d"
    "0da7db4fe904efd49380d760e9f6f5"
    "e675de4f743509bccde0963cc4951a"
    "8dcf2e4bf1e3efef50f94297e3663e"
    "65e9af7e5e8cda953f50bebe736f28"
    "eedd1f995819f91223653e9d29800a"
    "3586c88a398d442b627f69f940815c"
    "7887ab50c47321cbfcd0aac2b44dba"
    "e1dd8845c7817e0822f3ba6ce3c27b"
    "16494a4b2d4f7259ae46a2b8ff3d1b"
    "81a2476c882477fab1070d4569f4e5"
    "67e1c4f59b915ee622a9467d4aeec1"
    "5ae781df392bbea2a1";
#endif

/*****************************************/
/* Key to import in menu 300-300-1       */
/*****************************************/
// source: https://github.com/pyca/cryptography/blob/master/vectors/cryptography_vectors/asymmetric/RSA/pkcs-1v2-1d2-vec/pss-vect.txt

static const uint8_t *N_1024 =(void *)
    "A56E4A0E701017589A5187DC7EA841D156F2EC0E36AD52A44DFEB1E61F7AD991"
    "D8C51056FFEDB162B4C0F283A12A88A394DFF526AB7291CBB307CEABFCE0B1DF"
    "D5CD9508096D5B2B8B6DF5D671EF6377C0921CB23C270A70E2598E6FF89D19F1"
    "05ACC2D3F0CB35F29280E1386B6F64C4EF22E1E1F20D0CE8CFFB2249BD9A2137";
#ifdef NEED_PRIV_KEY
//# Exponent: 
// 33 a5 04 2a 90 b2 7d 4f 54 51 ca 9b bb d0 b4 47 
// 71 a1 01 af 88 43 40 ae f9 88 5f 2a 4b be 92 e8 
// 94 a7 24 ac 3c 56 8c 8f 97 85 3a d0 7c 02 66 c8 
// c6 a3 ca 09 29 f1 e8 f1 12 31 88 44 29 fc 4d 9a 
// e5 5f ee 89 6a 10 ce 70 7c 3e d7 e7 34 e4 47 27 
// a3 95 74 50 1a 53 26 83 10 9c 2a ba ca ba 28 3c 
// 31 b4 bd 2f 53 c3 ee 37 e3 52 ce e3 4f 9e 50 3b 
// d8 0c 06 22 ad 79 c6 dc ee 88 35 47 c6 a3 b3 25 
static const uint8_t *D_1024=(void *)
    "33a5042a90b27d4f5451ca9bbbd0b447"
    "71a101af884340aef9885f2a4bbe92e8"
    "94a724ac3c568c8f97853ad07c0266c8"
    "c6a3ca0929f1e8f11231884429fc4d9a"
    "e55fee896a10ce707c3ed7e734e44727"
    "a39574501a532683109c2abacaba283c"
    "31b4bd2f53c3ee37e352cee34f9e503b"
    "d80c0622ad79c6dcee883547c6a3b325";
#endif

#endif/*_PE1_RSA_KEYS_H_*/
