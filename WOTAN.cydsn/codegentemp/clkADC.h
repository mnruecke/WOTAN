/*******************************************************************************
* File Name: clkADC.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_clkADC_H)
#define CY_CLOCK_clkADC_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void clkADC_Start(void) ;
void clkADC_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void clkADC_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void clkADC_StandbyPower(uint8 state) ;
void clkADC_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 clkADC_GetDividerRegister(void) ;
void clkADC_SetModeRegister(uint8 modeBitMask) ;
void clkADC_ClearModeRegister(uint8 modeBitMask) ;
uint8 clkADC_GetModeRegister(void) ;
void clkADC_SetSourceRegister(uint8 clkSource) ;
uint8 clkADC_GetSourceRegister(void) ;
#if defined(clkADC__CFG3)
void clkADC_SetPhaseRegister(uint8 clkPhase) ;
uint8 clkADC_GetPhaseRegister(void) ;
#endif /* defined(clkADC__CFG3) */

#define clkADC_Enable()                       clkADC_Start()
#define clkADC_Disable()                      clkADC_Stop()
#define clkADC_SetDivider(clkDivider)         clkADC_SetDividerRegister(clkDivider, 1u)
#define clkADC_SetDividerValue(clkDivider)    clkADC_SetDividerRegister((clkDivider) - 1u, 1u)
#define clkADC_SetMode(clkMode)               clkADC_SetModeRegister(clkMode)
#define clkADC_SetSource(clkSource)           clkADC_SetSourceRegister(clkSource)
#if defined(clkADC__CFG3)
#define clkADC_SetPhase(clkPhase)             clkADC_SetPhaseRegister(clkPhase)
#define clkADC_SetPhaseValue(clkPhase)        clkADC_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(clkADC__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define clkADC_CLKEN              (* (reg8 *) clkADC__PM_ACT_CFG)
#define clkADC_CLKEN_PTR          ((reg8 *) clkADC__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define clkADC_CLKSTBY            (* (reg8 *) clkADC__PM_STBY_CFG)
#define clkADC_CLKSTBY_PTR        ((reg8 *) clkADC__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define clkADC_DIV_LSB            (* (reg8 *) clkADC__CFG0)
#define clkADC_DIV_LSB_PTR        ((reg8 *) clkADC__CFG0)
#define clkADC_DIV_PTR            ((reg16 *) clkADC__CFG0)

/* Clock MSB divider configuration register. */
#define clkADC_DIV_MSB            (* (reg8 *) clkADC__CFG1)
#define clkADC_DIV_MSB_PTR        ((reg8 *) clkADC__CFG1)

/* Mode and source configuration register */
#define clkADC_MOD_SRC            (* (reg8 *) clkADC__CFG2)
#define clkADC_MOD_SRC_PTR        ((reg8 *) clkADC__CFG2)

#if defined(clkADC__CFG3)
/* Analog clock phase configuration register */
#define clkADC_PHASE              (* (reg8 *) clkADC__CFG3)
#define clkADC_PHASE_PTR          ((reg8 *) clkADC__CFG3)
#endif /* defined(clkADC__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define clkADC_CLKEN_MASK         clkADC__PM_ACT_MSK
#define clkADC_CLKSTBY_MASK       clkADC__PM_STBY_MSK

/* CFG2 field masks */
#define clkADC_SRC_SEL_MSK        clkADC__CFG2_SRC_SEL_MASK
#define clkADC_MODE_MASK          (~(clkADC_SRC_SEL_MSK))

#if defined(clkADC__CFG3)
/* CFG3 phase mask */
#define clkADC_PHASE_MASK         clkADC__CFG3_PHASE_DLY_MASK
#endif /* defined(clkADC__CFG3) */

#endif /* CY_CLOCK_clkADC_H */


/* [] END OF FILE */
