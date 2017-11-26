/*******************************************************************************
* File Name: pwmSAMPLING.h
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

#if !defined(CY_PWM_pwmSAMPLING_H)
#define CY_PWM_pwmSAMPLING_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 pwmSAMPLING_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define pwmSAMPLING_Resolution                     (8u)
#define pwmSAMPLING_UsingFixedFunction             (1u)
#define pwmSAMPLING_DeadBandMode                   (0u)
#define pwmSAMPLING_KillModeMinTime                (0u)
#define pwmSAMPLING_KillMode                       (1u)
#define pwmSAMPLING_PWMMode                        (0u)
#define pwmSAMPLING_PWMModeIsCenterAligned         (0u)
#define pwmSAMPLING_DeadBandUsed                   (0u)
#define pwmSAMPLING_DeadBand2_4                    (0u)

#if !defined(pwmSAMPLING_PWMUDB_genblk8_stsreg__REMOVED)
    #define pwmSAMPLING_UseStatus                  (1u)
#else
    #define pwmSAMPLING_UseStatus                  (0u)
#endif /* !defined(pwmSAMPLING_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(pwmSAMPLING_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define pwmSAMPLING_UseControl                 (1u)
#else
    #define pwmSAMPLING_UseControl                 (0u)
#endif /* !defined(pwmSAMPLING_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define pwmSAMPLING_UseOneCompareMode              (1u)
#define pwmSAMPLING_MinimumKillTime                (1u)
#define pwmSAMPLING_EnableMode                     (0u)

#define pwmSAMPLING_CompareMode1SW                 (0u)
#define pwmSAMPLING_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define pwmSAMPLING__B_PWM__DISABLED 0
#define pwmSAMPLING__B_PWM__ASYNCHRONOUS 1
#define pwmSAMPLING__B_PWM__SINGLECYCLE 2
#define pwmSAMPLING__B_PWM__LATCHED 3
#define pwmSAMPLING__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define pwmSAMPLING__B_PWM__DBMDISABLED 0
#define pwmSAMPLING__B_PWM__DBM_2_4_CLOCKS 1
#define pwmSAMPLING__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define pwmSAMPLING__B_PWM__ONE_OUTPUT 0
#define pwmSAMPLING__B_PWM__TWO_OUTPUTS 1
#define pwmSAMPLING__B_PWM__DUAL_EDGE 2
#define pwmSAMPLING__B_PWM__CENTER_ALIGN 3
#define pwmSAMPLING__B_PWM__DITHER 5
#define pwmSAMPLING__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define pwmSAMPLING__B_PWM__LESS_THAN 1
#define pwmSAMPLING__B_PWM__LESS_THAN_OR_EQUAL 2
#define pwmSAMPLING__B_PWM__GREATER_THAN 3
#define pwmSAMPLING__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define pwmSAMPLING__B_PWM__EQUAL 0
#define pwmSAMPLING__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!pwmSAMPLING_UsingFixedFunction)
        uint8 PWMUdb;               /* PWM Current Counter value  */
        #if(!pwmSAMPLING_PWMModeIsCenterAligned)
            uint8 PWMPeriod;
        #endif /* (!pwmSAMPLING_PWMModeIsCenterAligned) */
        #if (pwmSAMPLING_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (pwmSAMPLING_UseStatus) */

        /* Backup for Deadband parameters */
        #if(pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_256_CLOCKS || \
            pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(pwmSAMPLING_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (pwmSAMPLING_KillModeMinTime) */

        /* Backup control register */
        #if(pwmSAMPLING_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (pwmSAMPLING_UseControl) */

    #endif /* (!pwmSAMPLING_UsingFixedFunction) */

}pwmSAMPLING_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    pwmSAMPLING_Start(void) ;
void    pwmSAMPLING_Stop(void) ;

#if (pwmSAMPLING_UseStatus || pwmSAMPLING_UsingFixedFunction)
    void  pwmSAMPLING_SetInterruptMode(uint8 interruptMode) ;
    uint8 pwmSAMPLING_ReadStatusRegister(void) ;
#endif /* (pwmSAMPLING_UseStatus || pwmSAMPLING_UsingFixedFunction) */

