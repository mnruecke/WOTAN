/*******************************************************************************
* File Name: Track_Hold_1.c
* Version 1.40
*
* Description:
*  This file provides the power manager source code to the API for 
*  the SAMPLE/TRACK AND HOLD component.
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

#include "Track_Hold_1.h"

static Track_Hold_1_backupStruct  Track_Hold_1_backup;


/*******************************************************************************
* Function Name: Track_Hold_1_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
*  
* Parameters:  
*  void.
*
* Return: 
*  void.
*
*******************************************************************************/
void Track_Hold_1_SaveConfig(void) 
{
    /* Nothing to save before entering into sleep mode as all the registers used 
       here are retention registers. */
}


/*******************************************************************************
* Function Name: Track_Hold_1_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void.
*
* Return: 
*  void
*
*******************************************************************************/
void Track_Hold_1_RestoreConfig(void) 
{
    /* Nothing to restore */
}


/*******************************************************************************
* Function Name: Track_Hold_1_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves its configuration. Should be called 
*  just prior to entering sleep.
*  
* Parameters:  
*  None
*
* Return: 
*  None
*
* Global variables:
*  Track_Hold_1_backup:  The structure field 'enableState' is modified 
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void Track_Hold_1_Sleep(void) 
{
    /* Save TIA enable state */
    if((Track_Hold_1_PM_ACT_CFG_REG & Track_Hold_1_ACT_PWR_EN) != 0u)
    {
        /* Component is enabled */
        Track_Hold_1_backup.enableState = 1u;
        /* Stop the configuration */
        Track_Hold_1_Stop();
    }
    else
    {
        /* Component is disabled */
        Track_Hold_1_backup.enableState = 0u;
    }
    /* Saves the user configuration */
    Track_Hold_1_SaveConfig();
}


/*******************************************************************************
* Function Name: Track_Hold_1_Wakeup
********************************************************************************
*
* Summary:
*  Enables block's operation and restores its configuration. Should be called
*  just after awaking from sleep.
*  
* Parameters:  
*  None
*
* Return: 
*  void
*
* Global variables:
*  Track_Hold_1_backup:  The structure field 'enableState' is used to 
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void Track_Hold_1_Wakeup(void) 
{
    /* Restore the user configuration */
    Track_Hold_1_RestoreConfig();
    
    /* Enable's the component operation */
    if(Track_Hold_1_backup.enableState == 1u)
    {
        Track_Hold_1_Enable();
    } /* Do nothing if component was disable before */
}


/* [] END OF FILE */
