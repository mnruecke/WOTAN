/*******************************************************************************
* File Name: resetClock_PM.c
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

#include "resetClock.h"

/* Check for removal by optimization */
#if !defined(resetClock_Sync_ctrl_reg__REMOVED)

static resetClock_BACKUP_STRUCT  resetClock_backup = {0u};

    
/*******************************************************************************
* Function Name: resetClock_SaveConfig
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
void resetClock_SaveConfig(void) 
{
    resetClock_backup.controlState = resetClock_Control;
}


/*******************************************************************************
* Function Name: resetClock_RestoreConfig
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
void resetClock_RestoreConfig(void) 
{
     resetClock_Control = resetClock_backup.controlState;
}


/*******************************************************************************
* Function Name: resetClock_Sleep
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
void resetClock_Sleep(void) 
{
    resetClock_SaveConfig();
}


/*******************************************************************************
* Function Name: resetClock_Wakeup
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
void resetClock_Wakeup(void)  
{
    resetClock_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
