/*******************************************************************************
* File Name: CalibClk.h
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

#if !defined(CY_CLOCK_CalibClk_H)
#define CY_CLOCK_CalibClk_H

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

void CalibClk_Start(void) ;
void CalibClk_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void CalibClk_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void CalibClk_StandbyPower(uint8 state) ;
void CalibClk_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 CalibClk_GetDividerRegister(void) ;
void CalibClk_SetModeRegister(uint8 modeBitMask) ;
void CalibClk_ClearModeRegister(uint8 modeBitMask) ;
uint8 CalibClk_GetModeRegister(void) ;
void CalibClk_SetSourceRegister(uint8 clkSource) ;
uint8 CalibClk_GetSourceRegister(void) ;
#if defined(CalibClk__CFG3)
void CalibClk_SetPhaseRegister(uint8 clkPhase) ;
uint8 CalibClk_GetPhaseRegister(void) ;
#endif /* defined(CalibClk__CFG3) */

#define CalibClk_Enable()                       CalibClk_Start()
#define CalibClk_Disable()                      CalibClk_Stop()
#define CalibClk_SetDivider(clkDivider)         CalibClk_SetDividerRegister(clkDivider, 1u)
#define CalibClk_SetDividerValue(clkDivider)    CalibClk_SetDividerRegister((clkDivider) - 1u, 1u)
#define CalibClk_SetMode(clkMode)               CalibClk_SetModeRegister(clkMode)
#define CalibClk_SetSource(clkSource)           CalibClk_SetSourceRegister(clkSource)
#if defined(CalibClk__CFG3)
#define CalibClk_SetPhase(clkPhase)             CalibClk_SetPhaseRegister(clkPhase)
#define CalibClk_SetPhaseValue(clkPhase)        CalibClk_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(CalibClk__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define CalibClk_CLKEN              (* (reg8 *) CalibClk__PM_ACT_CFG)
#define CalibClk_CLKEN_PTR          ((reg8 *) CalibClk__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define CalibClk_CLKSTBY            (* (reg8 *) CalibClk__PM_STBY_CFG)
#define CalibClk_CLKSTBY_PTR        ((reg8 *) CalibClk__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define CalibClk_DIV_LSB            (* (reg8 *) CalibClk__CFG0)
#define CalibClk_DIV_LSB_PTR        ((reg8 *) CalibClk__CFG0)
#define CalibClk_DIV_PTR            ((reg16 *) CalibClk__CFG0)

/* Clock MSB divider configuration register. */
#define CalibClk_DIV_MSB            (* (reg8 *) CalibClk__CFG1)
#define CalibClk_DIV_MSB_PTR        ((reg8 *) CalibClk__CFG1)

/* Mode and source configuration register */
#define CalibClk_MOD_SRC            (* (reg8 *) CalibClk__CFG2)
#define CalibClk_MOD_SRC_PTR        ((reg8 *) CalibClk__CFG2)

#if defined(CalibClk__CFG3)
/* Analog clock phase configuration register */
#define CalibClk_PHASE              (* (reg8 *) CalibClk__CFG3)
#define CalibClk_PHASE_PTR          ((reg8 *) CalibClk__CFG3)
#endif /* defined(CalibClk__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define CalibClk_CLKEN_MASK         CalibClk__PM_ACT_MSK
#define CalibClk_CLKSTBY_MASK       CalibClk__PM_STBY_MSK

/* CFG2 field masks */
#define CalibClk_SRC_SEL_MSK        CalibClk__CFG2_SRC_SEL_MASK
#define CalibClk_MODE_MASK          (~(CalibClk_SRC_SEL_MSK))

#if defined(CalibClk__CFG3)
/* CFG3 phase mask */
#define CalibClk_PHASE_MASK         CalibClk__CFG3_PHASE_DLY_MASK
#endif /* defined(CalibClk__CFG3) */

#endif /* CY_CLOCK_CalibClk_H */


/* [] END OF FILE */
