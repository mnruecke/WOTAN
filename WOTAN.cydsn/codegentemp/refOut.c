/*******************************************************************************
* File Name: refOut.c
* Version 1.90
*
* Description:
*  This file provides the source code to the API for OpAmp (Analog Buffer) 
*  Component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "refOut.h"

uint8 refOut_initVar = 0u;


/*******************************************************************************   
* Function Name: refOut_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the 
*  customizer of the component placed onto schematic. Usually called in 
*  refOut_Start().
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void refOut_Init(void) 
{
    refOut_SetPower(refOut_DEFAULT_POWER);
}


/*******************************************************************************   
* Function Name: refOut_Enable
********************************************************************************
*
* Summary:
*  Enables the OpAmp block operation
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void refOut_Enable(void) 
{
    /* Enable negative charge pumps in ANIF */
    refOut_PUMP_CR1_REG  |= (refOut_PUMP_CR1_CLKSEL | refOut_PUMP_CR1_FORCE);

    /* Enable power to buffer in active mode */
    refOut_PM_ACT_CFG_REG |= refOut_ACT_PWR_EN;

    /* Enable power to buffer in alternative active mode */
    refOut_PM_STBY_CFG_REG |= refOut_STBY_PWR_EN;
}


/*******************************************************************************
* Function Name:   refOut_Start
********************************************************************************
*
* Summary:
*  The start function initializes the Analog Buffer with the default values and 
*  sets the power to the given level. A power level of 0, is same as 
*  executing the stop function.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  refOut_initVar: Used to check the initial configuration, modified 
*  when this function is called for the first time.
*
*******************************************************************************/
void refOut_Start(void) 
{
    if(refOut_initVar == 0u)
    {
        refOut_initVar = 1u;
        refOut_Init();
    }

    refOut_Enable();
}


/*******************************************************************************
* Function Name: refOut_Stop
********************************************************************************
*
* Summary:
*  Powers down amplifier to lowest power state.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void refOut_Stop(void) 
{
    /* Disable power to buffer in active mode template */
    refOut_PM_ACT_CFG_REG &= (uint8)(~refOut_ACT_PWR_EN);

    /* Disable power to buffer in alternative active mode template */
    refOut_PM_STBY_CFG_REG &= (uint8)(~refOut_STBY_PWR_EN);
    
    /* Disable negative charge pumps for ANIF only if all ABuf is turned OFF */
    if(refOut_PM_ACT_CFG_REG == 0u)
    {
        refOut_PUMP_CR1_REG &= (uint8)(~(refOut_PUMP_CR1_CLKSEL | refOut_PUMP_CR1_FORCE));
    }
}


/*******************************************************************************
* Function Name: refOut_SetPower
********************************************************************************
*
* Summary:
*  Sets power level of Analog buffer.
*
* Parameters: 
*  power: PSoC3: Sets power level between low (1) and high power (3).
*         PSoC5: Sets power level High (0)
*
* Return:
*  void
*
**********************************************************************************/
void refOut_SetPower(uint8 power) 
{
    #if (CY_PSOC3 || CY_PSOC5LP)
        refOut_CR_REG &= (uint8)(~refOut_PWR_MASK);
        refOut_CR_REG |= power & refOut_PWR_MASK;      /* Set device power */
    #else
        CYASSERT(refOut_HIGHPOWER == power);
    #endif /* CY_PSOC3 || CY_PSOC5LP */
}


/* [] END OF FILE */
