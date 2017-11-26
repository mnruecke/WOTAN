/*******************************************************************************
* File Name: clkSAMPLING.h
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

#if !defined(CY_CLOCK_clkSAMPLING_H)
#define CY_CLOCK_clkSAMPLING_H

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

void clkSAMPLING_Start(void) ;
void clkSAMPLING_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void clkSAMPLING_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void clkSAMPLING_StandbyPower(uint8 state) ;
void clkSAMPLING_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 clkSAMPLING_GetDividerRegister(void) ;
void clkSAMPLING_SetModeRegister(uint8 modeBitMask) ;
void clkSAMPLING_ClearModeRegister(uint8 modeBitMask) ;
uint8 clkSAMPLING_GetModeRegister(void) ;
void clkSAMPLING_SetSourceRegister(uint8 clkSource) ;
uint8 clkSAMPLING_GetSourceRegister(void) ;
#if defined(clkSAMPLING__CFG3)
void clkSAMPLING_SetPhaseRegister(uint8 clkPhase) ;
uint8 clkSAMPLING_GetPhaseRegister(void) ;
#endif /* defined(clkSAMPLING__CFG3) */

#define clkSAMPLING_Enable()                       clkSAMPLING_Start()
#define clkSAMPLING_Disable()                      clkSAMPLING_Stop()
#define clkSAMPLING_SetDivider(clkDivider)         clkSAMPLING_SetDividerRegister(clkDivider, 1u)
#define clkSAMPLING_SetDividerValue(clkDivider)    clkSAMPLING_SetDividerRegister((clkDivider) - 1u, 1u)
#define clkSAMPLING_SetMode(clkMode)               clkSAMPLING_SetModeRegister(clkMode)
#define clkSAMPLING_SetSource(clkSource)           clkSAMPLING_SetSourceRegister(clkSource)
#if defined(clkSAMPLING__CFG3)
#define clkSAMPLING_SetPhase(clkPhase)             clkSAMPLING_SetPhaseRegister(clkPhase)
#define clkSAMPLING_SetPhaseValue(clkPhase)        clkSAMPLING_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(clkSAMPLING__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define clkSAMPLING_CLKEN              (* (reg8 *) clkSAMPLING__PM_ACT_CFG)
#define clkSAMPLING_CLKEN_PTR          ((reg8 *) clkSAMPLING__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define clkSAMPLING_CLKSTBY            (* (reg8 *) clkSAMPLING__PM_STBY_CFG)
#define clkSAMPLING_CLKSTBY_PTR        ((reg8 *) clkSAMPLING__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define clkSAMPLING_DIV_LSB            (* (reg8 *) clkSAMPLING__CFG0)
#define clkSAMPLING_DIV_LSB_PTR        ((reg8 *) clkSAMPLING__CFG0)
#define clkSAMPLING_DIV_PTR            ((reg16 *) clkSAMPLING__CFG0)

/* Clock MSB divider configuration register. */
#define clkSAMPLING_DIV_MSB            (* (reg8 *) clkSAMPLING__CFG1)
#define clkSAMPLING_DIV_MSB_PTR        ((reg8 *) clkSAMPLING__CFG1)

/* Mode and source configuration register */
#define clkSAMPLING_MOD_SRC            (* (reg8 *) clkSAMPLING__CFG2)
#define clkSAMPLING_MOD_SRC_PTR        ((reg8 *) clkSAMPLING__CFG2)

#if defined(clkSAMPLING__CFG3)
/* Analog clock phase configuration register */
#define clkSAMPLING_PHASE              (* (reg8 *) clkSAMPLING__CFG3)
#define clkSAMPLING_PHASE_PTR          ((reg8 *) clkSAMPLING__CFG3)
#endif /* defined(clkSAMPLING__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define clkSAMPLING_CLKEN_MASK         clkSAMPLING__PM_ACT_MSK
#define clkSAMPLING_CLKSTBY_MASK       clkSAMPLING__PM_STBY_MSK

/* CFG2 field masks */
#define clkSAMPLING_SRC_SEL_MSK        clkSAMPLING__CFG2_SRC_SEL_MASK
#define clkSAMPLING_MODE_MASK          (~(clkSAMPLING_SRC_SEL_MSK))

#if defined(clkSAMPLING__CFG3)
/* CFG3 phase mask */
#define clkSAMPLING_PHASE_MASK         clkSAMPLING__CFG3_PHASE_DLY_MASK
#endif /* defined(clkSAMPLING__CFG3) */

#endif /* CY_CLOCK_clkSAMPLING_H */


/* [] END OF FILE */
