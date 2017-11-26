/*******************************************************************************
* File Name: IDAC8_4.c
* Version 2.0
*
* Description:
*  This file contains the function prototypes and constants used in
*  the 8-bit Current DAC (IDAC8) User Module.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_IDAC8_IDAC8_4_H)  
#define CY_IDAC8_IDAC8_4_H

#include "cyfitter.h"
#include "cytypes.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component IDAC8_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*       Type defines
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 data_value;
}IDAC8_4_backupStruct;


#if (CY_PSOC5A)
    /* Stop API changes for PSoC5A */
    typedef struct
    {
        uint8 DACCR0Reg;
    }   IDAC8_4_LOWPOWER_BACKUP_STRUCT;
#endif /* CY_PSOC5A */

/* component init state */
extern uint8 IDAC8_4_initVar;


/***************************************
*        Function Prototypes 
***************************************/

void    IDAC8_4_Start(void)                 ;
void    IDAC8_4_Stop(void)                  ;
void    IDAC8_4_SetSpeed(uint8 speed)       ;
void    IDAC8_4_SetPolarity(uint8 polarity) ;
void    IDAC8_4_SetRange(uint8 range)      ;
void    IDAC8_4_SetValue(uint8 value)       ;
void    IDAC8_4_DacTrim(void)               ;

/* Sleep Retention APIs */
void IDAC8_4_Init(void)                     ;
void IDAC8_4_Enable(void)                   ;
void IDAC8_4_SaveConfig(void)               ;
void IDAC8_4_RestoreConfig(void)            ;
void IDAC8_4_Sleep(void)                    ;
void IDAC8_4_Wakeup(void)                   ;


/***************************************
*       Paramater Initial Values
***************************************/

#define IDAC8_4_DEFAULT_RANGE     4u                                   /* Default DAC range */
#define IDAC8_4_DEFAULT_SPEED     ((uint8)(((2u != 0u) ? 1u : 0u) << 1u))   /* Default DAC speed */
#define IDAC8_4_DEFAULT_CNTL      0x00u                                            /* Default Control */
#define IDAC8_4_DEFAULT_STRB     0u                                   /* Default Strobe mode */
#define IDAC8_4_DEFAULT_DATA     0u                                 /* Initial DAC value */
#define IDAC8_4_DEFAULT_POLARITY 0u                                      /* Default Sink or Source */
#define IDAC8_4_DEFAULT_DATA_SRC 0u                                   /* Default Data Source */   
#define IDAC8_4_HARDWARE_ENABLE  0u                               /*Hardware Enable */


/***************************************
*              API Constants        
***************************************/

/* SetRange constants */

#define IDAC8_4_RANGE_32uA             (0x00u)
#define IDAC8_4_RANGE_255uA            (0x04u)
#define IDAC8_4_RANGE_2mA              (0x08u)

/* SetPolarity constants */
#define IDAC8_4_SOURCE                 (0x00u)
#define IDAC8_4_SINK                   (0x04u)
#define IDAC8_4_HARDWARE_CONTROLLED    (0x02u)

/* Power setting for SetSpeed API  */
#define IDAC8_4_LOWSPEED               (0x00u)
#define IDAC8_4_HIGHSPEED              (0x02u)


