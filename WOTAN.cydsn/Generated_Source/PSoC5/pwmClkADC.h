/*******************************************************************************
* File Name: pwmClkADC.h
* Version 3.30
*
* Description:
*  Contains the prototypes and constants for the functions available to the
*  PWM user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_PWM_pwmClkADC_H)
#define CY_PWM_pwmClkADC_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 pwmClkADC_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define pwmClkADC_Resolution                     (8u)
#define pwmClkADC_UsingFixedFunction             (1u)
#define pwmClkADC_DeadBandMode                   (0u)
#define pwmClkADC_KillModeMinTime                (0u)
#define pwmClkADC_KillMode                       (1u)
#define pwmClkADC_PWMMode                        (0u)
#define pwmClkADC_PWMModeIsCenterAligned         (0u)
#define pwmClkADC_DeadBandUsed                   (0u)
#define pwmClkADC_DeadBand2_4                    (0u)

#if !defined(pwmClkADC_PWMUDB_genblk8_stsreg__REMOVED)
    #define pwmClkADC_UseStatus                  (1u)
#else
    #define pwmClkADC_UseStatus                  (0u)
#endif /* !defined(pwmClkADC_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(pwmClkADC_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define pwmClkADC_UseControl                 (1u)
#else
    #define pwmClkADC_UseControl                 (0u)
#endif /* !defined(pwmClkADC_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define pwmClkADC_UseOneCompareMode              (1u)
#define pwmClkADC_MinimumKillTime                (1u)
#define pwmClkADC_EnableMode                     (0u)

#define pwmClkADC_CompareMode1SW                 (0u)
#define pwmClkADC_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define pwmClkADC__B_PWM__DISABLED 0
#define pwmClkADC__B_PWM__ASYNCHRONOUS 1
#define pwmClkADC__B_PWM__SINGLECYCLE 2
#define pwmClkADC__B_PWM__LATCHED 3
#define pwmClkADC__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define pwmClkADC__B_PWM__DBMDISABLED 0
#define pwmClkADC__B_PWM__DBM_2_4_CLOCKS 1
#define pwmClkADC__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define pwmClkADC__B_PWM__ONE_OUTPUT 0
#define pwmClkADC__B_PWM__TWO_OUTPUTS 1
#define pwmClkADC__B_PWM__DUAL_EDGE 2
#define pwmClkADC__B_PWM__CENTER_ALIGN 3
#define pwmClkADC__B_PWM__DITHER 5
#define pwmClkADC__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define pwmClkADC__B_PWM__LESS_THAN 1
#define pwmClkADC__B_PWM__LESS_THAN_OR_EQUAL 2
#define pwmClkADC__B_PWM__GREATER_THAN 3
#define pwmClkADC__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define pwmClkADC__B_PWM__EQUAL 0
#define pwmClkADC__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!pwmClkADC_UsingFixedFunction)
        uint8 PWMUdb;               /* PWM Current Counter value  */
        #if(!pwmClkADC_PWMModeIsCenterAligned)
            uint8 PWMPeriod;
        #endif /* (!pwmClkADC_PWMModeIsCenterAligned) */
        #if (pwmClkADC_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (pwmClkADC_UseStatus) */

        /* Backup for Deadband parameters */
        #if(pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_256_CLOCKS || \
            pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(pwmClkADC_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (pwmClkADC_KillModeMinTime) */

        /* Backup control register */
        #if(pwmClkADC_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (pwmClkADC_UseControl) */

    #endif /* (!pwmClkADC_UsingFixedFunction) */

}pwmClkADC_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    pwmClkADC_Start(void) ;
void    pwmClkADC_Stop(void) ;

#if (pwmClkADC_UseStatus || pwmClkADC_UsingFixedFunction)
    void  pwmClkADC_SetInterruptMode(uint8 interruptMode) ;
    uint8 pwmClkADC_ReadStatusRegister(void) ;
#endif /* (pwmClkADC_UseStatus || pwmClkADC_UsingFixedFunction) */

#define pwmClkADC_GetInterruptSource() pwmClkADC_ReadStatusRegister()

#if (pwmClkADC_UseControl)
    uint8 pwmClkADC_ReadControlRegister(void) ;
    void  pwmClkADC_WriteControlRegister(uint8 control)
          ;
#endif /* (pwmClkADC_UseControl) */

#if (pwmClkADC_UseOneCompareMode)
   #if (pwmClkADC_CompareMode1SW)
       void    pwmClkADC_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (pwmClkADC_CompareMode1SW) */
