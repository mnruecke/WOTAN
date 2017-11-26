/*******************************************************************************
* File Name: sigBuf_PM.c
* Version 1.90
*
* Description:
*  This file provides the power management source code to the API for the 
*  OpAmp (Analog Buffer) component.
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

static sigBuf_BACKUP_STRUCT  sigBuf_backup;


/*******************************************************************************  
* Function Name: sigBuf_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration registers.
* 
* Parameters:
*  void
* 
* Return:
*  void
*
*******************************************************************************/
void sigBuf_SaveConfig(void) 
{
    /* Nothing to save as registers are System reset on retention flops */
}


/*******************************************************************************  
* Function Name: sigBuf_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration registers.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void sigBuf_RestoreConfig(void) 
{
    /* Nothing to restore */
}


/*******************************************************************************   
* Function Name: sigBuf_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves its configuration. Should be called 
*  just prior to entering sleep.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  sigBuf_backup: The structure field 'enableState' is modified 
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void sigBuf_Sleep(void) 
{
    /* Save OpAmp enable state */
    if((sigBuf_PM_ACT_CFG_REG & sigBuf_ACT_PWR_EN) != 0u)
    {
        /* Component is enabled */
        sigBuf_backup.enableState = 1u;
         /* Stops the component */
         sigBuf_Stop();
    }
    else
    {
        /* Component is disabled */
        sigBuf_backup.enableState = 0u;
    }
    /* Saves the configuration */
    sigBuf_SaveConfig();
}


/*******************************************************************************  
* Function Name: sigBuf_Wakeup
********************************************************************************
*
* Summary:
*  Enables block's operation and restores its configuration. Should be called
*  just after awaking from sleep.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  sigBuf_backup: The structure field 'enableState' is used to 
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void sigBuf_Wakeup(void) 
{
    /* Restore the user configuration */
    sigBuf_RestoreConfig();

    /* Enables the component operation */
    if(sigBuf_backup.enableState == 1u)
    {
        sigBuf_Enable();
    } /* Do nothing if component was disable before */
}


/* [] END OF FILE */
