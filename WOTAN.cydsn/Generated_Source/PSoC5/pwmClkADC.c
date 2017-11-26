/*******************************************************************************
* File Name: pwmClkADC.c
* Version 3.30
*
* Description:
*  The PWM User Module consist of an 8 or 16-bit counter with two 8 or 16-bit
*  comparitors. Each instance of this user module is capable of generating
*  two PWM outputs with the same period. The pulse width is selectable between
*  1 and 255/65535. The period is selectable between 2 and 255/65536 clocks.
*  The compare value output may be configured to be active when the present
*  counter is less than or less than/equal to the compare value.
*  A terminal count output is also provided. It generates a pulse one clock
*  width wide when the counter is equal to zero.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "pwmClkADC.h"

/* Error message for removed <resource> through optimization */
#ifdef pwmClkADC_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* pwmClkADC_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 pwmClkADC_initVar = 0u;


/*******************************************************************************
* Function Name: pwmClkADC_Start
********************************************************************************
*
* Summary:
*  The start function initializes the pwm with the default values, the
*  enables the counter to begin counting.  It does not enable interrupts,
*  the EnableInt command should be called if interrupt generation is required.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  pwmClkADC_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void pwmClkADC_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(pwmClkADC_initVar == 0u)
    {
        pwmClkADC_Init();
        pwmClkADC_initVar = 1u;
    }
    pwmClkADC_Enable();

}


/*******************************************************************************
* Function Name: pwmClkADC_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  pwmClkADC_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void pwmClkADC_Init(void) 
{
    #if (pwmClkADC_UsingFixedFunction || pwmClkADC_UseControl)
        uint8 ctrl;
    #endif /* (pwmClkADC_UsingFixedFunction || pwmClkADC_UseControl) */

    #if(!pwmClkADC_UsingFixedFunction)
        #if(pwmClkADC_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 pwmClkADC_interruptState;
        #endif /* (pwmClkADC_UseStatus) */
    #endif /* (!pwmClkADC_UsingFixedFunction) */

    #if (pwmClkADC_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        pwmClkADC_CONTROL |= pwmClkADC_CFG0_MODE;
        #if (pwmClkADC_DeadBand2_4)
            pwmClkADC_CONTROL |= pwmClkADC_CFG0_DB;
        #endif /* (pwmClkADC_DeadBand2_4) */

        ctrl = pwmClkADC_CONTROL3 & ((uint8 )(~pwmClkADC_CTRL_CMPMODE1_MASK));
        pwmClkADC_CONTROL3 = ctrl | pwmClkADC_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        pwmClkADC_RT1 &= ((uint8)(~pwmClkADC_RT1_MASK));
        pwmClkADC_RT1 |= pwmClkADC_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        pwmClkADC_RT1 &= ((uint8)(~pwmClkADC_SYNCDSI_MASK));
        pwmClkADC_RT1 |= pwmClkADC_SYNCDSI_EN;

    #elif (pwmClkADC_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = pwmClkADC_CONTROL & ((uint8)(~pwmClkADC_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~pwmClkADC_CTRL_CMPMODE1_MASK));
        pwmClkADC_CONTROL = ctrl | pwmClkADC_DEFAULT_COMPARE2_MODE |
                                   pwmClkADC_DEFAULT_COMPARE1_MODE;
    #endif /* (pwmClkADC_UsingFixedFunction) */

    #if (!pwmClkADC_UsingFixedFunction)
        #if (pwmClkADC_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            pwmClkADC_AUX_CONTROLDP0 |= (pwmClkADC_AUX_CTRL_FIFO0_CLR);
        #else /* (pwmClkADC_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            pwmClkADC_AUX_CONTROLDP0 |= (pwmClkADC_AUX_CTRL_FIFO0_CLR);
            pwmClkADC_AUX_CONTROLDP1 |= (pwmClkADC_AUX_CTRL_FIFO0_CLR);
        #endif /* (pwmClkADC_Resolution == 8) */

        pwmClkADC_WriteCounter(pwmClkADC_INIT_PERIOD_VALUE);
    #endif /* (!pwmClkADC_UsingFixedFunction) */

    pwmClkADC_WritePeriod(pwmClkADC_INIT_PERIOD_VALUE);

        #if (pwmClkADC_UseOneCompareMode)
            pwmClkADC_WriteCompare(pwmClkADC_INIT_COMPARE_VALUE1);
        #else
            pwmClkADC_WriteCompare1(pwmClkADC_INIT_COMPARE_VALUE1);
            pwmClkADC_WriteCompare2(pwmClkADC_INIT_COMPARE_VALUE2);
        #endif /* (pwmClkADC_UseOneCompareMode) */

        #if (pwmClkADC_KillModeMinTime)
            pwmClkADC_WriteKillTime(pwmClkADC_MinimumKillTime);
        #endif /* (pwmClkADC_KillModeMinTime) */

        #if (pwmClkADC_DeadBandUsed)
            pwmClkADC_WriteDeadTime(pwmClkADC_INIT_DEAD_TIME);
        #endif /* (pwmClkADC_DeadBandUsed) */

    #if (pwmClkADC_UseStatus || pwmClkADC_UsingFixedFunction)
        pwmClkADC_SetInterruptMode(pwmClkADC_INIT_INTERRUPTS_MODE);
    #endif /* (pwmClkADC_UseStatus || pwmClkADC_UsingFixedFunction) */

    #if (pwmClkADC_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        pwmClkADC_GLOBAL_ENABLE |= pwmClkADC_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        pwmClkADC_CONTROL2 |= pwmClkADC_CTRL2_IRQ_SEL;
    #else
        #if(pwmClkADC_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            pwmClkADC_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            pwmClkADC_STATUS_AUX_CTRL |= pwmClkADC_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(pwmClkADC_interruptState);

            /* Clear the FIFO to enable the pwmClkADC_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            pwmClkADC_ClearFIFO();
        #endif /* (pwmClkADC_UseStatus) */
    #endif /* (pwmClkADC_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: pwmClkADC_Enable