#else
    #if (pwmClkADC_CompareMode1SW)
        void    pwmClkADC_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (pwmClkADC_CompareMode1SW) */
    #if (pwmClkADC_CompareMode2SW)
        void    pwmClkADC_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (pwmClkADC_CompareMode2SW) */
#endif /* (pwmClkADC_UseOneCompareMode) */

#if (!pwmClkADC_UsingFixedFunction)
    uint8   pwmClkADC_ReadCounter(void) ;
    uint8 pwmClkADC_ReadCapture(void) ;

    #if (pwmClkADC_UseStatus)
            void pwmClkADC_ClearFIFO(void) ;
    #endif /* (pwmClkADC_UseStatus) */

    void    pwmClkADC_WriteCounter(uint8 counter)
            ;
#endif /* (!pwmClkADC_UsingFixedFunction) */

void    pwmClkADC_WritePeriod(uint8 period)
        ;
uint8 pwmClkADC_ReadPeriod(void) ;

#if (pwmClkADC_UseOneCompareMode)
    void    pwmClkADC_WriteCompare(uint8 compare)
            ;
    uint8 pwmClkADC_ReadCompare(void) ;
#else
    void    pwmClkADC_WriteCompare1(uint8 compare)
            ;
    uint8 pwmClkADC_ReadCompare1(void) ;
    void    pwmClkADC_WriteCompare2(uint8 compare)
            ;
    uint8 pwmClkADC_ReadCompare2(void) ;
#endif /* (pwmClkADC_UseOneCompareMode) */


#if (pwmClkADC_DeadBandUsed)
    void    pwmClkADC_WriteDeadTime(uint8 deadtime) ;
    uint8   pwmClkADC_ReadDeadTime(void) ;
#endif /* (pwmClkADC_DeadBandUsed) */

#if ( pwmClkADC_KillModeMinTime)
    void pwmClkADC_WriteKillTime(uint8 killtime) ;
    uint8 pwmClkADC_ReadKillTime(void) ;
#endif /* ( pwmClkADC_KillModeMinTime) */

void pwmClkADC_Init(void) ;
void pwmClkADC_Enable(void) ;
void pwmClkADC_Sleep(void) ;
void pwmClkADC_Wakeup(void) ;
void pwmClkADC_SaveConfig(void) ;
void pwmClkADC_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define pwmClkADC_INIT_PERIOD_VALUE          (3u)
#define pwmClkADC_INIT_COMPARE_VALUE1        (2u)
#define pwmClkADC_INIT_COMPARE_VALUE2        (63u)
#define pwmClkADC_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    pwmClkADC_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    pwmClkADC_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    pwmClkADC_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    pwmClkADC_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define pwmClkADC_DEFAULT_COMPARE2_MODE      (uint8)((uint8)1u <<  pwmClkADC_CTRL_CMPMODE2_SHIFT)
#define pwmClkADC_DEFAULT_COMPARE1_MODE      (uint8)((uint8)1u <<  pwmClkADC_CTRL_CMPMODE1_SHIFT)
#define pwmClkADC_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (pwmClkADC_UsingFixedFunction)
   #define pwmClkADC_PERIOD_LSB              (*(reg16 *) pwmClkADC_PWMHW__PER0)
   #define pwmClkADC_PERIOD_LSB_PTR          ( (reg16 *) pwmClkADC_PWMHW__PER0)
   #define pwmClkADC_COMPARE1_LSB            (*(reg16 *) pwmClkADC_PWMHW__CNT_CMP0)
   #define pwmClkADC_COMPARE1_LSB_PTR        ( (reg16 *) pwmClkADC_PWMHW__CNT_CMP0)
   #define pwmClkADC_COMPARE2_LSB            (0x00u)
   #define pwmClkADC_COMPARE2_LSB_PTR        (0x00u)
   #define pwmClkADC_COUNTER_LSB             (*(reg16 *) pwmClkADC_PWMHW__CNT_CMP0)
   #define pwmClkADC_COUNTER_LSB_PTR         ( (reg16 *) pwmClkADC_PWMHW__CNT_CMP0)
   #define pwmClkADC_CAPTURE_LSB             (*(reg16 *) pwmClkADC_PWMHW__CAP0)
   #define pwmClkADC_CAPTURE_LSB_PTR         ( (reg16 *) pwmClkADC_PWMHW__CAP0)
   #define pwmClkADC_RT1                     (*(reg8 *)  pwmClkADC_PWMHW__RT1)
   #define pwmClkADC_RT1_PTR                 ( (reg8 *)  pwmClkADC_PWMHW__RT1)

