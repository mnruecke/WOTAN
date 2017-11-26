/*******************************************************************************
* File Name: internTrigger_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "internTrigger.h"

/* Check for removal by optimization */
#if !defined(internTrigger_Sync_ctrl_reg__REMOVED)

static internTrigger_BACKUP_STRUCT  internTrigger_backup = {0u};

    
/*******************************************************************************
* Function Name: internTrigger_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void internTrigger_SaveConfig(void) 
{
    internTrigger_backup.controlState = internTrigger_Control;
}


/*******************************************************************************
* Function Name: internTrigger_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void internTrigger_RestoreConfig(void) 
{
     internTrigger_Control = internTrigger_backup.controlState;
}


/*******************************************************************************
* Function Name: internTrigger_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void internTrigger_Sleep(void) 
{
    internTrigger_SaveConfig();
}


/*******************************************************************************
* Function Name: internTrigger_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void internTrigger_Wakeup(void)  
{
    internTrigger_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
