/*******************************************************************************
* File Name: Track_Hold_1.c
* Version 1.40
*
* Description:
*  This file provides the source code to the API for the SAMPLE/TRACK AND HOLD 
*  component.
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

#if (!CY_PSOC5A)
    #if (CYDEV_VARIABLE_VDDA == 1u)
        #include "CyScBoostClk.h"
    #endif /* (CYDEV_VARIABLE_VDDA == 1u) */
#endif /* (!CY_PSOC5A) */

uint8 Track_Hold_1_initVar = 0u;

/* static Track_Hold_1_backupStruct  Track_Hold_1_backup; */
#if (CY_PSOC5A)
    static Track_Hold_1_backupStruct  Track_Hold_1_P5backup;
#endif /* CY_PSOC5A */



/*******************************************************************************   
* Function Name: Track_Hold_1_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the 
*  customizer of the component placed onto schematic. Usually called in 
*  Track_Hold_1_Start().
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Track_Hold_1_Init(void) 
{   
    /* Configure SC Block based on selected Sample/Track type */
    #if (Track_Hold_1_SAMPLE_TRACK_MODE == Track_Hold_1_SAMPLEANDHOLD) /* Sample and hold mode */
        /* SC Block mode -  - SCxx_CR0[3:1] */
        Track_Hold_1_CR0_REG = Track_Hold_1_MODE_SAMPLEANDHOLD;
        
        /* SC Block CR1 */ 
        Track_Hold_1_CR1_REG = (Track_Hold_1_COMP_4P35PF  |
                                Track_Hold_1_GAIN_0DB);
        #if(Track_Hold_1_SAMPLE_CLOCK_EDGE == Track_Hold_1_EDGE_POSITIVENEGATIVE)
            Track_Hold_1_CR1_REG =  Track_Hold_1_CR1_REG  | Track_Hold_1_DIV2_DISABLE ;
        #else
            Track_Hold_1_CR1_REG =  Track_Hold_1_CR1_REG  | Track_Hold_1_DIV2_ENABLE ;
        #endif
         
        #if(Track_Hold_1_VREF_TYPE == Track_Hold_1_EXTERNAL)
            Track_Hold_1_CR2_REG = Track_Hold_1_BIAS_LOW |
                                       Track_Hold_1_REDC_00 | 
                                       Track_Hold_1_GNDVREF_DI;
        #else
            Track_Hold_1_CR2_REG = Track_Hold_1_BIAS_LOW |
                                       Track_Hold_1_REDC_00 | 
                                       Track_Hold_1_GNDVREF_E;
        #endif
    #else
        /* Track and Hold Mode */
        /* SC Block mode -  - SCxx_CR0[3:1] */
        Track_Hold_1_CR0_REG = Track_Hold_1_MODE_TRACKANDHOLD; 
        
        /* SC Block CR1 */ 
        Track_Hold_1_CR1_REG = Track_Hold_1_COMP_4P35PF  |
                                   Track_Hold_1_DIV2_ENABLE |
                                   Track_Hold_1_GAIN_0DB;
                                
        /* SC Block CR2 */
        Track_Hold_1_CR2_REG = Track_Hold_1_BIAS_LOW |
                                   Track_Hold_1_REDC_00 |
                                   Track_Hold_1_GNDVREF_E;
    #endif /* end if - Sample/Track Type */
    
    /* Enable SC Block clocking */
    Track_Hold_1_CLK_REG |= Track_Hold_1_CLK_EN;
    
    /* Set default power */
    Track_Hold_1_SetPower(Track_Hold_1_INIT_POWER);
}