#else
   #if (pwmClkADC_Resolution == 8u) /* 8bit - PWM */

       #if(pwmClkADC_PWMModeIsCenterAligned)
           #define pwmClkADC_PERIOD_LSB      (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__D1_REG)
           #define pwmClkADC_PERIOD_LSB_PTR  ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #else
           #define pwmClkADC_PERIOD_LSB      (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__F0_REG)
           #define pwmClkADC_PERIOD_LSB_PTR  ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u0__F0_REG)
       #endif /* (pwmClkADC_PWMModeIsCenterAligned) */

       #define pwmClkADC_COMPARE1_LSB        (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define pwmClkADC_COMPARE1_LSB_PTR    ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define pwmClkADC_COMPARE2_LSB        (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define pwmClkADC_COMPARE2_LSB_PTR    ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define pwmClkADC_COUNTERCAP_LSB      (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define pwmClkADC_COUNTERCAP_LSB_PTR  ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define pwmClkADC_COUNTER_LSB         (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define pwmClkADC_COUNTER_LSB_PTR     ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define pwmClkADC_CAPTURE_LSB         (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__F1_REG)
       #define pwmClkADC_CAPTURE_LSB_PTR     ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(pwmClkADC_PWMModeIsCenterAligned)
               #define pwmClkADC_PERIOD_LSB      (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__D1_REG)
               #define pwmClkADC_PERIOD_LSB_PTR  ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #else
               #define pwmClkADC_PERIOD_LSB      (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__F0_REG)
               #define pwmClkADC_PERIOD_LSB_PTR  ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__F0_REG)
            #endif /* (pwmClkADC_PWMModeIsCenterAligned) */

            #define pwmClkADC_COMPARE1_LSB       (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define pwmClkADC_COMPARE1_LSB_PTR   ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define pwmClkADC_COMPARE2_LSB       (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define pwmClkADC_COMPARE2_LSB_PTR   ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define pwmClkADC_COUNTERCAP_LSB     (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define pwmClkADC_COUNTERCAP_LSB_PTR ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define pwmClkADC_COUNTER_LSB        (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define pwmClkADC_COUNTER_LSB_PTR    ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define pwmClkADC_CAPTURE_LSB        (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__F1_REG)
            #define pwmClkADC_CAPTURE_LSB_PTR    ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__F1_REG)
        #else
            #if(pwmClkADC_PWMModeIsCenterAligned)
               #define pwmClkADC_PERIOD_LSB      (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
               #define pwmClkADC_PERIOD_LSB_PTR  ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #else
               #define pwmClkADC_PERIOD_LSB      (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
               #define pwmClkADC_PERIOD_LSB_PTR  ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
            #endif /* (pwmClkADC_PWMModeIsCenterAligned) */

            #define pwmClkADC_COMPARE1_LSB       (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define pwmClkADC_COMPARE1_LSB_PTR   ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define pwmClkADC_COMPARE2_LSB       (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define pwmClkADC_COMPARE2_LSB_PTR   ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define pwmClkADC_COUNTERCAP_LSB     (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define pwmClkADC_COUNTERCAP_LSB_PTR ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define pwmClkADC_COUNTER_LSB        (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define pwmClkADC_COUNTER_LSB_PTR    ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define pwmClkADC_CAPTURE_LSB        (*(reg16 *) pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
            #define pwmClkADC_CAPTURE_LSB_PTR    ((reg16 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define pwmClkADC_AUX_CONTROLDP1          (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)
       #define pwmClkADC_AUX_CONTROLDP1_PTR      ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (pwmClkADC_Resolution == 8) */

   #define pwmClkADC_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__A1_REG)
   #define pwmClkADC_AUX_CONTROLDP0          (*(reg8 *)  pwmClkADC_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)
   #define pwmClkADC_AUX_CONTROLDP0_PTR      ((reg8 *)   pwmClkADC_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (pwmClkADC_UsingFixedFunction) */

#if(pwmClkADC_KillModeMinTime )
    #define pwmClkADC_KILLMODEMINTIME        (*(reg8 *)  pwmClkADC_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define pwmClkADC_KILLMODEMINTIME_PTR    ((reg8 *)   pwmClkADC_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (pwmClkADC_KillModeMinTime ) */