********************************************************************************
*
* Summary:
*  Enables the PWM block operation
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This works only if software enable mode is chosen
*
*******************************************************************************/
void pwmClkADC_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (pwmClkADC_UsingFixedFunction)
        pwmClkADC_GLOBAL_ENABLE |= pwmClkADC_BLOCK_EN_MASK;
        pwmClkADC_GLOBAL_STBY_ENABLE |= pwmClkADC_BLOCK_STBY_EN_MASK;
    #endif /* (pwmClkADC_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (pwmClkADC_UseControl || pwmClkADC_UsingFixedFunction)
        pwmClkADC_CONTROL |= pwmClkADC_CTRL_ENABLE;
    #endif /* (pwmClkADC_UseControl || pwmClkADC_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: pwmClkADC_Stop
********************************************************************************
*
* Summary:
*  The stop function halts the PWM, but does not change any modes or disable
*  interrupts.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  If the Enable mode is set to Hardware only then this function
*  has no effect on the operation of the PWM
*
*******************************************************************************/
void pwmClkADC_Stop(void) 
{
    #if (pwmClkADC_UseControl || pwmClkADC_UsingFixedFunction)
        pwmClkADC_CONTROL &= ((uint8)(~pwmClkADC_CTRL_ENABLE));
    #endif /* (pwmClkADC_UseControl || pwmClkADC_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (pwmClkADC_UsingFixedFunction)
        pwmClkADC_GLOBAL_ENABLE &= ((uint8)(~pwmClkADC_BLOCK_EN_MASK));
        pwmClkADC_GLOBAL_STBY_ENABLE &= ((uint8)(~pwmClkADC_BLOCK_STBY_EN_MASK));
    #endif /* (pwmClkADC_UsingFixedFunction) */
}

