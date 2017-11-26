/*******************************************************************************
* File Name: ShiftClockADC.c
* Version 2.30
*
* Description:
*  This file provides the API source code for the Shift Register component.
*
* Note: none
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "ShiftClockADC.h"

uint8 ShiftClockADC_initVar = 0u;


/*******************************************************************************
* Function Name: ShiftClockADC_Start
********************************************************************************
*
* Summary:
*  Starts the Shift Register.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ShiftClockADC_initVar - used to check initial configuration, modified on
*  first function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ShiftClockADC_Start(void) 
{
    if(0u == ShiftClockADC_initVar)
    {
        ShiftClockADC_Init();
        ShiftClockADC_initVar = 1u; /* Component initialized */
    }

    ShiftClockADC_Enable();
}


/*******************************************************************************
* Function Name: ShiftClockADC_Enable
********************************************************************************
*
* Summary:
*  Enables the Shift Register.
*
* Parameters:
*  void.
*
* Return:
*  void.
*
*******************************************************************************/
void ShiftClockADC_Enable(void) 
{
    /* Changing address in Datapath Control Store
       from NOP to component state machine commands space */
    ShiftClockADC_SR_CONTROL |= ShiftClockADC_CLK_EN;

    ShiftClockADC_EnableInt();
}


/*******************************************************************************
* Function Name: ShiftClockADC_Init
********************************************************************************
*
* Summary:
*  Initializes Tx and/or Rx interrupt sources with initial values.
*
* Parameters:
*  void.
*
* Return:
*  void.
*
*******************************************************************************/
void ShiftClockADC_Init(void) 
{
    ShiftClockADC_SetIntMode(ShiftClockADC_INT_SRC);
}


/*******************************************************************************
* Function Name: ShiftClockADC_Stop
********************************************************************************
*
* Summary:
*  Disables the Shift Register
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftClockADC_Stop(void) 
{
    /*changing Datapath Control Store address to NOP space*/
    ShiftClockADC_SR_CONTROL &= ((uint8) ~ShiftClockADC_CLK_EN);
    ShiftClockADC_DisableInt();
}


/*******************************************************************************
* Function Name: ShiftClockADC_EnableInt
********************************************************************************
*
* Summary:
*  Enables the Shift Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftClockADC_EnableInt(void) 
{
    uint8 interruptState;

    interruptState = CyEnterCriticalSection();
    ShiftClockADC_SR_AUX_CONTROL |= ShiftClockADC_INTERRUPTS_ENABLE;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: ShiftClockADC_DisableInt
********************************************************************************
*
* Summary:
*  Disables the Shift Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftClockADC_DisableInt(void) 
{
    uint8 interruptState;

    interruptState = CyEnterCriticalSection();
    ShiftClockADC_SR_AUX_CONTROL &= ((uint8) ~ShiftClockADC_INTERRUPTS_ENABLE);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: ShiftClockADC_GetFIFOStatus
********************************************************************************
*
* Summary:
*  Returns current status of input or output FIFO.
*
* Parameters:
*  fifoId.
*
* Return:
*  FIFO status.
*
*******************************************************************************/
uint8 ShiftClockADC_GetFIFOStatus(uint8 fifoId) 
{
    uint8 result;

    result = ShiftClockADC_RET_FIFO_NOT_DEFINED;

    #if(0u != ShiftClockADC_USE_INPUT_FIFO)
        if(ShiftClockADC_IN_FIFO == fifoId)
        {
            switch(ShiftClockADC_GET_IN_FIFO_STS)
            {
                case ShiftClockADC_IN_FIFO_FULL :
                    result = ShiftClockADC_RET_FIFO_FULL;
                    break;

                case ShiftClockADC_IN_FIFO_EMPTY :
                    result = ShiftClockADC_RET_FIFO_EMPTY;
                    break;

                case ShiftClockADC_IN_FIFO_PARTIAL:
                    result = ShiftClockADC_RET_FIFO_PARTIAL;
                    break;
                    
                default:
                    /* Initial result value, while 
                       IN_FIFO_EMPTY case is false 
                     */
                    result = ShiftClockADC_RET_FIFO_EMPTY;
                    break;
            }   
        }
    #endif /* (0u != ShiftClockADC_USE_INPUT_FIFO) */

    if(ShiftClockADC_OUT_FIFO == fifoId)
    {
        switch(ShiftClockADC_GET_OUT_FIFO_STS)
        {
            case ShiftClockADC_OUT_FIFO_FULL :
                result = ShiftClockADC_RET_FIFO_FULL;
                break;

            case ShiftClockADC_OUT_FIFO_EMPTY :
                result = ShiftClockADC_RET_FIFO_EMPTY;
                break;

            case ShiftClockADC_OUT_FIFO_PARTIAL :
                result = ShiftClockADC_RET_FIFO_PARTIAL;
                break;

            default:
                /* Initial result value, while 
                   OUT_FIFO_FULL case is false 
                 */
                result = ShiftClockADC_RET_FIFO_FULL;
                break;
        }
    }

    return(result);
}


