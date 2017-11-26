/*******************************************************************************
* File Name: ChannelSel.h
* Version 1.80
*
*  Description:
*    This file contains the constants and function prototypes for the Analog
*    Multiplexer User Module AMux.
*
*   Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_AMUX_ChannelSel_H)
#define CY_AMUX_ChannelSel_H

#include "cyfitter.h"
#include "cyfitter_cfg.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
         (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
         (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))    
    #include "cytypes.h"
#else
    #include "syslib/cy_syslib.h"
#endif /* ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) */


/***************************************
*        Function Prototypes
***************************************/

void ChannelSel_Start(void) ;
#define ChannelSel_Init() ChannelSel_Start()
void ChannelSel_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void ChannelSel_Stop(void); */
/* void ChannelSel_Select(uint8 channel); */
/* void ChannelSel_Connect(uint8 channel); */
/* void ChannelSel_Disconnect(uint8 channel); */
/* void ChannelSel_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define ChannelSel_CHANNELS  5u
#define ChannelSel_MUXTYPE   1
#define ChannelSel_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define ChannelSel_NULL_CHANNEL 0xFFu
#define ChannelSel_MUX_SINGLE   1
#define ChannelSel_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if ChannelSel_MUXTYPE == ChannelSel_MUX_SINGLE
# if !ChannelSel_ATMOSTONE
#  define ChannelSel_Connect(channel) ChannelSel_Set(channel)
# endif
# define ChannelSel_Disconnect(channel) ChannelSel_Unset(channel)
#else
# if !ChannelSel_ATMOSTONE
void ChannelSel_Connect(uint8 channel) ;
# endif
void ChannelSel_Disconnect(uint8 channel) ;
#endif

#if ChannelSel_ATMOSTONE
# define ChannelSel_Stop() ChannelSel_DisconnectAll()
# define ChannelSel_Select(channel) ChannelSel_FastSelect(channel)
void ChannelSel_DisconnectAll(void) ;
#else
# define ChannelSel_Stop() ChannelSel_Start()
void ChannelSel_Select(uint8 channel) ;
# define ChannelSel_DisconnectAll() ChannelSel_Start()
#endif

#endif /* CY_AMUX_ChannelSel_H */


/* [] END OF FILE */
