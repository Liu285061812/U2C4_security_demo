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

#ifndef _PE1_RSA_SIGNATURE_PKCS1_H_
#define _PE1_RSA_SIGNATURE_PKCS1_H_

/* Signature corresponding to test message test_msg_PSS_11 [] */

/* signatures for rsa_sign_verify_ex_PKCS1() */
/* Key and message are the same as PSS version */

/* Key length pattern */
// PKCS signature: key length 512 bit, SHA1
//0000 - a4 27 3b 1b 43 86 68 ff-c0 71 57 af 4e fe c2 da   .';.C.h..qW.N...
//0010 - ea 1d 31 8a 13 f8 0e 42-17 8f 3e 47 d4 23 97 ac   ..1....B..>G.#..
//0020 - df 1c f0 7c 86 25 69 79-74 39 ac 63 1b d2 b1 c0   ...|.%iyt9.c....
//0030 - 90 35 bb ad ff e8 88 c3-ff cf 97 28 fb c2 dd 95   .5.........(....
static const uint8_t sig_key512_sha1_pkcs[]=
"a4273b1b438668ffc07157af4efec2da"
"ea1d318a13f80e42178f3e47d42397ac"
"df1cf07c862569797439ac631bd2b1c0"
"9035bbadffe888c3ffcf9728fbc2dd95"
"";

// PKCS signature: key length 3072 bits, SHA1
//0000 - b6 ba 63 69 07 99 e9 24-00 3b 00 8b 5d db 03 bc   ..ci...$.;..]...
//0010 - 9a ac 89 38 f6 68 da dc-54 60 7d 77 80 0b f9 3a   ...8.h..T`}w...:
//0020 - 25 25 33 7f 33 5a 7d 22-52 46 2b a7 20 b8 ef e0   %%3.3Z}"RF+. ...
//0030 - 01 5a 39 8f 1d f2 91 5e-3c 5f 7d 4a cd e6 ad c2   .Z9....^<_}J....
//0040 - 6e f3 4a bc 3b 1f 6d 15-5f 53 aa ba c4 fe 07 cd   n.J.;.m._S......
//0050 - f3 c2 d1 ab 4d 8b 05 e5-73 50 85 6f 51 a4 86 63   ....M...sP.oQ..c
//0060 - c2 60 6a 48 6b a9 e4 37-c2 8f 1d e2 e8 32 fb 39   .`jHk..7.....2.9
//0070 - b0 84 a1 ff 9d 5b 01 34-eb a9 2e d4 7d 82 8b 1f   .....[.4....}...
//0080 - d1 bc 3a 31 e2 64 c1 84-08 62 8a 33 de 91 28 f4   ..:1.d...b.3..(.
//0090 - 66 8f 41 1c d8 64 ad e4-1a 3e 07 01 69 4a c2 d7   f.A..d...>..iJ..
//00a0 - 80 9e c5 36 89 c0 75 8d-ef 10 ef 6e 12 5f 34 43   ...6..u....n._4C
//00b0 - 7d cb e6 d7 17 6b e6 f1-ae 5e 33 79 8b 0e 82 5c   }....k...^3y...\
//00c0 - 05 20 54 0a e0 ce dc b0-63 c6 c1 5c fa 61 97 f4   . T.....c..\.a..
//00d0 - f6 2c 66 f9 9c 62 bd c2-ea 66 64 b3 d5 53 c0 d5   .,f..b...fd..S..
//00e0 - 6e bb be c2 d2 29 8f f1-6a 1e 5b 6e a0 b9 4a 1f   n....)..j.[n..J.
//00f0 - 10 74 06 bf 38 2b 83 6a-45 c4 a6 3d dd 85 a1 b6   .t..8+.jE..=....
//0100 - 75 5e 3a a6 9b 65 84 a0-5f 95 81 2b 72 ce 72 1c   u^:..e.._..+r.r.
//0110 - 5c 65 87 a7 3f fd 0b de-e3 08 fe 55 1e 46 65 e8   \e..?......U.Fe.
//0120 - eb 27 76 4a 32 ef 72 d4-0c e8 80 23 08 39 7a 72   .'vJ2.r....#.9zr
//0130 - cf 15 71 64 22 55 cb 30-f8 23 9b 99 a1 8f 8e 10   ..qd"U.0.#......
//0140 - 34 ac e8 88 9f 09 1e aa-85 87 7f 7b c5 c5 c1 66   4..........{...f
//0150 - 4f a7 19 d3 a6 01 5e ba-1b c6 ac 98 12 d3 8d 64   O.....^........d
//0160 - 8f 34 8b 0a 8f 81 ff 76-72 8d 37 7f 24 b7 d8 ad   .4.....vr.7.$...
//0170 - af ac 68 56 a9 e3 9b 30-93 19 80 e8 94 c6 e9 bd   ..hV...0........
static const uint8_t sig_key3072_sha1_pkcs[]=
"b6ba63690799e924003b008b5ddb03bc"
"9aac8938f668dadc54607d77800bf93a"
"2525337f335a7d2252462ba720b8efe0"
"015a398f1df2915e3c5f7d4acde6adc2"
"6ef34abc3b1f6d155f53aabac4fe07cd"
"f3c2d1ab4d8b05e57350856f51a48663"
"c2606a486ba9e437c28f1de2e832fb39"
"b084a1ff9d5b0134eba92ed47d828b1f"
"d1bc3a31e264c18408628a33de9128f4"
"668f411cd864ade41a3e0701694ac2d7"
"809ec53689c0758def10ef6e125f3443"
"7dcbe6d7176be6f1ae5e33798b0e825c"
"0520540ae0cedcb063c6c15cfa6197f4"
"f62c66f99c62bdc2ea6664b3d553c0d5"
"6ebbbec2d2298ff16a1e5b6ea0b94a1f"
"107406bf382b836a45c4a63ddd85a1b6"
"755e3aa69b6584a05f95812b72ce721c"
"5c6587a73ffd0bdee308fe551e4665e8"
"eb27764a32ef72d40ce8802308397a72"
"cf1571642255cb30f8239b99a18f8e10"
"34ace8889f091eaa85877f7bc5c5c166"
"4fa719d3a6015eba1bc6ac9812d38d64"
"8f348b0a8f81ff76728d377f24b7d8ad"
"afac6856a9e39b30931980e894c6e9bd"
"";

