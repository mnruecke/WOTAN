/*******************************************************************************
* File Name: pwmSAMPLING_PM.c
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

#include "pwmSAMPLING.h"

static pwmSAMPLING_backupStruct pwmSAMPLING_backup;


/*******************************************************************************
* Function Name: pwmSAMPLING_SaveConfig
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
*  pwmSAMPLING_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void pwmSAMPLING_SaveConfig(void) 
{

    #if(!pwmSAMPLING_UsingFixedFunction)
        #if(!pwmSAMPLING_PWMModeIsCenterAligned)
            pwmSAMPLING_backup.PWMPeriod = pwmSAMPLING_ReadPeriod();
        #endif /* (!pwmSAMPLING_PWMModeIsCenterAligned) */
        pwmSAMPLING_backup.PWMUdb = pwmSAMPLING_ReadCounter();
        #if (pwmSAMPLING_UseStatus)
            pwmSAMPLING_backup.InterruptMaskValue = pwmSAMPLING_STATUS_MASK;
        #endif /* (pwmSAMPLING_UseStatus) */

        #if(pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_256_CLOCKS || \
            pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_2_4_CLOCKS)
            pwmSAMPLING_backup.PWMdeadBandValue = pwmSAMPLING_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(pwmSAMPLING_KillModeMinTime)
             pwmSAMPLING_backup.PWMKillCounterPeriod = pwmSAMPLING_ReadKillTime();
        #endif /* (pwmSAMPLING_KillModeMinTime) */

        #if(pwmSAMPLING_UseControl)
            pwmSAMPLING_backup.PWMControlRegister = pwmSAMPLING_ReadControlRegister();
        #endif /* (pwmSAMPLING_UseControl) */
    #endif  /* (!pwmSAMPLING_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: pwmSAMPLING_RestoreConfig
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
*  pwmSAMPLING_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void pwmSAMPLING_RestoreConfig(void) 
{
        #if(!pwmSAMPLING_UsingFixedFunction)
            #if(!pwmSAMPLING_PWMModeIsCenterAligned)
                pwmSAMPLING_WritePeriod(pwmSAMPLING_backup.PWMPeriod);
            #endif /* (!pwmSAMPLING_PWMModeIsCenterAligned) */

            pwmSAMPLING_WriteCounter(pwmSAMPLING_backup.PWMUdb);

            #if (pwmSAMPLING_UseStatus)
                pwmSAMPLING_STATUS_MASK = pwmSAMPLING_backup.InterruptMaskValue;
            #endif /* (pwmSAMPLING_UseStatus) */

            #if(pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_256_CLOCKS || \
                pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_2_4_CLOCKS)
                pwmSAMPLING_WriteDeadTime(pwmSAMPLING_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(pwmSAMPLING_KillModeMinTime)
                pwmSAMPLING_WriteKillTime(pwmSAMPLING_backup.PWMKillCounterPeriod);
            #endif /* (pwmSAMPLING_KillModeMinTime) */

            #if(pwmSAMPLING_UseControl)
                pwmSAMPLING_WriteControlRegister(pwmSAMPLING_backup.PWMControlRegister);
            #endif /* (pwmSAMPLING_UseControl) */
        #endif  /* (!pwmSAMPLING_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: pwmSAMPLING_Sleep
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
*  pwmSAMPLING_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void pwmSAMPLING_Sleep(void) 
{
    #if(pwmSAMPLING_UseControl)
        if(pwmSAMPLING_CTRL_ENABLE == (pwmSAMPLING_CONTROL & pwmSAMPLING_CTRL_ENABLE))
        {
            /*Component is enabled */
            pwmSAMPLING_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            pwmSAMPLING_backup.PWMEnableState = 0u;
        }
    #endif /* (pwmSAMPLING_UseControl) */

    /* Stop component */
    pwmSAMPLING_Stop();

    /* Save registers configuration */
    pwmSAMPLING_SaveConfig();
}


/*******************************************************************************
* Function Name: pwmSAMPLING_Wakeup
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
*  pwmSAMPLING_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void pwmSAMPLING_Wakeup(void) 
{
     /* Restore registers values */
    pwmSAMPLING_RestoreConfig();

    if(pwmSAMPLING_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        pwmSAMPLING_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
