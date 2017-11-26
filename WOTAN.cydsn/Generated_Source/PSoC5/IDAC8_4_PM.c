/*******************************************************************************
* File Name: IDAC8_4.c
* Version 2.0
*
* Description:
*  This file provides the power management source code to API for the
*  IDAC8.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include "IDAC8_4.h"

static IDAC8_4_backupStruct IDAC8_4_backup;


/*******************************************************************************
* Function Name: IDAC8_4_SaveConfig
********************************************************************************
* Summary:
*  Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void IDAC8_4_SaveConfig(void) 
{
    if (!((IDAC8_4_CR1 & IDAC8_4_SRC_MASK) == IDAC8_4_SRC_UDB))
    {
        IDAC8_4_backup.data_value = IDAC8_4_Data;
    }
}


/*******************************************************************************
* Function Name: IDAC8_4_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void IDAC8_4_RestoreConfig(void) 
{
    if (!((IDAC8_4_CR1 & IDAC8_4_SRC_MASK) == IDAC8_4_SRC_UDB))
    {
        if((IDAC8_4_Strobe & IDAC8_4_STRB_MASK) == IDAC8_4_STRB_EN)
        {
            IDAC8_4_Strobe &= (uint8)(~IDAC8_4_STRB_MASK);
            IDAC8_4_Data = IDAC8_4_backup.data_value;
            IDAC8_4_Strobe |= IDAC8_4_STRB_EN;
        }
        else
        {
            IDAC8_4_Data = IDAC8_4_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: IDAC8_4_Sleep
********************************************************************************
* Summary:
*  Stop and Save the user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
* Global variables:
*  IDAC8_4_backup.enableState: Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void IDAC8_4_Sleep(void) 
{
    if(IDAC8_4_ACT_PWR_EN == (IDAC8_4_PWRMGR & IDAC8_4_ACT_PWR_EN))
    {
        /* IDAC8 is enabled */
        IDAC8_4_backup.enableState = 1u;
    }
    else
    {
        /* IDAC8 is disabled */
        IDAC8_4_backup.enableState = 0u;
    }

    IDAC8_4_Stop();
    IDAC8_4_SaveConfig();
}


/*******************************************************************************
* Function Name: IDAC8_4_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  IDAC8_4_backup.enableState: Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void IDAC8_4_Wakeup(void) 
{
    IDAC8_4_RestoreConfig();
    
    if(IDAC8_4_backup.enableState == 1u)
    {
        /* Enable IDAC8's operation */
        IDAC8_4_Enable();
        
        /* Set the data register */
        IDAC8_4_SetValue(IDAC8_4_Data);
    } /* Do nothing if IDAC8 was disabled before */    
}


/* [] END OF FILE */