#define pwmSAMPLING_GetInterruptSource() pwmSAMPLING_ReadStatusRegister()

#if (pwmSAMPLING_UseControl)
    uint8 pwmSAMPLING_ReadControlRegister(void) ;
    void  pwmSAMPLING_WriteControlRegister(uint8 control)
          ;
#endif /* (pwmSAMPLING_UseControl) */

#if (pwmSAMPLING_UseOneCompareMode)
   #if (pwmSAMPLING_CompareMode1SW)
       void    pwmSAMPLING_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (pwmSAMPLING_CompareMode1SW) */
#else
    #if (pwmSAMPLING_CompareMode1SW)
        void    pwmSAMPLING_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (pwmSAMPLING_CompareMode1SW) */
    #if (pwmSAMPLING_CompareMode2SW)
        void    pwmSAMPLING_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (pwmSAMPLING_CompareMode2SW) */
#endif /* (pwmSAMPLING_UseOneCompareMode) */

#if (!pwmSAMPLING_UsingFixedFunction)
    uint8   pwmSAMPLING_ReadCounter(void) ;
    uint8 pwmSAMPLING_ReadCapture(void) ;

    #if (pwmSAMPLING_UseStatus)
            void pwmSAMPLING_ClearFIFO(void) ;
    #endif /* (pwmSAMPLING_UseStatus) */

    void    pwmSAMPLING_WriteCounter(uint8 counter)
            ;
#endif /* (!pwmSAMPLING_UsingFixedFunction) */

void    pwmSAMPLING_WritePeriod(uint8 period)
        ;
uint8 pwmSAMPLING_ReadPeriod(void) ;

#if (pwmSAMPLING_UseOneCompareMode)
    void    pwmSAMPLING_WriteCompare(uint8 compare)
            ;
    uint8 pwmSAMPLING_ReadCompare(void) ;
#else
    void    pwmSAMPLING_WriteCompare1(uint8 compare)
            ;
    uint8 pwmSAMPLING_ReadCompare1(void) ;
    void    pwmSAMPLING_WriteCompare2(uint8 compare)
            ;
    uint8 pwmSAMPLING_ReadCompare2(void) ;
#endif /* (pwmSAMPLING_UseOneCompareMode) */


#if (pwmSAMPLING_DeadBandUsed)
    void    pwmSAMPLING_WriteDeadTime(uint8 deadtime) ;
    uint8   pwmSAMPLING_ReadDeadTime(void) ;
#endif /* (pwmSAMPLING_DeadBandUsed) */

#if ( pwmSAMPLING_KillModeMinTime)
    void pwmSAMPLING_WriteKillTime(uint8 killtime) ;
    uint8 pwmSAMPLING_ReadKillTime(void) ;
#endif /* ( pwmSAMPLING_KillModeMinTime) */

