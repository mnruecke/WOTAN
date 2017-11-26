/*******************************************************************************
* File Name: Sig_inP.h  
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

#if !defined(CY_PINS_Sig_inP_H) /* Pins Sig_inP_H */
#define CY_PINS_Sig_inP_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Sig_inP_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Sig_inP__PORT == 15 && ((Sig_inP__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Sig_inP_Write(uint8 value);
void    Sig_inP_SetDriveMode(uint8 mode);
uint8   Sig_inP_ReadDataReg(void);
uint8   Sig_inP_Read(void);
void    Sig_inP_SetInterruptMode(uint16 position, uint16 mode);
uint8   Sig_inP_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Sig_inP_SetDriveMode() function.
     *  @{
     */
        #define Sig_inP_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Sig_inP_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Sig_inP_DM_RES_UP          PIN_DM_RES_UP
        #define Sig_inP_DM_RES_DWN         PIN_DM_RES_DWN
        #define Sig_inP_DM_OD_LO           PIN_DM_OD_LO
        #define Sig_inP_DM_OD_HI           PIN_DM_OD_HI
        #define Sig_inP_DM_STRONG          PIN_DM_STRONG
        #define Sig_inP_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Sig_inP_MASK               Sig_inP__MASK
#define Sig_inP_SHIFT              Sig_inP__SHIFT
#define Sig_inP_WIDTH              1u

/* Interrupt constants */
#if defined(Sig_inP__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Sig_inP_SetInterruptMode() function.
     *  @{
     */
        #define Sig_inP_INTR_NONE      (uint16)(0x0000u)
        #define Sig_inP_INTR_RISING    (uint16)(0x0001u)
        #define Sig_inP_INTR_FALLING   (uint16)(0x0002u)
        #define Sig_inP_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Sig_inP_INTR_MASK      (0x01u) 
#endif /* (Sig_inP__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Sig_inP_PS                     (* (reg8 *) Sig_inP__PS)
/* Data Register */
#define Sig_inP_DR                     (* (reg8 *) Sig_inP__DR)
/* Port Number */
#define Sig_inP_PRT_NUM                (* (reg8 *) Sig_inP__PRT) 
/* Connect to Analog Globals */                                                  
#define Sig_inP_AG                     (* (reg8 *) Sig_inP__AG)                       
/* Analog MUX bux enable */
#define Sig_inP_AMUX                   (* (reg8 *) Sig_inP__AMUX) 
/* Bidirectional Enable */                                                        
#define Sig_inP_BIE                    (* (reg8 *) Sig_inP__BIE)
/* Bit-mask for Aliased Register Access */
#define Sig_inP_BIT_MASK               (* (reg8 *) Sig_inP__BIT_MASK)
/* Bypass Enable */
#define Sig_inP_BYP                    (* (reg8 *) Sig_inP__BYP)
/* Port wide control signals */                                                   
#define Sig_inP_CTL                    (* (reg8 *) Sig_inP__CTL)
/* Drive Modes */
#define Sig_inP_DM0                    (* (reg8 *) Sig_inP__DM0) 
#define Sig_inP_DM1                    (* (reg8 *) Sig_inP__DM1)
#define Sig_inP_DM2                    (* (reg8 *) Sig_inP__DM2) 
/* Input Buffer Disable Override */
#define Sig_inP_INP_DIS                (* (reg8 *) Sig_inP__INP_DIS)
/* LCD Common or Segment Drive */
#define Sig_inP_LCD_COM_SEG            (* (reg8 *) Sig_inP__LCD_COM_SEG)
/* Enable Segment LCD */
#define Sig_inP_LCD_EN                 (* (reg8 *) Sig_inP__LCD_EN)
/* Slew Rate Control */
#define Sig_inP_SLW                    (* (reg8 *) Sig_inP__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Sig_inP_PRTDSI__CAPS_SEL       (* (reg8 *) Sig_inP__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Sig_inP_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Sig_inP__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Sig_inP_PRTDSI__OE_SEL0        (* (reg8 *) Sig_inP__PRTDSI__OE_SEL0) 
#define Sig_inP_PRTDSI__OE_SEL1        (* (reg8 *) Sig_inP__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Sig_inP_PRTDSI__OUT_SEL0       (* (reg8 *) Sig_inP__PRTDSI__OUT_SEL0) 
#define Sig_inP_PRTDSI__OUT_SEL1       (* (reg8 *) Sig_inP__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Sig_inP_PRTDSI__SYNC_OUT       (* (reg8 *) Sig_inP__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Sig_inP__SIO_CFG)
    #define Sig_inP_SIO_HYST_EN        (* (reg8 *) Sig_inP__SIO_HYST_EN)
    #define Sig_inP_SIO_REG_HIFREQ     (* (reg8 *) Sig_inP__SIO_REG_HIFREQ)
    #define Sig_inP_SIO_CFG            (* (reg8 *) Sig_inP__SIO_CFG)
    #define Sig_inP_SIO_DIFF           (* (reg8 *) Sig_inP__SIO_DIFF)
#endif /* (Sig_inP__SIO_CFG) */

/* Interrupt Registers */
#if defined(Sig_inP__INTSTAT)
    #define Sig_inP_INTSTAT            (* (reg8 *) Sig_inP__INTSTAT)
    #define Sig_inP_SNAP               (* (reg8 *) Sig_inP__SNAP)
    
	#define Sig_inP_0_INTTYPE_REG 		(* (reg8 *) Sig_inP__0__INTTYPE)
#endif /* (Sig_inP__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Sig_inP_H */


/* [] END OF FILE */
