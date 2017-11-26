/*******************************************************************************
* File Name: Sig_inN.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Sig_inN_H) /* Pins Sig_inN_H */
#define CY_PINS_Sig_inN_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Sig_inN_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Sig_inN__PORT == 15 && ((Sig_inN__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Sig_inN_Write(uint8 value);
void    Sig_inN_SetDriveMode(uint8 mode);
uint8   Sig_inN_ReadDataReg(void);
uint8   Sig_inN_Read(void);
void    Sig_inN_SetInterruptMode(uint16 position, uint16 mode);
uint8   Sig_inN_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Sig_inN_SetDriveMode() function.
     *  @{
     */
        #define Sig_inN_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Sig_inN_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Sig_inN_DM_RES_UP          PIN_DM_RES_UP
        #define Sig_inN_DM_RES_DWN         PIN_DM_RES_DWN
        #define Sig_inN_DM_OD_LO           PIN_DM_OD_LO
        #define Sig_inN_DM_OD_HI           PIN_DM_OD_HI
        #define Sig_inN_DM_STRONG          PIN_DM_STRONG
        #define Sig_inN_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Sig_inN_MASK               Sig_inN__MASK
#define Sig_inN_SHIFT              Sig_inN__SHIFT
#define Sig_inN_WIDTH              1u

/* Interrupt constants */
#if defined(Sig_inN__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Sig_inN_SetInterruptMode() function.
     *  @{
     */
        #define Sig_inN_INTR_NONE      (uint16)(0x0000u)
        #define Sig_inN_INTR_RISING    (uint16)(0x0001u)
        #define Sig_inN_INTR_FALLING   (uint16)(0x0002u)
        #define Sig_inN_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Sig_inN_INTR_MASK      (0x01u) 
#endif /* (Sig_inN__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Sig_inN_PS                     (* (reg8 *) Sig_inN__PS)
/* Data Register */
#define Sig_inN_DR                     (* (reg8 *) Sig_inN__DR)
/* Port Number */
#define Sig_inN_PRT_NUM                (* (reg8 *) Sig_inN__PRT) 
/* Connect to Analog Globals */                                                  
#define Sig_inN_AG                     (* (reg8 *) Sig_inN__AG)                       
/* Analog MUX bux enable */
#define Sig_inN_AMUX                   (* (reg8 *) Sig_inN__AMUX) 
/* Bidirectional Enable */                                                        
#define Sig_inN_BIE                    (* (reg8 *) Sig_inN__BIE)
/* Bit-mask for Aliased Register Access */
#define Sig_inN_BIT_MASK               (* (reg8 *) Sig_inN__BIT_MASK)
/* Bypass Enable */
#define Sig_inN_BYP                    (* (reg8 *) Sig_inN__BYP)
/* Port wide control signals */                                                   
#define Sig_inN_CTL                    (* (reg8 *) Sig_inN__CTL)
/* Drive Modes */
#define Sig_inN_DM0                    (* (reg8 *) Sig_inN__DM0) 
#define Sig_inN_DM1                    (* (reg8 *) Sig_inN__DM1)
#define Sig_inN_DM2                    (* (reg8 *) Sig_inN__DM2) 
/* Input Buffer Disable Override */
#define Sig_inN_INP_DIS                (* (reg8 *) Sig_inN__INP_DIS)
/* LCD Common or Segment Drive */
#define Sig_inN_LCD_COM_SEG            (* (reg8 *) Sig_inN__LCD_COM_SEG)
/* Enable Segment LCD */
#define Sig_inN_LCD_EN                 (* (reg8 *) Sig_inN__LCD_EN)
/* Slew Rate Control */
#define Sig_inN_SLW                    (* (reg8 *) Sig_inN__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Sig_inN_PRTDSI__CAPS_SEL       (* (reg8 *) Sig_inN__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Sig_inN_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Sig_inN__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Sig_inN_PRTDSI__OE_SEL0        (* (reg8 *) Sig_inN__PRTDSI__OE_SEL0) 
#define Sig_inN_PRTDSI__OE_SEL1        (* (reg8 *) Sig_inN__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Sig_inN_PRTDSI__OUT_SEL0       (* (reg8 *) Sig_inN__PRTDSI__OUT_SEL0) 
#define Sig_inN_PRTDSI__OUT_SEL1       (* (reg8 *) Sig_inN__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Sig_inN_PRTDSI__SYNC_OUT       (* (reg8 *) Sig_inN__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Sig_inN__SIO_CFG)
    #define Sig_inN_SIO_HYST_EN        (* (reg8 *) Sig_inN__SIO_HYST_EN)
    #define Sig_inN_SIO_REG_HIFREQ     (* (reg8 *) Sig_inN__SIO_REG_HIFREQ)
    #define Sig_inN_SIO_CFG            (* (reg8 *) Sig_inN__SIO_CFG)
    #define Sig_inN_SIO_DIFF           (* (reg8 *) Sig_inN__SIO_DIFF)
#endif /* (Sig_inN__SIO_CFG) */

/* Interrupt Registers */
#if defined(Sig_inN__INTSTAT)
    #define Sig_inN_INTSTAT            (* (reg8 *) Sig_inN__INTSTAT)
    #define Sig_inN_SNAP               (* (reg8 *) Sig_inN__SNAP)
    
	#define Sig_inN_0_INTTYPE_REG 		(* (reg8 *) Sig_inN__0__INTTYPE)
#endif /* (Sig_inN__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Sig_inN_H */


/* [] END OF FILE */