/* Message length pattern*/
// PKCS signature: key length 1024 bit, SHA1, message length 0
//0000 - 99 b9 b3 ae 49 e6 74 5a-3a 51 f6 0a a3 20 2f ce   ....I.tZ:Q... /.
//0010 - 6f cd f5 23 38 a5 fa 5c-5d 63 75 b8 e3 be 21 4d   o..#8..\]cu...!M
//0020 - 59 d9 4d 42 39 3c c8 68-ca 9e 32 2b f5 ca 5d 29   Y.MB9<.h..2+..])
//0030 - 0f 95 01 0a 25 42 6e 65-bd cc 7f 05 78 2f a5 42   ....%Bne....x/.B
//0040 - d4 67 89 db 99 c4 0f 0b-b7 28 71 07 5b ae 69 4b   .g.......(q.[.iK
//0050 - 05 26 ea 4f ec 3c 02 df-e4 e1 78 8f 66 6e 8c 85   .&.O.<....x.fn..
//0060 - 6b 22 0e 74 7f 67 cb 70-27 13 a8 41 4f 27 0f 73   k".t.g.p'..AO'.s
//0070 - 57 03 fa ad e6 61 9c 21-56 11 8f 0b 8a 31 1e b6   W....a.!V....1..
static const uint8_t sig_key1024_sha1_msg0_pkcs[]=
"99b9b3ae49e6745a3a51f60aa3202fce"
"6fcdf52338a5fa5c5d6375b8e3be214d"
"59d94d42393cc868ca9e322bf5ca5d29"
"0f95010a25426e65bdcc7f05782fa542"
"d46789db99c40f0bb72871075bae694b"
"0526ea4fec3c02dfe4e1788f666e8c85"
"6b220e747f67cb702713a8414f270f73"
"5703faade6619c2156118f0b8a311eb6"
"";

