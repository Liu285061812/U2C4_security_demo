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

#if !defined(PE1_KEY_BUFFER_H)
#define PE1_KEY_BUFFER_H

// ********************************************************************************
// Prototypes
// ********************************************************************************
void icum_key_buffer_aes_key_update(void);
void icum_key_buffer_aes_key_cmac_test(void);

#endif //if !defined(PE1_KEY_BUFFER_H)