void pwmSAMPLING_Init(void) ;
void pwmSAMPLING_Enable(void) ;
void pwmSAMPLING_Sleep(void) ;
void pwmSAMPLING_Wakeup(void) ;
void pwmSAMPLING_SaveConfig(void) ;
void pwmSAMPLING_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define pwmSAMPLING_INIT_PERIOD_VALUE          (71u)
#define pwmSAMPLING_INIT_COMPARE_VALUE1        (36u)
#define pwmSAMPLING_INIT_COMPARE_VALUE2        (63u)
#define pwmSAMPLING_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    pwmSAMPLING_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    pwmSAMPLING_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    pwmSAMPLING_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    pwmSAMPLING_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define pwmSAMPLING_DEFAULT_COMPARE2_MODE      (uint8)((uint8)1u <<  pwmSAMPLING_CTRL_CMPMODE2_SHIFT)
#define pwmSAMPLING_DEFAULT_COMPARE1_MODE      (uint8)((uint8)1u <<  pwmSAMPLING_CTRL_CMPMODE1_SHIFT)
#define pwmSAMPLING_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (pwmSAMPLING_UsingFixedFunction)
   #define pwmSAMPLING_PERIOD_LSB              (*(reg16 *) pwmSAMPLING_PWMHW__PER0)
   #define pwmSAMPLING_PERIOD_LSB_PTR          ( (reg16 *) pwmSAMPLING_PWMHW__PER0)
   #define pwmSAMPLING_COMPARE1_LSB            (*(reg16 *) pwmSAMPLING_PWMHW__CNT_CMP0)
   #define pwmSAMPLING_COMPARE1_LSB_PTR        ( (reg16 *) pwmSAMPLING_PWMHW__CNT_CMP0)
   #define pwmSAMPLING_COMPARE2_LSB            (0x00u)
   #define pwmSAMPLING_COMPARE2_LSB_PTR        (0x00u)
   #define pwmSAMPLING_COUNTER_LSB             (*(reg16 *) pwmSAMPLING_PWMHW__CNT_CMP0)
   #define pwmSAMPLING_COUNTER_LSB_PTR         ( (reg16 *) pwmSAMPLING_PWMHW__CNT_CMP0)
   #define pwmSAMPLING_CAPTURE_LSB             (*(reg16 *) pwmSAMPLING_PWMHW__CAP0)
   #define pwmSAMPLING_CAPTURE_LSB_PTR         ( (reg16 *) pwmSAMPLING_PWMHW__CAP0)
   #define pwmSAMPLING_RT1                     (*(reg8 *)  pwmSAMPLING_PWMHW__RT1)
   #define pwmSAMPLING_RT1_PTR                 ( (reg8 *)  pwmSAMPLING_PWMHW__RT1)

#else
   #if (pwmSAMPLING_Resolution == 8u) /* 8bit - PWM */

       #if(pwmSAMPLING_PWMModeIsCenterAligned)
           #define pwmSAMPLING_PERIOD_LSB      (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D1_REG)
           #define pwmSAMPLING_PERIOD_LSB_PTR  ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #else
           #define pwmSAMPLING_PERIOD_LSB      (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__F0_REG)
           #define pwmSAMPLING_PERIOD_LSB_PTR  ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__F0_REG)
       #endif /* (pwmSAMPLING_PWMModeIsCenterAligned) */

       #define pwmSAMPLING_COMPARE1_LSB        (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define pwmSAMPLING_COMPARE1_LSB_PTR    ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define pwmSAMPLING_COMPARE2_LSB        (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define pwmSAMPLING_COMPARE2_LSB_PTR    ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define pwmSAMPLING_COUNTERCAP_LSB      (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define pwmSAMPLING_COUNTERCAP_LSB_PTR  ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define pwmSAMPLING_COUNTER_LSB         (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define pwmSAMPLING_COUNTER_LSB_PTR     ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define pwmSAMPLING_CAPTURE_LSB         (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__F1_REG)
       #define pwmSAMPLING_CAPTURE_LSB_PTR     ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(pwmSAMPLING_PWMModeIsCenterAligned)
               #define pwmSAMPLING_PERIOD_LSB      (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D1_REG)
               #define pwmSAMPLING_PERIOD_LSB_PTR  ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #else
               #define pwmSAMPLING_PERIOD_LSB      (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__F0_REG)
               #define pwmSAMPLING_PERIOD_LSB_PTR  ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__F0_REG)
            #endif /* (pwmSAMPLING_PWMModeIsCenterAligned) */

            #define pwmSAMPLING_COMPARE1_LSB       (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define pwmSAMPLING_COMPARE1_LSB_PTR   ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define pwmSAMPLING_COMPARE2_LSB       (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define pwmSAMPLING_COMPARE2_LSB_PTR   ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define pwmSAMPLING_COUNTERCAP_LSB     (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define pwmSAMPLING_COUNTERCAP_LSB_PTR ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define pwmSAMPLING_COUNTER_LSB        (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define pwmSAMPLING_COUNTER_LSB_PTR    ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define pwmSAMPLING_CAPTURE_LSB        (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__F1_REG)
            #define pwmSAMPLING_CAPTURE_LSB_PTR    ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__F1_REG)
        #else
            #if(pwmSAMPLING_PWMModeIsCenterAligned)
               #define pwmSAMPLING_PERIOD_LSB      (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
               #define pwmSAMPLING_PERIOD_LSB_PTR  ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #else
               #define pwmSAMPLING_PERIOD_LSB      (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
               #define pwmSAMPLING_PERIOD_LSB_PTR  ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
            #endif /* (pwmSAMPLING_PWMModeIsCenterAligned) */

            #define pwmSAMPLING_COMPARE1_LSB       (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define pwmSAMPLING_COMPARE1_LSB_PTR   ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define pwmSAMPLING_COMPARE2_LSB       (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define pwmSAMPLING_COMPARE2_LSB_PTR   ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define pwmSAMPLING_COUNTERCAP_LSB     (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define pwmSAMPLING_COUNTERCAP_LSB_PTR ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define pwmSAMPLING_COUNTER_LSB        (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define pwmSAMPLING_COUNTER_LSB_PTR    ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define pwmSAMPLING_CAPTURE_LSB        (*(reg16 *) pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
            #define pwmSAMPLING_CAPTURE_LSB_PTR    ((reg16 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define pwmSAMPLING_AUX_CONTROLDP1          (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)
       #define pwmSAMPLING_AUX_CONTROLDP1_PTR      ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (pwmSAMPLING_Resolution == 8) */

   #define pwmSAMPLING_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__A1_REG)
   #define pwmSAMPLING_AUX_CONTROLDP0          (*(reg8 *)  pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)
   #define pwmSAMPLING_AUX_CONTROLDP0_PTR      ((reg8 *)   pwmSAMPLING_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (pwmSAMPLING_UsingFixedFunction) */