/***************************************
*              Registers        
***************************************/
#define IDAC8_4_CR0_REG         (* (reg8 *) IDAC8_4_viDAC8__CR0 )
#define IDAC8_4_CR0_PTR         (  (reg8 *) IDAC8_4_viDAC8__CR0 )
#define IDAC8_4_CR1_REG         (* (reg8 *) IDAC8_4_viDAC8__CR1 )
#define IDAC8_4_CR1_PTR         (  (reg8 *) IDAC8_4_viDAC8__CR1 )
#define IDAC8_4_Data_REG        (* (reg8 *) IDAC8_4_viDAC8__D )
#define IDAC8_4_Data_PTR        (  (reg8 *) IDAC8_4_viDAC8__D )
#define IDAC8_4_Strobe_REG      (* (reg8 *) IDAC8_4_viDAC8__STROBE )
#define IDAC8_4_Strobe_PTR      (  (reg8 *) IDAC8_4_viDAC8__STROBE )
#define IDAC8_4_SW0_REG         (* (reg8 *) IDAC8_4_viDAC8__SW0 )
#define IDAC8_4_SW0_PTR         (  (reg8 *) IDAC8_4_viDAC8__SW0 )
#define IDAC8_4_SW2_REG         (* (reg8 *) IDAC8_4_viDAC8__SW2 )
#define IDAC8_4_SW2_PTR         (  (reg8 *) IDAC8_4_viDAC8__SW2 )
#define IDAC8_4_SW3_REG         (* (reg8 *) IDAC8_4_viDAC8__SW3 )
#define IDAC8_4_SW3_PTR         (  (reg8 *) IDAC8_4_viDAC8__SW3 )
#define IDAC8_4_SW4_REG         (* (reg8 *) IDAC8_4_viDAC8__SW4 )
#define IDAC8_4_SW4_PTR         (  (reg8 *) IDAC8_4_viDAC8__SW4 )
#define IDAC8_4_TR_REG          (* (reg8 *) IDAC8_4_viDAC8__TR )
#define IDAC8_4_TR_PTR          (  (reg8 *) IDAC8_4_viDAC8__TR )
#define IDAC8_4_PWRMGR_REG      (* (reg8 *) IDAC8_4_viDAC8__PM_ACT_CFG )  /* Power manager */
#define IDAC8_4_PWRMGR_PTR      (  (reg8 *) IDAC8_4_viDAC8__PM_ACT_CFG )  /* Power manager */
#define IDAC8_4_STBY_PWRMGR_REG (* (reg8 *) IDAC8_4_viDAC8__PM_STBY_CFG )  /* Standby Power manager */
#define IDAC8_4_STBY_PWRMGR_PTR (  (reg8 *) IDAC8_4_viDAC8__PM_STBY_CFG )  /* Standby Power manager */

/***************************************
*  Registers definitions
* for backward capability        
***************************************/
#define IDAC8_4_CR0         (* (reg8 *) IDAC8_4_viDAC8__CR0 )
#define IDAC8_4_CR1         (* (reg8 *) IDAC8_4_viDAC8__CR1 )
#define IDAC8_4_Data        (* (reg8 *) IDAC8_4_viDAC8__D )
#define IDAC8_4_Data_PTR    (  (reg8 *) IDAC8_4_viDAC8__D )
#define IDAC8_4_Strobe      (* (reg8 *) IDAC8_4_viDAC8__STROBE )
#define IDAC8_4_SW0         (* (reg8 *) IDAC8_4_viDAC8__SW0 )
#define IDAC8_4_SW2         (* (reg8 *) IDAC8_4_viDAC8__SW2 )
#define IDAC8_4_SW3         (* (reg8 *) IDAC8_4_viDAC8__SW3 )
#define IDAC8_4_SW4         (* (reg8 *) IDAC8_4_viDAC8__SW4 )
#define IDAC8_4_TR          (* (reg8 *) IDAC8_4_viDAC8__TR )
#define IDAC8_4_PWRMGR      (* (reg8 *) IDAC8_4_viDAC8__PM_ACT_CFG )  /* Power manager */
#define IDAC8_4_STBY_PWRMGR (* (reg8 *) IDAC8_4_viDAC8__PM_STBY_CFG )  /* Standby Power manager */


/******************************************************************************
*              Trim    
*
* Note - VIDAC trim values are stored in the "Customer Table" area in 
* Row 1 of the Hidden Flash.  There are 8 bytes of trim data for each VIDAC 
* block.
* The values are:
*       I Gain offset, min range, Sourcing
*       I Gain offset, min range, Sinking
*       I Gain offset, med range, Sourcing
*       I Gain offset, med range, Sinking
*       I Gain offset, max range, Sourcing
*       I Gain offset, max range, Sinking
*       V Gain offset, 1V range
*       V Gain offset, 4V range
*
* The data set for the 4 VIDACs are arranged using a left side/right 
* side approach:
*  Left 0, Left 1, Right 0, Right 1.
* When mapped to the VIDAC0 thru VIDAC3 as:
*  VIDAC 0, VIDAC 2, VIDAC 1, VIDAC 3
******************************************************************************/