#if(pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_256_CLOCKS)
    #define pwmClkADC_DEADBAND_COUNT         (*(reg8 *)  pwmClkADC_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define pwmClkADC_DEADBAND_COUNT_PTR     ((reg8 *)   pwmClkADC_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define pwmClkADC_DEADBAND_LSB_PTR       ((reg8 *)   pwmClkADC_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define pwmClkADC_DEADBAND_LSB           (*(reg8 *)  pwmClkADC_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (pwmClkADC_UsingFixedFunction)
        #define pwmClkADC_DEADBAND_COUNT         (*(reg8 *)  pwmClkADC_PWMHW__CFG0)
        #define pwmClkADC_DEADBAND_COUNT_PTR     ((reg8 *)   pwmClkADC_PWMHW__CFG0)
        #define pwmClkADC_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << pwmClkADC_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define pwmClkADC_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define pwmClkADC_DEADBAND_COUNT         (*(reg8 *)  pwmClkADC_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define pwmClkADC_DEADBAND_COUNT_PTR     ((reg8 *)   pwmClkADC_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define pwmClkADC_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << pwmClkADC_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define pwmClkADC_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (pwmClkADC_UsingFixedFunction) */
#endif /* (pwmClkADC_DeadBandMode == pwmClkADC__B_PWM__DBM_256_CLOCKS) */



#if (pwmClkADC_UsingFixedFunction)
    #define pwmClkADC_STATUS                 (*(reg8 *) pwmClkADC_PWMHW__SR0)
    #define pwmClkADC_STATUS_PTR             ((reg8 *) pwmClkADC_PWMHW__SR0)
    #define pwmClkADC_STATUS_MASK            (*(reg8 *) pwmClkADC_PWMHW__SR0)
    #define pwmClkADC_STATUS_MASK_PTR        ((reg8 *) pwmClkADC_PWMHW__SR0)
    #define pwmClkADC_CONTROL                (*(reg8 *) pwmClkADC_PWMHW__CFG0)
    #define pwmClkADC_CONTROL_PTR            ((reg8 *) pwmClkADC_PWMHW__CFG0)
    #define pwmClkADC_CONTROL2               (*(reg8 *) pwmClkADC_PWMHW__CFG1)
    #define pwmClkADC_CONTROL3               (*(reg8 *) pwmClkADC_PWMHW__CFG2)
    #define pwmClkADC_GLOBAL_ENABLE          (*(reg8 *) pwmClkADC_PWMHW__PM_ACT_CFG)
    #define pwmClkADC_GLOBAL_ENABLE_PTR      ( (reg8 *) pwmClkADC_PWMHW__PM_ACT_CFG)
    #define pwmClkADC_GLOBAL_STBY_ENABLE     (*(reg8 *) pwmClkADC_PWMHW__PM_STBY_CFG)
    #define pwmClkADC_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) pwmClkADC_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define pwmClkADC_BLOCK_EN_MASK          (pwmClkADC_PWMHW__PM_ACT_MSK)
    #define pwmClkADC_BLOCK_STBY_EN_MASK     (pwmClkADC_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define pwmClkADC_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define pwmClkADC_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define pwmClkADC_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define pwmClkADC_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define pwmClkADC_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define pwmClkADC_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define pwmClkADC_CTRL_ENABLE            (uint8)((uint8)0x01u << pwmClkADC_CTRL_ENABLE_SHIFT)
    #define pwmClkADC_CTRL_RESET             (uint8)((uint8)0x01u << pwmClkADC_CTRL_RESET_SHIFT)
    #define pwmClkADC_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << pwmClkADC_CTRL_CMPMODE2_SHIFT)
    #define pwmClkADC_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << pwmClkADC_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define pwmClkADC_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define pwmClkADC_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << pwmClkADC_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define pwmClkADC_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define pwmClkADC_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define pwmClkADC_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define pwmClkADC_STATUS_TC_INT_EN_MASK_SHIFT            (pwmClkADC_STATUS_TC_SHIFT - 4u)
    #define pwmClkADC_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define pwmClkADC_STATUS_CMP1_INT_EN_MASK_SHIFT          (pwmClkADC_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define pwmClkADC_STATUS_TC              (uint8)((uint8)0x01u << pwmClkADC_STATUS_TC_SHIFT)
    #define pwmClkADC_STATUS_CMP1            (uint8)((uint8)0x01u << pwmClkADC_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define pwmClkADC_STATUS_TC_INT_EN_MASK              (uint8)((uint8)pwmClkADC_STATUS_TC >> 4u)
    #define pwmClkADC_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)pwmClkADC_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define pwmClkADC_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define pwmClkADC_RT1_MASK              (uint8)((uint8)0x03u << pwmClkADC_RT1_SHIFT)
    #define pwmClkADC_SYNC                  (uint8)((uint8)0x03u << pwmClkADC_RT1_SHIFT)
    #define pwmClkADC_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define pwmClkADC_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << pwmClkADC_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define pwmClkADC_SYNCDSI_EN            (uint8)((uint8)0x0Fu << pwmClkADC_SYNCDSI_SHIFT)