#if(pwmSAMPLING_KillModeMinTime )
    #define pwmSAMPLING_KILLMODEMINTIME        (*(reg8 *)  pwmSAMPLING_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define pwmSAMPLING_KILLMODEMINTIME_PTR    ((reg8 *)   pwmSAMPLING_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (pwmSAMPLING_KillModeMinTime ) */

#if(pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_256_CLOCKS)
    #define pwmSAMPLING_DEADBAND_COUNT         (*(reg8 *)  pwmSAMPLING_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define pwmSAMPLING_DEADBAND_COUNT_PTR     ((reg8 *)   pwmSAMPLING_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define pwmSAMPLING_DEADBAND_LSB_PTR       ((reg8 *)   pwmSAMPLING_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define pwmSAMPLING_DEADBAND_LSB           (*(reg8 *)  pwmSAMPLING_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (pwmSAMPLING_UsingFixedFunction)
        #define pwmSAMPLING_DEADBAND_COUNT         (*(reg8 *)  pwmSAMPLING_PWMHW__CFG0)
        #define pwmSAMPLING_DEADBAND_COUNT_PTR     ((reg8 *)   pwmSAMPLING_PWMHW__CFG0)
        #define pwmSAMPLING_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << pwmSAMPLING_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define pwmSAMPLING_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define pwmSAMPLING_DEADBAND_COUNT         (*(reg8 *)  pwmSAMPLING_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define pwmSAMPLING_DEADBAND_COUNT_PTR     ((reg8 *)   pwmSAMPLING_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define pwmSAMPLING_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << pwmSAMPLING_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define pwmSAMPLING_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (pwmSAMPLING_UsingFixedFunction) */
#endif /* (pwmSAMPLING_DeadBandMode == pwmSAMPLING__B_PWM__DBM_256_CLOCKS) */