#define IDAC8_4_DAC_TRIM_BASE   IDAC8_4_viDAC8__TRIM__M1


/***************************************
*         Register Constants       
***************************************/

/* CR0 iDAC Control Register 0 definitions */  

/* Bit Field  DAC_HS_MODE                  */
#define IDAC8_4_HS_MASK        (0x02u)
#define IDAC8_4_HS_LOWPOWER    (0x00u)
#define IDAC8_4_HS_HIGHSPEED   (0x02u)

/* Bit Field  DAC_MODE                  */
#define IDAC8_4_MODE_MASK      (0x10u)
#define IDAC8_4_MODE_V         (0x00u)
#define IDAC8_4_MODE_I         (0x10u)

/* Bit Field  DAC_RANGE                  */
#define IDAC8_4_RANGE_MASK     (0x0Cu)
#define IDAC8_4_RANGE_0        (0x00u)
#define IDAC8_4_RANGE_1        (0x04u)
#define IDAC8_4_RANGE_2        (0x08u)
#define IDAC8_4_RANGE_3        (0x0Cu)

/* CR1 iDAC Control Register 1 definitions */

/* Bit Field  DAC_MX_DATA                  */
#define IDAC8_4_SRC_MASK       (0x20u)
#define IDAC8_4_SRC_REG        (0x00u)
#define IDAC8_4_SRC_UDB        (0x20u)

/* This bit enable reset from UDB array      */
#define IDAC8_4_RESET_MASK     (0x10u)
#define IDAC8_4_RESET_ENABLE   (0x10u)
#define IDAC8_4_RESET_DISABLE  (0x00u)

/* Bit Field  DAC_MX_IDIR_SRC              */
#define IDAC8_4_IDIR_SRC_MASK  (0x08u)
#define IDAC8_4_IDIR_SRC_REG   (0x00u)
#define IDAC8_4_IDIR_SRC_UDB   (0x08u)

/* Bit Field  DAC_I_DIR                  */
/* Register control of current direction      */
#define IDAC8_4_IDIR_MASK      (0x04u)
#define IDAC8_4_IDIR_SRC       (0x00u)
#define IDAC8_4_IDIR_SINK      (0x04u)

/* Bit Field  DAC_MX_IOFF_SRC                  */
/* Selects source of IOFF control, reg or UDB  */
#define IDAC8_4_IDIR_CTL_MASK  (0x02u)
#define IDAC8_4_IDIR_CTL_REG   (0x00u)
#define IDAC8_4_IDIR_CTL_UDB   (0x02u)

/* Bit Field  DAC_MX_IOFF                   */
/* Register control of IDAC                 */
/* Only valid if IOFF CTL is set to Reg     */
#define IDAC8_4_I_OFF_MASK     (0x01u)
#define IDAC8_4_I_OFF          (0x00u)
#define IDAC8_4_I_ON           (0x01u)

/* This bit enables data from DAC bus      */
#define IDAC8_4_DACBUS_MASK    (0x20u)
#define IDAC8_4_DACBUS_ENABLE  (0x20u)
#define IDAC8_4_DACBUS_DISABLE (0x00u)

/* DAC STROBE Strobe Control Register definitions */

/* Bit Field  DAC_MX_STROBE                  */
#define IDAC8_4_STRB_MASK      (0x08u)
#define IDAC8_4_STRB_EN        (0x08u)
#define IDAC8_4_STRB_DIS       (0x00u)

/* PM_ACT_CFG (Active Power Mode CFG Register)     */ 
#define IDAC8_4_ACT_PWR_EN   IDAC8_4_viDAC8__PM_ACT_MSK  /* Power enable mask */ 
#define IDAC8_4_STBY_PWR_EN  IDAC8_4_viDAC8__PM_STBY_MSK  /* Standby Power enable mask */ 

#endif /* CY_IDAC8_IDAC8_4_H */


/* [] END OF FILE */
