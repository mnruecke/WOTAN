/*******************************************************************************
* File Name: sigBuf.c
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

#include "sigBuf.h"

uint8 sigBuf_initVar = 0u;


/*******************************************************************************   
* Function Name: sigBuf_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the 
*  customizer of the component placed onto schematic. Usually called in 
*  sigBuf_Start().
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void sigBuf_Init(void) 
{
    sigBuf_SetPower(sigBuf_DEFAULT_POWER);
}


/*******************************************************************************   
* Function Name: sigBuf_Enable
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
void sigBuf_Enable(void) 
{
    /* Enable negative charge pumps in ANIF */
    sigBuf_PUMP_CR1_REG  |= (sigBuf_PUMP_CR1_CLKSEL | sigBuf_PUMP_CR1_FORCE);

    /* Enable power to buffer in active mode */
    sigBuf_PM_ACT_CFG_REG |= sigBuf_ACT_PWR_EN;

    /* Enable power to buffer in alternative active mode */
    sigBuf_PM_STBY_CFG_REG |= sigBuf_STBY_PWR_EN;
}


/*******************************************************************************
* Function Name:   sigBuf_Start
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
*  sigBuf_initVar: Used to check the initial configuration, modified 
*  when this function is called for the first time.
*
*******************************************************************************/
void sigBuf_Start(void) 
{
    if(sigBuf_initVar == 0u)
    {
        sigBuf_initVar = 1u;
        sigBuf_Init();
    }

    sigBuf_Enable();
}


/*******************************************************************************
* Function Name: sigBuf_Stop
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
void sigBuf_Stop(void) 
{
    /* Disable power to buffer in active mode template */
    sigBuf_PM_ACT_CFG_REG &= (uint8)(~sigBuf_ACT_PWR_EN);

    /* Disable power to buffer in alternative active mode template */
    sigBuf_PM_STBY_CFG_REG &= (uint8)(~sigBuf_STBY_PWR_EN);
    
    /* Disable negative charge pumps for ANIF only if all ABuf is turned OFF */
    if(sigBuf_PM_ACT_CFG_REG == 0u)
    {
        sigBuf_PUMP_CR1_REG &= (uint8)(~(sigBuf_PUMP_CR1_CLKSEL | sigBuf_PUMP_CR1_FORCE));
    }
}


/*******************************************************************************
* Function Name: sigBuf_SetPower
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
void sigBuf_SetPower(uint8 power) 
{
    #if (CY_PSOC3 || CY_PSOC5LP)
        sigBuf_CR_REG &= (uint8)(~sigBuf_PWR_MASK);
        sigBuf_CR_REG |= power & sigBuf_PWR_MASK;      /* Set device power */
    #else
        CYASSERT(sigBuf_HIGHPOWER == power);
    #endif /* CY_PSOC3 || CY_PSOC5LP */
}


/* [] END OF FILE */