// PKCS signature: key length 1024 bit, SHA1, message length 1
//0000 - 97 e2 ab 6f 5e b4 08 28-ac bd a1 aa 67 79 1f 56   ...o^..(....gy.V
//0010 - 73 13 6e e2 b2 9c bd dd-57 49 01 02 7e 34 39 82   s.n.....WI..~49.
//0020 - 05 18 a3 64 e2 e6 34 39-8c b3 8b 4e 55 5b 72 e7   ...d..49...NU[r.
//0030 - 04 77 b1 83 94 48 73 73-21 cc 88 30 ba c0 78 69   .w...Hss!..0..xi
//0040 - a3 c5 20 57 7d 94 b6 77-79 4f 4e 7d 7a 28 da e4   .. W}..wyON}z(..
//0050 - 97 d4 07 e0 61 82 04 a3-aa 6b 31 e2 d4 b9 3e d8   ....a....k1...>.
//0060 - 37 65 83 f3 a4 9b f2 1a-0c a0 69 56 db 95 2a a3   7e........iV..*.
//0070 - e3 62 a1 dd 25 00 94 cf-e1 63 22 b0 bb b5 c2 99   .b..%....c".....
static const uint8_t sig_key1024_sha1_msg1_pkcs[]=
"97e2ab6f5eb40828acbda1aa67791f56"
"73136ee2b29cbddd574901027e343982"
"0518a364e2e634398cb38b4e555b72e7"
"0477b1839448737321cc8830bac07869"
"a3c520577d94b677794f4e7d7a28dae4"
"97d407e0618204a3aa6b31e2d4b93ed8"
"376583f3a49bf21a0ca06956db952aa3"
"e362a1dd250094cfe16322b0bbb5c299"
"";

// PKCS signature: key length 1024 bit, SHA1, message length 16
//0000 - 46 eb 13 66 49 f8 b6 f8-df 92 6c 3f 99 b5 a1 4b   F..fI.....l?...K
//0010 - 9d b2 55 36 6b a2 58 9a-31 ac 84 e8 eb ac 05 36   ..U6k.X.1......6
//0020 - de 5f 7e d7 c4 67 25 e8-62 84 fe 7b b1 82 62 49   ._~..g%.b..{..bI
//0030 - 30 cc 70 52 06 8e be 56-89 91 52 c6 eb 8c 0f a4   0.pR...V..R.....
//0040 - b3 33 75 b8 35 44 f1 8e-46 e0 0b 0f 34 9f 96 f6   .3u.5D..F...4...
//0050 - ac 10 56 c7 0e 5e 41 f5-fa d1 24 f0 45 f5 a7 14   ..V..^A...$.E...
//0060 - 1b 5a 09 e1 fb cf ed 08-e1 dd e9 69 17 94 d0 ba   .Z.........i....
//0070 - 44 9a 6e d5 46 70 9b 57-f8 3a 1b e6 b7 74 d5 88   D.n.Fp.W.:...t..
static const uint8_t sig_key1024_sha1_msg16_pkcs[]=
"46eb136649f8b6f8df926c3f99b5a14b"
"9db255366ba2589a31ac84e8ebac0536"
"de5f7ed7c46725e86284fe7bb1826249"
"30cc7052068ebe56899152c6eb8c0fa4"
"b33375b83544f18e46e00b0f349f96f6"
"ac1056c70e5e41f5fad124f045f5a714"
"1b5a09e1fbcfed08e1dde9691794d0ba"
"449a6ed546709b57f83a1be6b774d588"
"";