#else
    #define pwmClkADC_STATUS                (*(reg8 *)   pwmClkADC_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define pwmClkADC_STATUS_PTR            ((reg8 *)    pwmClkADC_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define pwmClkADC_STATUS_MASK           (*(reg8 *)   pwmClkADC_PWMUDB_genblk8_stsreg__MASK_REG)
    #define pwmClkADC_STATUS_MASK_PTR       ((reg8 *)    pwmClkADC_PWMUDB_genblk8_stsreg__MASK_REG)
    #define pwmClkADC_STATUS_AUX_CTRL       (*(reg8 *)   pwmClkADC_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define pwmClkADC_CONTROL               (*(reg8 *)   pwmClkADC_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define pwmClkADC_CONTROL_PTR           ((reg8 *)    pwmClkADC_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define pwmClkADC_CTRL_ENABLE_SHIFT      (0x07u)
    #define pwmClkADC_CTRL_RESET_SHIFT       (0x06u)
    #define pwmClkADC_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define pwmClkADC_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define pwmClkADC_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define pwmClkADC_CTRL_ENABLE            (uint8)((uint8)0x01u << pwmClkADC_CTRL_ENABLE_SHIFT)
    #define pwmClkADC_CTRL_RESET             (uint8)((uint8)0x01u << pwmClkADC_CTRL_RESET_SHIFT)
    #define pwmClkADC_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << pwmClkADC_CTRL_CMPMODE2_SHIFT)
    #define pwmClkADC_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << pwmClkADC_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define pwmClkADC_STATUS_KILL_SHIFT          (0x05u)
    #define pwmClkADC_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define pwmClkADC_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define pwmClkADC_STATUS_TC_SHIFT            (0x02u)
    #define pwmClkADC_STATUS_CMP2_SHIFT          (0x01u)
    #define pwmClkADC_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define pwmClkADC_STATUS_KILL_INT_EN_MASK_SHIFT          (pwmClkADC_STATUS_KILL_SHIFT)
    #define pwmClkADC_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (pwmClkADC_STATUS_FIFONEMPTY_SHIFT)
    #define pwmClkADC_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (pwmClkADC_STATUS_FIFOFULL_SHIFT)
    #define pwmClkADC_STATUS_TC_INT_EN_MASK_SHIFT            (pwmClkADC_STATUS_TC_SHIFT)
    #define pwmClkADC_STATUS_CMP2_INT_EN_MASK_SHIFT          (pwmClkADC_STATUS_CMP2_SHIFT)
    #define pwmClkADC_STATUS_CMP1_INT_EN_MASK_SHIFT          (pwmClkADC_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define pwmClkADC_STATUS_KILL            (uint8)((uint8)0x00u << pwmClkADC_STATUS_KILL_SHIFT )
    #define pwmClkADC_STATUS_FIFOFULL        (uint8)((uint8)0x01u << pwmClkADC_STATUS_FIFOFULL_SHIFT)
    #define pwmClkADC_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << pwmClkADC_STATUS_FIFONEMPTY_SHIFT)
    #define pwmClkADC_STATUS_TC              (uint8)((uint8)0x01u << pwmClkADC_STATUS_TC_SHIFT)
    #define pwmClkADC_STATUS_CMP2            (uint8)((uint8)0x01u << pwmClkADC_STATUS_CMP2_SHIFT)
    #define pwmClkADC_STATUS_CMP1            (uint8)((uint8)0x01u << pwmClkADC_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define pwmClkADC_STATUS_KILL_INT_EN_MASK            (pwmClkADC_STATUS_KILL)
    #define pwmClkADC_STATUS_FIFOFULL_INT_EN_MASK        (pwmClkADC_STATUS_FIFOFULL)
    #define pwmClkADC_STATUS_FIFONEMPTY_INT_EN_MASK      (pwmClkADC_STATUS_FIFONEMPTY)
    #define pwmClkADC_STATUS_TC_INT_EN_MASK              (pwmClkADC_STATUS_TC)
    #define pwmClkADC_STATUS_CMP2_INT_EN_MASK            (pwmClkADC_STATUS_CMP2)
    #define pwmClkADC_STATUS_CMP1_INT_EN_MASK            (pwmClkADC_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define pwmClkADC_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define pwmClkADC_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define pwmClkADC_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define pwmClkADC_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define pwmClkADC_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* pwmClkADC_UsingFixedFunction */

#endif  /* CY_PWM_pwmClkADC_H */


/* [] END OF FILE */
