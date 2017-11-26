/*******************************************************************************
* File Name: opamp.c
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

#include "opamp.h"

uint8 opamp_initVar = 0u;


/*******************************************************************************   
* Function Name: opamp_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the 
*  customizer of the component placed onto schematic. Usually called in 
*  opamp_Start().
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void opamp_Init(void) 
{
    opamp_SetPower(opamp_DEFAULT_POWER);
}


/*******************************************************************************   
* Function Name: opamp_Enable
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
void opamp_Enable(void) 
{
    /* Enable negative charge pumps in ANIF */
    opamp_PUMP_CR1_REG  |= (opamp_PUMP_CR1_CLKSEL | opamp_PUMP_CR1_FORCE);

    /* Enable power to buffer in active mode */
    opamp_PM_ACT_CFG_REG |= opamp_ACT_PWR_EN;

    /* Enable power to buffer in alternative active mode */
    opamp_PM_STBY_CFG_REG |= opamp_STBY_PWR_EN;
}


/*******************************************************************************
* Function Name:   opamp_Start
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
*  opamp_initVar: Used to check the initial configuration, modified 
*  when this function is called for the first time.
*
*******************************************************************************/
void opamp_Start(void) 
{
    if(opamp_initVar == 0u)
    {
        opamp_initVar = 1u;
        opamp_Init();
    }

    opamp_Enable();
}


/*******************************************************************************
* Function Name: opamp_Stop
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
void opamp_Stop(void) 
{
    /* Disable power to buffer in active mode template */
    opamp_PM_ACT_CFG_REG &= (uint8)(~opamp_ACT_PWR_EN);

    /* Disable power to buffer in alternative active mode template */
    opamp_PM_STBY_CFG_REG &= (uint8)(~opamp_STBY_PWR_EN);
    
    /* Disable negative charge pumps for ANIF only if all ABuf is turned OFF */
    if(opamp_PM_ACT_CFG_REG == 0u)
    {
        opamp_PUMP_CR1_REG &= (uint8)(~(opamp_PUMP_CR1_CLKSEL | opamp_PUMP_CR1_FORCE));
    }
}


/*******************************************************************************
* Function Name: opamp_SetPower
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
void opamp_SetPower(uint8 power) 
{
    #if (CY_PSOC3 || CY_PSOC5LP)
        opamp_CR_REG &= (uint8)(~opamp_PWR_MASK);
        opamp_CR_REG |= power & opamp_PWR_MASK;      /* Set device power */
    #else
        CYASSERT(opamp_HIGHPOWER == power);
    #endif /* CY_PSOC3 || CY_PSOC5LP */
}


/* [] END OF FILE */