#if (pwmSAMPLING_UsingFixedFunction)
    #define pwmSAMPLING_STATUS                 (*(reg8 *) pwmSAMPLING_PWMHW__SR0)
    #define pwmSAMPLING_STATUS_PTR             ((reg8 *) pwmSAMPLING_PWMHW__SR0)
    #define pwmSAMPLING_STATUS_MASK            (*(reg8 *) pwmSAMPLING_PWMHW__SR0)
    #define pwmSAMPLING_STATUS_MASK_PTR        ((reg8 *) pwmSAMPLING_PWMHW__SR0)
    #define pwmSAMPLING_CONTROL                (*(reg8 *) pwmSAMPLING_PWMHW__CFG0)
    #define pwmSAMPLING_CONTROL_PTR            ((reg8 *) pwmSAMPLING_PWMHW__CFG0)
    #define pwmSAMPLING_CONTROL2               (*(reg8 *) pwmSAMPLING_PWMHW__CFG1)
    #define pwmSAMPLING_CONTROL3               (*(reg8 *) pwmSAMPLING_PWMHW__CFG2)
    #define pwmSAMPLING_GLOBAL_ENABLE          (*(reg8 *) pwmSAMPLING_PWMHW__PM_ACT_CFG)
    #define pwmSAMPLING_GLOBAL_ENABLE_PTR      ( (reg8 *) pwmSAMPLING_PWMHW__PM_ACT_CFG)
    #define pwmSAMPLING_GLOBAL_STBY_ENABLE     (*(reg8 *) pwmSAMPLING_PWMHW__PM_STBY_CFG)
    #define pwmSAMPLING_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) pwmSAMPLING_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define pwmSAMPLING_BLOCK_EN_MASK          (pwmSAMPLING_PWMHW__PM_ACT_MSK)
    #define pwmSAMPLING_BLOCK_STBY_EN_MASK     (pwmSAMPLING_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define pwmSAMPLING_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define pwmSAMPLING_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define pwmSAMPLING_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define pwmSAMPLING_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define pwmSAMPLING_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define pwmSAMPLING_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define pwmSAMPLING_CTRL_ENABLE            (uint8)((uint8)0x01u << pwmSAMPLING_CTRL_ENABLE_SHIFT)
    #define pwmSAMPLING_CTRL_RESET             (uint8)((uint8)0x01u << pwmSAMPLING_CTRL_RESET_SHIFT)
    #define pwmSAMPLING_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << pwmSAMPLING_CTRL_CMPMODE2_SHIFT)
    #define pwmSAMPLING_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << pwmSAMPLING_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define pwmSAMPLING_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define pwmSAMPLING_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << pwmSAMPLING_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define pwmSAMPLING_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define pwmSAMPLING_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define pwmSAMPLING_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define pwmSAMPLING_STATUS_TC_INT_EN_MASK_SHIFT            (pwmSAMPLING_STATUS_TC_SHIFT - 4u)
    #define pwmSAMPLING_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define pwmSAMPLING_STATUS_CMP1_INT_EN_MASK_SHIFT          (pwmSAMPLING_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define pwmSAMPLING_STATUS_TC              (uint8)((uint8)0x01u << pwmSAMPLING_STATUS_TC_SHIFT)
    #define pwmSAMPLING_STATUS_CMP1            (uint8)((uint8)0x01u << pwmSAMPLING_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define pwmSAMPLING_STATUS_TC_INT_EN_MASK              (uint8)((uint8)pwmSAMPLING_STATUS_TC >> 4u)
    #define pwmSAMPLING_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)pwmSAMPLING_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define pwmSAMPLING_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define pwmSAMPLING_RT1_MASK              (uint8)((uint8)0x03u << pwmSAMPLING_RT1_SHIFT)
    #define pwmSAMPLING_SYNC                  (uint8)((uint8)0x03u << pwmSAMPLING_RT1_SHIFT)
    #define pwmSAMPLING_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define pwmSAMPLING_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << pwmSAMPLING_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define pwmSAMPLING_SYNCDSI_EN            (uint8)((uint8)0x0Fu << pwmSAMPLING_SYNCDSI_SHIFT)


