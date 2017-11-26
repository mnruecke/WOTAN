/*******************************************************************************
* File Name: pwmClkADC_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "pwmClkADC.h"

static pwmClkADC_backupStruct pwmClkADC_backup;


/*******************************************************************************
* Function Name: pwmClkADC_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmClkADC_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void pwmClkADC_SaveConfig(void) 
{

    #if(!pwmClkADC_UsingFixedFunction)
        #if(!pwmClkADC_PWMModeIsCenterAligned)
            pwmClkADC_backup.PWMPeriod = pwmClkADC_ReadPeriod();
        #endif /* (!pwmClkADC_PWMModeIsCenterAligned) */
        pwmClkADC_backup.PWMUdb = pwmClkADC_ReadCounter();
        #if (pwmClkADC_UseStatus)
            pwmClkADC_backup.InterruptMaskValue = pwmClkADC_STATUS_MASK;
        #endif /* (pwmClkADC_UseStatus) */

        #if(pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_256_CLOCKS || \
            pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_2_4_CLOCKS)
            pwmClkADC_backup.PWMdeadBandValue = pwmClkADC_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(pwmClkADC_KillModeMinTime)
             pwmClkADC_backup.PWMKillCounterPeriod = pwmClkADC_ReadKillTime();
        #endif /* (pwmClkADC_KillModeMinTime) */

        #if(pwmClkADC_UseControl)
            pwmClkADC_backup.PWMControlRegister = pwmClkADC_ReadControlRegister();
        #endif /* (pwmClkADC_UseControl) */
    #endif  /* (!pwmClkADC_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: pwmClkADC_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmClkADC_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void pwmClkADC_RestoreConfig(void) 
{
        #if(!pwmClkADC_UsingFixedFunction)
            #if(!pwmClkADC_PWMModeIsCenterAligned)
                pwmClkADC_WritePeriod(pwmClkADC_backup.PWMPeriod);
            #endif /* (!pwmClkADC_PWMModeIsCenterAligned) */

            pwmClkADC_WriteCounter(pwmClkADC_backup.PWMUdb);

            #if (pwmClkADC_UseStatus)
                pwmClkADC_STATUS_MASK = pwmClkADC_backup.InterruptMaskValue;
            #endif /* (pwmClkADC_UseStatus) */

            #if(pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_256_CLOCKS || \
                pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_2_4_CLOCKS)
                pwmClkADC_WriteDeadTime(pwmClkADC_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(pwmClkADC_KillModeMinTime)
                pwmClkADC_WriteKillTime(pwmClkADC_backup.PWMKillCounterPeriod);
            #endif /* (pwmClkADC_KillModeMinTime) */

            #if(pwmClkADC_UseControl)
                pwmClkADC_WriteControlRegister(pwmClkADC_backup.PWMControlRegister);
            #endif /* (pwmClkADC_UseControl) */
        #endif  /* (!pwmClkADC_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: pwmClkADC_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmClkADC_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void pwmClkADC_Sleep(void) 
{
    #if(pwmClkADC_UseControl)
        if(pwmClkADC_CTRL_ENABLE == (pwmClkADC_CONTROL & pwmClkADC_CTRL_ENABLE))
        {
            /*Component is enabled */
            pwmClkADC_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            pwmClkADC_backup.PWMEnableState = 0u;
        }
    #endif /* (pwmClkADC_UseControl) */

    /* Stop component */
    pwmClkADC_Stop();

    /* Save registers configuration */
    pwmClkADC_SaveConfig();
}


/*******************************************************************************
* Function Name: pwmClkADC_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmClkADC_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void pwmClkADC_Wakeup(void) 
{
     /* Restore registers values */
    pwmClkADC_RestoreConfig();

    if(pwmClkADC_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        pwmClkADC_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
