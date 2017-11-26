/*******************************************************************************
* File Name: ShiftClockADC_PM.c
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

#include "ShiftClockADC.h"

static ShiftClockADC_BACKUP_STRUCT ShiftClockADC_backup =
{
    ShiftClockADC_DISABLED,

    ((uint32) ShiftClockADC_DEFAULT_A0),
    ((uint32) ShiftClockADC_DEFAULT_A1),

    #if(CY_UDB_V0)
        ((uint32) ShiftClockADC_INT_SRC),
    #endif /* (CY_UDB_V0) */
};


/*******************************************************************************
* Function Name: ShiftClockADC_SaveConfig
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
void ShiftClockADC_SaveConfig(void) 
{
    /* Store working registers A0 and A1 */
    ShiftClockADC_backup.saveSrA0Reg   = CY_GET_REG32(ShiftClockADC_SHIFT_REG_LSB_PTR);
    ShiftClockADC_backup.saveSrA1Reg   = CY_GET_REG32(ShiftClockADC_SHIFT_REG_VALUE_LSB_PTR);

    #if(CY_UDB_V0)
        ShiftClockADC_backup.saveSrIntMask = ShiftClockADC_SR_STATUS_MASK;
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: ShiftClockADC_RestoreConfig
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
void ShiftClockADC_RestoreConfig(void) 
{
    /* Restore working registers A0 and A1 */
    CY_SET_REG32(ShiftClockADC_SHIFT_REG_LSB_PTR, ShiftClockADC_backup.saveSrA0Reg);
    CY_SET_REG32(ShiftClockADC_SHIFT_REG_VALUE_LSB_PTR, ShiftClockADC_backup.saveSrA1Reg);

    #if(CY_UDB_V0)
        ShiftClockADC_SR_STATUS_MASK = ((uint8) ShiftClockADC_backup.saveSrIntMask);
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: ShiftClockADC_Sleep
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
void ShiftClockADC_Sleep(void) 
{
    ShiftClockADC_backup.enableState = ((uint8) ShiftClockADC_IS_ENABLED);

    ShiftClockADC_Stop();
    ShiftClockADC_SaveConfig();
}


/*******************************************************************************
* Function Name: ShiftClockADC_Wakeup
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
void ShiftClockADC_Wakeup(void) 
{
    ShiftClockADC_RestoreConfig();

    if(0u != ShiftClockADC_backup.enableState)
    {
        ShiftClockADC_Enable();
    }
}


/* [] END OF FILE */