#else
    #define pwmSAMPLING_STATUS                (*(reg8 *)   pwmSAMPLING_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define pwmSAMPLING_STATUS_PTR            ((reg8 *)    pwmSAMPLING_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define pwmSAMPLING_STATUS_MASK           (*(reg8 *)   pwmSAMPLING_PWMUDB_genblk8_stsreg__MASK_REG)
    #define pwmSAMPLING_STATUS_MASK_PTR       ((reg8 *)    pwmSAMPLING_PWMUDB_genblk8_stsreg__MASK_REG)
    #define pwmSAMPLING_STATUS_AUX_CTRL       (*(reg8 *)   pwmSAMPLING_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define pwmSAMPLING_CONTROL               (*(reg8 *)   pwmSAMPLING_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define pwmSAMPLING_CONTROL_PTR           ((reg8 *)    pwmSAMPLING_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define pwmSAMPLING_CTRL_ENABLE_SHIFT      (0x07u)
    #define pwmSAMPLING_CTRL_RESET_SHIFT       (0x06u)
    #define pwmSAMPLING_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define pwmSAMPLING_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define pwmSAMPLING_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define pwmSAMPLING_CTRL_ENABLE            (uint8)((uint8)0x01u << pwmSAMPLING_CTRL_ENABLE_SHIFT)
    #define pwmSAMPLING_CTRL_RESET             (uint8)((uint8)0x01u << pwmSAMPLING_CTRL_RESET_SHIFT)
    #define pwmSAMPLING_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << pwmSAMPLING_CTRL_CMPMODE2_SHIFT)
    #define pwmSAMPLING_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << pwmSAMPLING_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define pwmSAMPLING_STATUS_KILL_SHIFT          (0x05u)
    #define pwmSAMPLING_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define pwmSAMPLING_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define pwmSAMPLING_STATUS_TC_SHIFT            (0x02u)
    #define pwmSAMPLING_STATUS_CMP2_SHIFT          (0x01u)
    #define pwmSAMPLING_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define pwmSAMPLING_STATUS_KILL_INT_EN_MASK_SHIFT          (pwmSAMPLING_STATUS_KILL_SHIFT)
    #define pwmSAMPLING_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (pwmSAMPLING_STATUS_FIFONEMPTY_SHIFT)
    #define pwmSAMPLING_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (pwmSAMPLING_STATUS_FIFOFULL_SHIFT)
    #define pwmSAMPLING_STATUS_TC_INT_EN_MASK_SHIFT            (pwmSAMPLING_STATUS_TC_SHIFT)
    #define pwmSAMPLING_STATUS_CMP2_INT_EN_MASK_SHIFT          (pwmSAMPLING_STATUS_CMP2_SHIFT)
    #define pwmSAMPLING_STATUS_CMP1_INT_EN_MASK_SHIFT          (pwmSAMPLING_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define pwmSAMPLING_STATUS_KILL            (uint8)((uint8)0x00u << pwmSAMPLING_STATUS_KILL_SHIFT )
    #define pwmSAMPLING_STATUS_FIFOFULL        (uint8)((uint8)0x01u << pwmSAMPLING_STATUS_FIFOFULL_SHIFT)
    #define pwmSAMPLING_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << pwmSAMPLING_STATUS_FIFONEMPTY_SHIFT)
    #define pwmSAMPLING_STATUS_TC              (uint8)((uint8)0x01u << pwmSAMPLING_STATUS_TC_SHIFT)
    #define pwmSAMPLING_STATUS_CMP2            (uint8)((uint8)0x01u << pwmSAMPLING_STATUS_CMP2_SHIFT)
    #define pwmSAMPLING_STATUS_CMP1            (uint8)((uint8)0x01u << pwmSAMPLING_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define pwmSAMPLING_STATUS_KILL_INT_EN_MASK            (pwmSAMPLING_STATUS_KILL)
    #define pwmSAMPLING_STATUS_FIFOFULL_INT_EN_MASK        (pwmSAMPLING_STATUS_FIFOFULL)
    #define pwmSAMPLING_STATUS_FIFONEMPTY_INT_EN_MASK      (pwmSAMPLING_STATUS_FIFONEMPTY)
    #define pwmSAMPLING_STATUS_TC_INT_EN_MASK              (pwmSAMPLING_STATUS_TC)
    #define pwmSAMPLING_STATUS_CMP2_INT_EN_MASK            (pwmSAMPLING_STATUS_CMP2)
    #define pwmSAMPLING_STATUS_CMP1_INT_EN_MASK            (pwmSAMPLING_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define pwmSAMPLING_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define pwmSAMPLING_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define pwmSAMPLING_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define pwmSAMPLING_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define pwmSAMPLING_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* pwmSAMPLING_UsingFixedFunction */

#endif  /* CY_PWM_pwmSAMPLING_H */


/* [] END OF FILE */
