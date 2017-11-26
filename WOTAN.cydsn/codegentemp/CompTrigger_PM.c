/*******************************************************************************
* File Name: CompTrigger.c
* Version 2.0
*
* Description:
*  This file provides the power management source code APIs for the
*  Comparator.
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

#include "CompTrigger.h"

static CompTrigger_backupStruct CompTrigger_backup;


/*******************************************************************************
* Function Name: CompTrigger_SaveConfig
********************************************************************************
*
* Summary:
*  Save the current user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
*******************************************************************************/
void CompTrigger_SaveConfig(void) 
{
    /* Empty since all are system reset for retention flops */
}


/*******************************************************************************
* Function Name: CompTrigger_RestoreConfig
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
********************************************************************************/
void CompTrigger_RestoreConfig(void) 
{
    /* Empty since all are system reset for retention flops */    
}


/*******************************************************************************
* Function Name: CompTrigger_Sleep
********************************************************************************
*
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
*  CompTrigger_backup.enableState:  Is modified depending on the enable 
*   state of the block before entering sleep mode.
*
*******************************************************************************/
void CompTrigger_Sleep(void) 
{
    /* Save Comp's enable state */    
    if(CompTrigger_ACT_PWR_EN == (CompTrigger_PWRMGR & CompTrigger_ACT_PWR_EN))
    {
        /* Comp is enabled */
        CompTrigger_backup.enableState = 1u;
    }
    else
    {
        /* Comp is disabled */
        CompTrigger_backup.enableState = 0u;
    }    
    
    CompTrigger_Stop();
    CompTrigger_SaveConfig();
}


/*******************************************************************************
* Function Name: CompTrigger_Wakeup
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
*  CompTrigger_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void CompTrigger_Wakeup(void) 
{
    CompTrigger_RestoreConfig();
    
    if(CompTrigger_backup.enableState == 1u)
    {
        /* Enable Comp's operation */
        CompTrigger_Enable();

    } /* Do nothing if Comp was disabled before */ 
}


/* [] END OF FILE */