/*******************************************************************************   
* Function Name: Track_Hold_1_Enable
********************************************************************************
*
* Summary:
*  Enables the Sample/Track and Hold block operation
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Track_Hold_1_Enable(void) 
{
    /* This is to restore the value of register CR1 and CR2 which is saved 
      in prior to the modifications in stop() API */
    #if (CY_PSOC5A)
        if(Track_Hold_1_P5backup.enableState == 1u)
        {
            Track_Hold_1_CR1_REG = Track_Hold_1_P5backup.scCR1Reg;
            Track_Hold_1_CR2_REG = Track_Hold_1_P5backup.scCR2Reg;
            Track_Hold_1_P5backup.enableState = 0u;
        }
    #endif /* CY_PSOC5A */
    
    /* Enable power to SC block in active mode */
    Track_Hold_1_PM_ACT_CFG_REG |= Track_Hold_1_ACT_PWR_EN;
    
    /* Enable power to SC block in Alternative active mode */
    Track_Hold_1_PM_STBY_CFG_REG |= Track_Hold_1_STBY_PWR_EN;

    Track_Hold_1_PUMP_CR1_REG |= Track_Hold_1_PUMP_CR1_SC_CLKSEL;
    
    #if (!CY_PSOC5A)
        #if (CYDEV_VARIABLE_VDDA == 1u)
            if(CyScPumpEnabled == 1u)
            {
                Track_Hold_1_BSTCLK_REG &= (uint8)(~Track_Hold_1_BST_CLK_INDEX_MASK);
                Track_Hold_1_BSTCLK_REG |= Track_Hold_1_BST_CLK_EN | CyScBoostClk__INDEX;
                Track_Hold_1_SC_MISC_REG |= Track_Hold_1_PUMP_FORCE;
                CyScBoostClk_Start();
            }
            else
            {
                Track_Hold_1_BSTCLK_REG &= (uint8)(~Track_Hold_1_BST_CLK_EN);
                Track_Hold_1_SC_MISC_REG &= (uint8)(~Track_Hold_1_PUMP_FORCE);
            }
        #endif /* (CYDEV_VARIABLE_VDDA == 1u) */
    #endif /* (!CY_PSOC5A) */
}


/*******************************************************************************
* Function Name: Track_Hold_1_Start
********************************************************************************
*
* Summary:
*  The start function initializes the Sample and Hold with the default values, 
*  and sets the power to the given level.  A power level of 0, is the same as 
*  executing the stop function.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Track_Hold_1_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(Track_Hold_1_initVar == 0u)
    {
        Track_Hold_1_Init();
        Track_Hold_1_initVar = 1u;
    }
    Track_Hold_1_Enable();
}


/*******************************************************************************
* Function Name: Track_Hold_1_Stop
********************************************************************************
*
* Summary:
*  Powers down amplifier to lowest power state.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Track_Hold_1_Stop(void) 
{   
    /* Disble power to the Amp in Active mode template */
    Track_Hold_1_PM_ACT_CFG_REG &= (uint8)(~Track_Hold_1_ACT_PWR_EN);

    /* Disble power to the Amp in Alternative Active mode template */
    Track_Hold_1_PM_STBY_CFG_REG &= (uint8)(~Track_Hold_1_STBY_PWR_EN);
    
    #if (!CY_PSOC5A)
        #if (CYDEV_VARIABLE_VDDA == 1u)
            Track_Hold_1_BSTCLK_REG &= (uint8)(~Track_Hold_1_BST_CLK_EN);
            /* Disable pumps only if there aren't any SC block in use */
            if ((Track_Hold_1_PM_ACT_CFG_REG & Track_Hold_1_PM_ACT_CFG_MASK) == 0u)
            {
                Track_Hold_1_SC_MISC_REG &= (uint8)(~Track_Hold_1_PUMP_FORCE);
                Track_Hold_1_PUMP_CR1_REG &= (uint8)(~Track_Hold_1_PUMP_CR1_SC_CLKSEL);
                CyScBoostClk_Stop();
            }
        #endif /* CYDEV_VARIABLE_VDDA == 1u */
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
 
    /* This sets Sample and hold in zero current mode and output routes are valid */
    #if (CY_PSOC5A)
        Track_Hold_1_P5backup.scCR1Reg = Track_Hold_1_CR1_REG;
        Track_Hold_1_P5backup.scCR2Reg = Track_Hold_1_CR2_REG;
        Track_Hold_1_CR1_REG = 0x00u;
        Track_Hold_1_CR2_REG = 0x00u;
        Track_Hold_1_P5backup.enableState = 1u;
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: Track_Hold_1_SetPower
********************************************************************************
*
* Summary:
*  Set the power of the Sample/Track and Hold.
*
* Parameters:
*  power: Sets power level between (0) and (3) high power
*
* Return:
*  void
*
*******************************************************************************/
void Track_Hold_1_SetPower(uint8 power) 
{
    uint8 tmpCR;

    /* Sets drive bits in SC Block Control Register 1:  SCxx_CR[1:0] */
    tmpCR = Track_Hold_1_CR1_REG & (uint8)(~Track_Hold_1_DRIVE_MASK);
    tmpCR |= (power & Track_Hold_1_DRIVE_MASK);
    Track_Hold_1_CR1_REG = tmpCR;
}


/* [] END OF FILE */
