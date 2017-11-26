/*******************************************************************************
* File Name: triggerIn.h  
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

#if !defined(CY_PINS_triggerIn_H) /* Pins triggerIn_H */
#define CY_PINS_triggerIn_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "triggerIn_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 triggerIn__PORT == 15 && ((triggerIn__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    triggerIn_Write(uint8 value);
void    triggerIn_SetDriveMode(uint8 mode);
uint8   triggerIn_ReadDataReg(void);
uint8   triggerIn_Read(void);
void    triggerIn_SetInterruptMode(uint16 position, uint16 mode);
uint8   triggerIn_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the triggerIn_SetDriveMode() function.
     *  @{
     */
        #define triggerIn_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define triggerIn_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define triggerIn_DM_RES_UP          PIN_DM_RES_UP
        #define triggerIn_DM_RES_DWN         PIN_DM_RES_DWN
        #define triggerIn_DM_OD_LO           PIN_DM_OD_LO
        #define triggerIn_DM_OD_HI           PIN_DM_OD_HI
        #define triggerIn_DM_STRONG          PIN_DM_STRONG
        #define triggerIn_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define triggerIn_MASK               triggerIn__MASK
#define triggerIn_SHIFT              triggerIn__SHIFT
#define triggerIn_WIDTH              1u

/* Interrupt constants */
#if defined(triggerIn__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in triggerIn_SetInterruptMode() function.
     *  @{
     */
        #define triggerIn_INTR_NONE      (uint16)(0x0000u)
        #define triggerIn_INTR_RISING    (uint16)(0x0001u)
        #define triggerIn_INTR_FALLING   (uint16)(0x0002u)
        #define triggerIn_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define triggerIn_INTR_MASK      (0x01u) 
#endif /* (triggerIn__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define triggerIn_PS                     (* (reg8 *) triggerIn__PS)
/* Data Register */
#define triggerIn_DR                     (* (reg8 *) triggerIn__DR)
/* Port Number */
#define triggerIn_PRT_NUM                (* (reg8 *) triggerIn__PRT) 
/* Connect to Analog Globals */                                                  
#define triggerIn_AG                     (* (reg8 *) triggerIn__AG)                       
/* Analog MUX bux enable */
#define triggerIn_AMUX                   (* (reg8 *) triggerIn__AMUX) 
/* Bidirectional Enable */                                                        
#define triggerIn_BIE                    (* (reg8 *) triggerIn__BIE)
/* Bit-mask for Aliased Register Access */
#define triggerIn_BIT_MASK               (* (reg8 *) triggerIn__BIT_MASK)
/* Bypass Enable */
#define triggerIn_BYP                    (* (reg8 *) triggerIn__BYP)
/* Port wide control signals */                                                   
#define triggerIn_CTL                    (* (reg8 *) triggerIn__CTL)
/* Drive Modes */
#define triggerIn_DM0                    (* (reg8 *) triggerIn__DM0) 
#define triggerIn_DM1                    (* (reg8 *) triggerIn__DM1)
#define triggerIn_DM2                    (* (reg8 *) triggerIn__DM2) 
/* Input Buffer Disable Override */
#define triggerIn_INP_DIS                (* (reg8 *) triggerIn__INP_DIS)
/* LCD Common or Segment Drive */
#define triggerIn_LCD_COM_SEG            (* (reg8 *) triggerIn__LCD_COM_SEG)
/* Enable Segment LCD */
#define triggerIn_LCD_EN                 (* (reg8 *) triggerIn__LCD_EN)
/* Slew Rate Control */
#define triggerIn_SLW                    (* (reg8 *) triggerIn__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define triggerIn_PRTDSI__CAPS_SEL       (* (reg8 *) triggerIn__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define triggerIn_PRTDSI__DBL_SYNC_IN    (* (reg8 *) triggerIn__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define triggerIn_PRTDSI__OE_SEL0        (* (reg8 *) triggerIn__PRTDSI__OE_SEL0) 
#define triggerIn_PRTDSI__OE_SEL1        (* (reg8 *) triggerIn__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define triggerIn_PRTDSI__OUT_SEL0       (* (reg8 *) triggerIn__PRTDSI__OUT_SEL0) 
#define triggerIn_PRTDSI__OUT_SEL1       (* (reg8 *) triggerIn__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define triggerIn_PRTDSI__SYNC_OUT       (* (reg8 *) triggerIn__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(triggerIn__SIO_CFG)
    #define triggerIn_SIO_HYST_EN        (* (reg8 *) triggerIn__SIO_HYST_EN)
    #define triggerIn_SIO_REG_HIFREQ     (* (reg8 *) triggerIn__SIO_REG_HIFREQ)
    #define triggerIn_SIO_CFG            (* (reg8 *) triggerIn__SIO_CFG)
    #define triggerIn_SIO_DIFF           (* (reg8 *) triggerIn__SIO_DIFF)
#endif /* (triggerIn__SIO_CFG) */

/* Interrupt Registers */
#if defined(triggerIn__INTSTAT)
    #define triggerIn_INTSTAT            (* (reg8 *) triggerIn__INTSTAT)
    #define triggerIn_SNAP               (* (reg8 *) triggerIn__SNAP)
    
	#define triggerIn_0_INTTYPE_REG 		(* (reg8 *) triggerIn__0__INTTYPE)
#endif /* (triggerIn__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_triggerIn_H */


/* [] END OF FILE */