/*******************************************************************************
* Function Name: ShiftClockADC_SetIntMode
********************************************************************************
*
* Summary:
*  Sets the Interrupt Source for the Shift Register interrupt. Multiple
*  sources may be ORed together
*
* Parameters:
*  interruptSource: Byte containing the constant for the selected interrupt
*  source/s.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftClockADC_SetIntMode(uint8 interruptSource) 
{
    ShiftClockADC_SR_STATUS_MASK &= ((uint8) ~ShiftClockADC_INTS_EN_MASK);          /* Clear existing int */
    ShiftClockADC_SR_STATUS_MASK |= (interruptSource & ShiftClockADC_INTS_EN_MASK); /* Set int */
}


/*******************************************************************************
* Function Name: ShiftClockADC_GetIntStatus
********************************************************************************
*
* Summary:
*  Gets the Shift Register Interrupt status.
*
* Parameters:
*  None.
*
* Return:
*  Byte containing the constant for the selected interrupt source/s.
*
*******************************************************************************/
uint8 ShiftClockADC_GetIntStatus(void) 
{
    return(ShiftClockADC_SR_STATUS & ShiftClockADC_INTS_EN_MASK);
}


/*******************************************************************************
* Function Name: ShiftClockADC_WriteRegValue
********************************************************************************
*
* Summary:
*  Send state directly to shift register
*
* Parameters:
*  shiftData: containing shift register state.
*
* Return:
*  None.
*
*******************************************************************************/
void ShiftClockADC_WriteRegValue(uint32 shiftData)
                                                                     
{
    CY_SET_REG32(ShiftClockADC_SHIFT_REG_LSB_PTR, shiftData);
}


#if(0u != ShiftClockADC_USE_INPUT_FIFO)
    /*******************************************************************************
    * Function Name: ShiftClockADC_WriteData
    ********************************************************************************
    *
    * Summary:
    *  Send state to FIFO for later transfer to shift register based on the Load
    *  input
    *
    * Parameters:
    *  shiftData: containing shift register state.
    *
    * Return:
    *  Indicates: successful execution of function
    *  when FIFO is empty; and error when FIFO is full.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    cystatus ShiftClockADC_WriteData(uint32 shiftData)
                                                                         
    {
        cystatus result;

        result = CYRET_INVALID_STATE;

        /* Writes data into the input FIFO if it is not FULL */
        if(ShiftClockADC_RET_FIFO_FULL != (ShiftClockADC_GetFIFOStatus(ShiftClockADC_IN_FIFO)))
        {
            CY_SET_REG32(ShiftClockADC_IN_FIFO_VAL_LSB_PTR, shiftData);
            result = CYRET_SUCCESS;
        }

        return(result);
    }
#endif /* (0u != ShiftClockADC_USE_INPUT_FIFO) */


#if(0u != ShiftClockADC_USE_OUTPUT_FIFO)
    /*******************************************************************************
    * Function Name: ShiftClockADC_ReadData
    ********************************************************************************
    *
    * Summary:
    *  Returns state in FIFO due to Store input.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Shift Register state
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint32 ShiftClockADC_ReadData(void) 
    {
        return(CY_GET_REG32(ShiftClockADC_OUT_FIFO_VAL_LSB_PTR));
    }
#endif /* (0u != ShiftClockADC_USE_OUTPUT_FIFO) */


/*******************************************************************************
* Function Name: ShiftClockADC_ReadRegValue
********************************************************************************
*
* Summary:
*  Directly returns current state in shift register, not data in FIFO due
*  to Store input.
*
* Parameters:
*  None.
*
* Return:
*  Shift Register state. Clears output FIFO.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint32 ShiftClockADC_ReadRegValue(void) 
{
    uint32 result;

    /* Clear FIFO before software capture */
    while(ShiftClockADC_RET_FIFO_EMPTY != ShiftClockADC_GetFIFOStatus(ShiftClockADC_OUT_FIFO))
    {
        (void) CY_GET_REG32(ShiftClockADC_OUT_FIFO_VAL_LSB_PTR);
    }

    /* Read of 8 bits from A1 causes capture to output FIFO */
    (void) CY_GET_REG8(ShiftClockADC_SHIFT_REG_CAPTURE_PTR);

    /* Read output FIFO */
    result  = CY_GET_REG32(ShiftClockADC_OUT_FIFO_VAL_LSB_PTR);
    
    #if(0u != (ShiftClockADC_SR_SIZE % 8u))
        result &= ((uint32) ShiftClockADC_SR_MASK);
    #endif /* (0u != (ShiftClockADC_SR_SIZE % 8u)) */
    
    return(result);
}


/* [] END OF FILE */