#if (pwmClkADC_UseOneCompareMode)
    #if (pwmClkADC_CompareMode1SW)


        /*******************************************************************************
        * Function Name: pwmClkADC_SetCompareMode
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm output when in Dither mode,
        *  Center Align Mode or One Output Mode.
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void pwmClkADC_SetCompareMode(uint8 comparemode) 
        {
            #if(pwmClkADC_UsingFixedFunction)

                #if(0 != pwmClkADC_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << pwmClkADC_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != pwmClkADC_CTRL_CMPMODE1_SHIFT) */

                pwmClkADC_CONTROL3 &= ((uint8)(~pwmClkADC_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                pwmClkADC_CONTROL3 |= comparemodemasked;

            #elif (pwmClkADC_UseControl)

                #if(0 != pwmClkADC_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << pwmClkADC_CTRL_CMPMODE1_SHIFT)) &
                                                pwmClkADC_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & pwmClkADC_CTRL_CMPMODE1_MASK;
                #endif /* (0 != pwmClkADC_CTRL_CMPMODE1_SHIFT) */

                #if(0 != pwmClkADC_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << pwmClkADC_CTRL_CMPMODE2_SHIFT)) &
                                               pwmClkADC_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & pwmClkADC_CTRL_CMPMODE2_MASK;
                #endif /* (0 != pwmClkADC_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                pwmClkADC_CONTROL &= ((uint8)(~(pwmClkADC_CTRL_CMPMODE1_MASK |
                                            pwmClkADC_CTRL_CMPMODE2_MASK)));
                pwmClkADC_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (pwmClkADC_UsingFixedFunction) */
        }
    #endif /* pwmClkADC_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (pwmClkADC_CompareMode1SW)


        /*******************************************************************************
        * Function Name: pwmClkADC_SetCompareMode1
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm or pwm1 output
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void pwmClkADC_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != pwmClkADC_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << pwmClkADC_CTRL_CMPMODE1_SHIFT)) &
                                           pwmClkADC_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & pwmClkADC_CTRL_CMPMODE1_MASK;
            #endif /* (0 != pwmClkADC_CTRL_CMPMODE1_SHIFT) */

            #if (pwmClkADC_UseControl)
                pwmClkADC_CONTROL &= ((uint8)(~pwmClkADC_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                pwmClkADC_CONTROL |= comparemodemasked;
            #endif /* (pwmClkADC_UseControl) */
        }
    #endif /* pwmClkADC_CompareMode1SW */

#if (pwmClkADC_CompareMode2SW)


    /*******************************************************************************
    * Function Name: pwmClkADC_SetCompareMode2
    ********************************************************************************
    *
    * Summary:
    *  This function writes the Compare Mode for the pwm or pwm2 output
    *
    * Parameters:
    *  comparemode:  The new compare mode for the PWM output. Use the compare types
    *                defined in the H file as input arguments.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmClkADC_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != pwmClkADC_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << pwmClkADC_CTRL_CMPMODE2_SHIFT)) &
                                                 pwmClkADC_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & pwmClkADC_CTRL_CMPMODE2_MASK;
        #endif /* (0 != pwmClkADC_CTRL_CMPMODE2_SHIFT) */

        #if (pwmClkADC_UseControl)
            pwmClkADC_CONTROL &= ((uint8)(~pwmClkADC_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            pwmClkADC_CONTROL |= comparemodemasked;
        #endif /* (pwmClkADC_UseControl) */
    }
    #endif /*pwmClkADC_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!pwmClkADC_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: pwmClkADC_WriteCounter
    ********************************************************************************
    *
    * Summary:
    *  Writes a new counter value directly to the counter register. This will be
    *  implemented for that currently running period and only that period. This API
    *  is valid only for UDB implementation and not available for fixed function
    *  PWM implementation.
    *
    * Parameters:
    *  counter:  The period new period counter value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  The PWM Period will be reloaded when a counter value will be a zero
    *
    *******************************************************************************/
    void pwmClkADC_WriteCounter(uint8 counter) \
                                       
    {
        CY_SET_REG8(pwmClkADC_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadCounter
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current value of the counter.  It doesn't matter
    *  if the counter is enabled or running.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  The current value of the counter.
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(pwmClkADC_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG8(pwmClkADC_CAPTURE_LSB_PTR));
    }

    #if (pwmClkADC_UseStatus)


        /*******************************************************************************
        * Function Name: pwmClkADC_ClearFIFO
        ********************************************************************************
        *
        * Summary:
        *  This function clears all capture data from the capture FIFO
        *
        * Parameters:
        *  None
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void pwmClkADC_ClearFIFO(void) 
        {
            while(0u != (pwmClkADC_ReadStatusRegister() & pwmClkADC_STATUS_FIFONEMPTY))
            {
                (void)pwmClkADC_ReadCapture();
            }
        }

    #endif /* pwmClkADC_UseStatus */

#endif /* !pwmClkADC_UsingFixedFunction */


/*******************************************************************************
* Function Name: pwmClkADC_WritePeriod
********************************************************************************
*
* Summary:
*  This function is used to change the period of the counter.  The new period
*  will be loaded the next time terminal count is detected.
*
* Parameters:
*  period:  Period value. May be between 1 and (2^Resolution)-1.  A value of 0
*           will result in the counter remaining at zero.
*
* Return:
*  None
*
*******************************************************************************/
void pwmClkADC_WritePeriod(uint8 period) 
{
    #if(pwmClkADC_UsingFixedFunction)
        CY_SET_REG16(pwmClkADC_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG8(pwmClkADC_PERIOD_LSB_PTR, period);
    #endif /* (pwmClkADC_UsingFixedFunction) */
}

#if (pwmClkADC_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: pwmClkADC_WriteCompare
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value when the PWM is in Dither
    *  mode. The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  compared to the compare value based on the compare mode defined in
    *  Dither Mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  This function is only available if the PWM mode parameter is set to
    *  Dither Mode, Center Aligned Mode or One Output Mode
    *
    *******************************************************************************/
    void pwmClkADC_WriteCompare(uint8 compare) \
                                       
    {
        #if(pwmClkADC_UsingFixedFunction)
            CY_SET_REG16(pwmClkADC_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(pwmClkADC_COMPARE1_LSB_PTR, compare);
        #endif /* (pwmClkADC_UsingFixedFunction) */

        #if (pwmClkADC_PWMMode == pwmClkADC__B_PWM__DITHER)
            #if(pwmClkADC_UsingFixedFunction)
                CY_SET_REG16(pwmClkADC_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG8(pwmClkADC_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (pwmClkADC_UsingFixedFunction) */
        #endif /* (pwmClkADC_PWMMode == pwmClkADC__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: pwmClkADC_WriteCompare1
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value.  The compare output will
    *  reflect the new value on the next UDB clock.  The compare output will be
    *  driven high when the present counter value is less than or less than or
    *  equal to the compare register, depending on the mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmClkADC_WriteCompare1(uint8 compare) \
                                        
    {
        #if(pwmClkADC_UsingFixedFunction)
            CY_SET_REG16(pwmClkADC_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(pwmClkADC_COMPARE1_LSB_PTR, compare);
        #endif /* (pwmClkADC_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: pwmClkADC_WriteCompare2
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare value, for compare1 output.
    *  The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  less than or less than or equal to the compare register, depending on the
    *  mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmClkADC_WriteCompare2(uint8 compare) \
                                        
    {
        #if(pwmClkADC_UsingFixedFunction)
            CY_SET_REG16(pwmClkADC_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG8(pwmClkADC_COMPARE2_LSB_PTR, compare);
        #endif /* (pwmClkADC_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (pwmClkADC_DeadBandUsed)


    /*******************************************************************************
    * Function Name: pwmClkADC_WriteDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function writes the dead-band counts to the corresponding register
    *
    * Parameters:
    *  deadtime:  Number of counts for dead time
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmClkADC_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!pwmClkADC_DeadBand2_4)
            CY_SET_REG8(pwmClkADC_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            pwmClkADC_DEADBAND_COUNT &= ((uint8)(~pwmClkADC_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(pwmClkADC_DEADBAND_COUNT_SHIFT)
                pwmClkADC_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << pwmClkADC_DEADBAND_COUNT_SHIFT)) &
                                                    pwmClkADC_DEADBAND_COUNT_MASK;
            #else
                pwmClkADC_DEADBAND_COUNT |= deadtime & pwmClkADC_DEADBAND_COUNT_MASK;
            #endif /* (pwmClkADC_DEADBAND_COUNT_SHIFT) */

        #endif /* (!pwmClkADC_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function reads the dead-band counts from the corresponding register
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Dead Band Counts
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!pwmClkADC_DeadBand2_4)
            return (CY_GET_REG8(pwmClkADC_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(pwmClkADC_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(pwmClkADC_DEADBAND_COUNT & pwmClkADC_DEADBAND_COUNT_MASK)) >>
                                                                           pwmClkADC_DEADBAND_COUNT_SHIFT));
            #else
                return (pwmClkADC_DEADBAND_COUNT & pwmClkADC_DEADBAND_COUNT_MASK);
            #endif /* (pwmClkADC_DEADBAND_COUNT_SHIFT) */
        #endif /* (!pwmClkADC_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (pwmClkADC_UseStatus || pwmClkADC_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: pwmClkADC_SetInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  This function configures the interrupts mask control of theinterrupt
    *  source status register.
    *
    * Parameters:
    *  uint8 interruptMode: Bit field containing the interrupt sources enabled
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmClkADC_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(pwmClkADC_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadStatusRegister
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current state of the status register.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current status register value. The status register bits are:
    *  [7:6] : Unused(0)
    *  [5]   : Kill event output
    *  [4]   : FIFO not empty
    *  [3]   : FIFO full
    *  [2]   : Terminal count
    *  [1]   : Compare output 2
    *  [0]   : Compare output 1
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(pwmClkADC_STATUS_PTR));
    }

#endif /* (pwmClkADC_UseStatus || pwmClkADC_UsingFixedFunction) */


#if (pwmClkADC_UseControl)


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the control register. This API is available
    *  only if the control register is not removed.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current control register value
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(pwmClkADC_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: pwmClkADC_WriteControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Sets the bit field of the control register. This API is available only if
    *  the control register is not removed.
    *
    * Parameters:
    *  uint8 control: Control register bit field, The status register bits are:
    *  [7]   : PWM Enable
    *  [6]   : Reset
    *  [5:3] : Compare Mode2
    *  [2:0] : Compare Mode2
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmClkADC_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(pwmClkADC_CONTROL_PTR, control);
    }

#endif /* (pwmClkADC_UseControl) */


#if (!pwmClkADC_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadCapture
    ********************************************************************************
    *
    * Summary:
    *  Reads the capture value from the capture FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: The current capture value
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadCapture(void) 
    {
        return (CY_GET_REG8(pwmClkADC_CAPTURE_LSB_PTR));
    }

#endif /* (!pwmClkADC_UsingFixedFunction) */


#if (pwmClkADC_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadCompare
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare output when the PWM Mode parameter is
    *  set to Dither mode, Center Aligned mode, or One Output mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadCompare(void) 
    {
        #if(pwmClkADC_UsingFixedFunction)
            return ((uint8)CY_GET_REG16(pwmClkADC_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG8(pwmClkADC_COMPARE1_LSB_PTR));
        #endif /* (pwmClkADC_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadCompare1
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare1 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadCompare1(void) 
    {
        return (CY_GET_REG8(pwmClkADC_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadCompare2
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare2 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadCompare2(void) 
    {
        return (CY_GET_REG8(pwmClkADC_COMPARE2_LSB_PTR));
    }

#endif /* (pwmClkADC_UseOneCompareMode) */


/*******************************************************************************
* Function Name: pwmClkADC_ReadPeriod
********************************************************************************
*
* Summary:
*  Reads the period value used by the PWM hardware.
*
* Parameters:
*  None
*
* Return:
*  uint8/16: Period value
*
*******************************************************************************/
uint8 pwmClkADC_ReadPeriod(void) 
{
    #if(pwmClkADC_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(pwmClkADC_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(pwmClkADC_PERIOD_LSB_PTR));
    #endif /* (pwmClkADC_UsingFixedFunction) */
}

#if ( pwmClkADC_KillModeMinTime)


    /*******************************************************************************
    * Function Name: pwmClkADC_WriteKillTime
    ********************************************************************************
    *
    * Summary:
    *  Writes the kill time value used by the hardware when the Kill Mode
    *  is set to Minimum Time.
    *
    * Parameters:
    *  uint8: Minimum Time kill counts
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void pwmClkADC_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(pwmClkADC_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: pwmClkADC_ReadKillTime
    ********************************************************************************
    *
    * Summary:
    *  Reads the kill time value used by the hardware when the Kill Mode is set
    *  to Minimum Time.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8: The current Minimum Time kill counts
    *
    *******************************************************************************/
    uint8 pwmClkADC_ReadKillTime(void) 
    {
        return (CY_GET_REG8(pwmClkADC_KILLMODEMINTIME_PTR));
    }

#endif /* ( pwmClkADC_KillModeMinTime) */

/* [] END OF FILE */