/* Hash pattern */
// PKCS signature: key length 2048 bits, SHA1
//0000 - 07 59 ea 42 1b 71 e1 09-a1 5d 2e 30 d7 0b 4f c3   .Y.B.q...].0..O.
//0010 - b4 96 d8 b0 18 b1 4c 61-92 37 00 c6 32 64 95 1d   ......La.7..2d..
//0020 - ef 7e 86 61 fc ce 8a db-a0 35 a0 27 eb b5 b7 a6   .~.a.....5.'....
//0030 - 61 ac 6f 7e 5e 4f a9 5b-d0 99 22 f4 54 78 60 2c   a.o~^O.[..".Tx`,
//0040 - 36 51 d4 20 bd c0 86 8f-9a 5a de 26 36 a5 fc 86   6Q. .....Z.&6...
//0050 - d7 e5 69 73 04 c5 93 b3-76 1d 31 bb df d6 ba 6c   ..is....v.1....l
//0060 - 66 f4 ab d2 c7 89 42 23-29 75 b8 0b 61 8f 1f f7   f.....B#)u..a...
//0070 - b4 bf 6b fa 9f 8c c5 86-7c 4c 9c b1 ab 05 c2 7a   ..k.....|L.....z
//0080 - 4e 87 55 b0 1f 57 33 04-f6 40 95 a8 f5 b9 30 95   N.U..W3..@....0.
//0090 - 00 5d 93 81 83 a0 9a 3f-1f 54 94 91 41 41 0f 0c   .].....?.T..AA..
//00a0 - fa 27 2f e3 cf 15 57 be-b4 9b 91 aa 3e 27 5e e7   .'/...W.....>'^.
//00b0 - aa 7f d5 ba 47 de 2d e1-f4 2f d6 17 2a b9 e3 ad   ....G.-../..*...
//00c0 - 60 c3 1a 5c 3f d1 9c bc-07 77 44 f8 20 30 a5 5d   `..\?....wD. 0.]
//00d0 - 5d 46 ed 34 c0 5a 49 3c-19 23 23 c6 47 73 e0 0b   ]F.4.ZI<.##.Gs..
//00e0 - af 56 b4 b5 e9 f0 88 0d-77 2f 82 aa 6a 17 4c 98   .V......w/..j.L.
//00f0 - d0 70 3d d4 23 e0 db d9-98 ed 05 bb 17 ff 8a 54   .p=.#..........T
static const uint8_t sig_key2048_sha1_pkcs[]=
"0759ea421b71e109a15d2e30d70b4fc3"
"b496d8b018b14c61923700c63264951d"
"ef7e8661fcce8adba035a027ebb5b7a6"
"61ac6f7e5e4fa95bd09922f45478602c"
"3651d420bdc0868f9a5ade2636a5fc86"
"d7e5697304c593b3761d31bbdfd6ba6c"
"66f4abd2c78942232975b80b618f1ff7"
"b4bf6bfa9f8cc5867c4c9cb1ab05c27a"
"4e8755b01f573304f64095a8f5b93095"
"005d938183a09a3f1f54949141410f0c"
"fa272fe3cf1557beb49b91aa3e275ee7"
"aa7fd5ba47de2de1f42fd6172ab9e3ad"
"60c31a5c3fd19cbc077744f82030a55d"
"5d46ed34c05a493c192323c64773e00b"
"af56b4b5e9f0880d772f82aa6a174c98"
"d0703dd423e0dbd998ed05bb17ff8a54"
"";

// PKCS signature: key length 2048 bits, SHA 256
//0000 - 10 5a f4 ba 8d 59 c6 e3-b2 02 3d 31 d7 ea 8b 70   .Z...Y....=1...p
//0010 - 28 3d 2f 4b f9 52 07 0b-4c 27 e0 ff e0 81 5f 03   (=/K.R..L'...._.
//0020 - 0f 5a f2 e8 fa 12 cc 3c-06 6d 39 54 4d 4b 20 d0   .Z.....<.m9TMK .
//0030 - 20 d9 8a 8c 36 66 1c d4-4d ae 69 90 7e 7e 59 c5    ...6f..M.i.~~Y.
//0040 - ab da a1 da 2d 5d 31 b3-00 a6 16 3d ce e9 ed 61   ....-]1....=...a
//0050 - 5d 61 ba 3b fa 21 96 44-3a 3d d8 99 09 13 a0 9b   ]a.;.!.D:=......
//0060 - 63 6b f3 c9 04 38 c9 dd-1f 2a 6f 15 65 63 0f 51   ck...8...*o.ec.Q
//0070 - 47 56 af e8 66 d6 40 7b-53 b1 e8 3e 3f e5 d3 7e   GV..f.@{S..>?..~
//0080 - 95 46 60 73 f7 20 f9 3c-8e 6d 7d d0 7e 59 99 ae   .F`s. .<.m}.~Y..
//0090 - 1e 10 fb 3d 95 9f 8b bf-62 ed da 91 4f d9 d8 f1   ...=....b...O...
//00a0 - 35 02 16 d3 4f 9e cd 19-35 00 68 ea 8f ef 78 7e   5...O...5.h...x~
//00b0 - 22 09 a4 b5 31 e7 0d 28-9b d3 a1 62 ab a5 2c c0   "...1..(...b..,.
//00c0 - ae 35 4a 0e e8 2d 71 35-e2 45 d6 9e 53 79 2e 7d   .5J..-q5.E..Sy.}
//00d0 - 6f 00 eb f0 55 77 2f 5f-01 f9 19 91 07 bd a0 42   o...Uw/_.......B
//00e0 - 17 eb 96 5e de 95 b2 9f-2a 1a 4b 0f 6d a7 1a f9   ...^....*.K.m...
//00f0 - 5b d1 af 45 a1 13 39 88-b4 c2 74 5b 9d 64 22 dd   [..E..9...t[.d".
static const uint8_t sig_key2048_sha256_pkcs[]=
"105af4ba8d59c6e3b2023d31d7ea8b70"
"283d2f4bf952070b4c27e0ffe0815f03"
"0f5af2e8fa12cc3c066d39544d4b20d0"
"20d98a8c36661cd44dae69907e7e59c5"
"abdaa1da2d5d31b300a6163dcee9ed61"
"5d61ba3bfa2196443a3dd8990913a09b"
"636bf3c90438c9dd1f2a6f1565630f51"
"4756afe866d6407b53b1e83e3fe5d37e"
"95466073f720f93c8e6d7dd07e5999ae"
"1e10fb3d959f8bbf62edda914fd9d8f1"
"350216d34f9ecd19350068ea8fef787e"
"2209a4b531e70d289bd3a162aba52cc0"
"ae354a0ee82d7135e245d69e53792e7d"
"6f00ebf055772f5f01f9199107bda042"
"17eb965ede95b29f2a1a4b0f6da71af9"
"5bd1af45a1133988b4c2745b9d6422dd"
"";

