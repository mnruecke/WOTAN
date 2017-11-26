/*******************************************************************************
* File Name: clkADDA.h
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

#if !defined(CY_CLOCK_clkADDA_H)
#define CY_CLOCK_clkADDA_H

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

void clkADDA_Start(void) ;
void clkADDA_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void clkADDA_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void clkADDA_StandbyPower(uint8 state) ;
void clkADDA_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 clkADDA_GetDividerRegister(void) ;
void clkADDA_SetModeRegister(uint8 modeBitMask) ;
void clkADDA_ClearModeRegister(uint8 modeBitMask) ;
uint8 clkADDA_GetModeRegister(void) ;
void clkADDA_SetSourceRegister(uint8 clkSource) ;
uint8 clkADDA_GetSourceRegister(void) ;
#if defined(clkADDA__CFG3)
void clkADDA_SetPhaseRegister(uint8 clkPhase) ;
uint8 clkADDA_GetPhaseRegister(void) ;
#endif /* defined(clkADDA__CFG3) */

#define clkADDA_Enable()                       clkADDA_Start()
#define clkADDA_Disable()                      clkADDA_Stop()
#define clkADDA_SetDivider(clkDivider)         clkADDA_SetDividerRegister(clkDivider, 1u)
#define clkADDA_SetDividerValue(clkDivider)    clkADDA_SetDividerRegister((clkDivider) - 1u, 1u)
#define clkADDA_SetMode(clkMode)               clkADDA_SetModeRegister(clkMode)
#define clkADDA_SetSource(clkSource)           clkADDA_SetSourceRegister(clkSource)
#if defined(clkADDA__CFG3)
#define clkADDA_SetPhase(clkPhase)             clkADDA_SetPhaseRegister(clkPhase)
#define clkADDA_SetPhaseValue(clkPhase)        clkADDA_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(clkADDA__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define clkADDA_CLKEN              (* (reg8 *) clkADDA__PM_ACT_CFG)
#define clkADDA_CLKEN_PTR          ((reg8 *) clkADDA__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define clkADDA_CLKSTBY            (* (reg8 *) clkADDA__PM_STBY_CFG)
#define clkADDA_CLKSTBY_PTR        ((reg8 *) clkADDA__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define clkADDA_DIV_LSB            (* (reg8 *) clkADDA__CFG0)
#define clkADDA_DIV_LSB_PTR        ((reg8 *) clkADDA__CFG0)
#define clkADDA_DIV_PTR            ((reg16 *) clkADDA__CFG0)

/* Clock MSB divider configuration register. */
#define clkADDA_DIV_MSB            (* (reg8 *) clkADDA__CFG1)
#define clkADDA_DIV_MSB_PTR        ((reg8 *) clkADDA__CFG1)

/* Mode and source configuration register */
#define clkADDA_MOD_SRC            (* (reg8 *) clkADDA__CFG2)
#define clkADDA_MOD_SRC_PTR        ((reg8 *) clkADDA__CFG2)

#if defined(clkADDA__CFG3)
/* Analog clock phase configuration register */
#define clkADDA_PHASE              (* (reg8 *) clkADDA__CFG3)
#define clkADDA_PHASE_PTR          ((reg8 *) clkADDA__CFG3)
#endif /* defined(clkADDA__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define clkADDA_CLKEN_MASK         clkADDA__PM_ACT_MSK
#define clkADDA_CLKSTBY_MASK       clkADDA__PM_STBY_MSK

/* CFG2 field masks */
#define clkADDA_SRC_SEL_MSK        clkADDA__CFG2_SRC_SEL_MASK
#define clkADDA_MODE_MASK          (~(clkADDA_SRC_SEL_MSK))

#if defined(clkADDA__CFG3)
/* CFG3 phase mask */
#define clkADDA_PHASE_MASK         clkADDA__CFG3_PHASE_DLY_MASK
#endif /* defined(clkADDA__CFG3) */

#endif /* CY_CLOCK_clkADDA_H */


/* [] END OF FILE */
