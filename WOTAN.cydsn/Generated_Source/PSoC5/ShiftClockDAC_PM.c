/*******************************************************************************
* File Name: ShiftClockDAC_PM.c
* Version 2.30
*
* Description:
*  This file provides the API source code for sleep mode support for Shift
*  Register component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ShiftClockDAC.h"

static ShiftClockDAC_BACKUP_STRUCT ShiftClockDAC_backup =
{
    ShiftClockDAC_DISABLED,

    ((uint8) ShiftClockDAC_DEFAULT_A0),
    ((uint8) ShiftClockDAC_DEFAULT_A1),

    #if(CY_UDB_V0)
        ((uint8) ShiftClockDAC_INT_SRC),
    #endif /* (CY_UDB_V0) */
};


/*******************************************************************************
* Function Name: ShiftClockDAC_SaveConfig
********************************************************************************
*
* Summary:
*  Saves Shift Register configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ShiftClockDAC_SaveConfig(void) 
{
    /* Store working registers A0 and A1 */
    ShiftClockDAC_backup.saveSrA0Reg   = CY_GET_REG8(ShiftClockDAC_SHIFT_REG_LSB_PTR);
    ShiftClockDAC_backup.saveSrA1Reg   = CY_GET_REG8(ShiftClockDAC_SHIFT_REG_VALUE_LSB_PTR);

    #if(CY_UDB_V0)
        ShiftClockDAC_backup.saveSrIntMask = ShiftClockDAC_SR_STATUS_MASK;
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: ShiftClockDAC_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores Shift Register configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftClockDAC_RestoreConfig(void) 
{
    /* Restore working registers A0 and A1 */
    CY_SET_REG8(ShiftClockDAC_SHIFT_REG_LSB_PTR, ShiftClockDAC_backup.saveSrA0Reg);
    CY_SET_REG8(ShiftClockDAC_SHIFT_REG_VALUE_LSB_PTR, ShiftClockDAC_backup.saveSrA1Reg);

    #if(CY_UDB_V0)
        ShiftClockDAC_SR_STATUS_MASK = ((uint8) ShiftClockDAC_backup.saveSrIntMask);
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: ShiftClockDAC_Sleep
********************************************************************************
*
* Summary:
*  Prepare the component to enter a Sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ShiftClockDAC_Sleep(void) 
{
    ShiftClockDAC_backup.enableState = ((uint8) ShiftClockDAC_IS_ENABLED);

    ShiftClockDAC_Stop();
    ShiftClockDAC_SaveConfig();
}


/*******************************************************************************
* Function Name: ShiftClockDAC_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftClockDAC_Wakeup(void) 
{
    ShiftClockDAC_RestoreConfig();

    if(0u != ShiftClockDAC_backup.enableState)
    {
        ShiftClockDAC_Enable();
    }
}


/* [] END OF FILE */