// PKCS signature: key length 2048 bits, SHA 512
//0000 - 54 18 ae 62 23 04 73 d7-bc 21 c2 94 06 0f 94 16   T..b#.s..!......
//0010 - 14 dd 44 ec 80 ef 02 73-7b 1e 13 16 49 4d 2e b1   ..D....s{...IM..
//0020 - 18 62 3d 4e e9 c5 bb 76-5c c1 37 d2 56 b4 4d 6a   .b=N...v\.7.V.Mj
//0030 - f0 a5 82 0f 54 93 05 84-d9 29 b9 47 f9 b9 39 8d   ....T....).G..9.
//0040 - 75 c6 52 a7 d2 86 95 13-89 1c a8 85 3e d3 46 63   u.R.........>.Fc
//0050 - 56 10 cb a5 9f 19 30 b9-91 85 9f 4d cb c3 70 1f   V.....0....M..p.
//0060 - 9c 06 f2 20 dc 7a 93 4f-f6 b9 75 57 75 12 ef bf   ... .z.O..uWu...
//0070 - 0e d6 fa 99 bb ef 67 d3-2b 0e 19 71 6f 9b 8f 15   ......g.+..qo...
//0080 - ce a4 92 55 fb 90 e5 fd-82 16 a0 90 d2 34 38 39   ...U.........489
//0090 - 0d b5 8d 8f dd ec 51 53-7a 98 9b b6 da e6 81 e1   ......QSz.......
//00a0 - d5 87 03 82 f3 37 7c ca-82 9d 51 43 90 f9 b4 36   .....7|...QC...6
//00b0 - fd 72 c0 1e c1 03 10 71-d4 07 3a 03 52 10 ac 65   .r.....q..:.R..e
//00c0 - 5e 0f 8b 7c 56 c9 42 79-05 30 1b ed ed cf b9 4a   ^..|V.By.0.....J
//00d0 - 6b 35 68 ef b7 db 24 44-11 4e e1 09 8f 5a 0f 6b   k5h...$D.N...Z.k
//00e0 - 1d 1d 42 80 03 e8 e2 96-cc 6a b2 6c 9c 37 c9 11   ..B......j.l.7..
//00f0 - 88 96 b6 26 f0 a4 ea ba-b4 b2 c4 99 64 ec 03 27   ...&........d..'
static const uint8_t sig_key2048_sha512_pkcs[]=
"5418ae62230473d7bc21c294060f9416"
"14dd44ec80ef02737b1e1316494d2eb1"
"18623d4ee9c5bb765cc137d256b44d6a"
"f0a5820f54930584d929b947f9b9398d"
"75c652a7d2869513891ca8853ed34663"
"5610cba59f1930b991859f4dcbc3701f"
"9c06f220dc7a934ff6b975577512efbf"
"0ed6fa99bbef67d32b0e19716f9b8f15"
"cea49255fb90e5fd8216a090d2343839"
"0db58d8fddec51537a989bb6dae681e1"
"d5870382f3377cca829d514390f9b436"
"fd72c01ec1031071d4073a035210ac65"
"5e0f8b7c56c9427905301bededcfb94a"
"6b3568efb7db2444114ee1098f5a0f6b"
"1d1d428003e8e296cc6ab26c9c37c911"
"8896b626f0a4eabab4b2c49964ec0327"
"";
#endif
