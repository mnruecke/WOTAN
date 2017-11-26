/*******************************************************************************
* File Name: ShiftClockDAC.h
* Version 2.30
*
* Description:
*  This header file contains definitions associated with the Shift Register
*  component.
*
* Note: none
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/


#if !defined(CY_SHIFTREG_ShiftClockDAC_H)
#define CY_SHIFTREG_ShiftClockDAC_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define ShiftClockDAC_FIFO_SIZE          (4u)
#define ShiftClockDAC_USE_INPUT_FIFO     (0u)
#define ShiftClockDAC_USE_OUTPUT_FIFO    (0u)
#define ShiftClockDAC_SR_SIZE            (2u)


/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;

    uint8 saveSrA0Reg;
    uint8 saveSrA1Reg;

    #if(CY_UDB_V0)
        uint8 saveSrIntMask;
    #endif /* (CY_UDB_V0) */

} ShiftClockDAC_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  ShiftClockDAC_Start(void)                              ;
void  ShiftClockDAC_Stop(void)                               ;
void  ShiftClockDAC_Init(void)                               ;
void  ShiftClockDAC_Enable(void)                             ;
void  ShiftClockDAC_RestoreConfig(void)                      ;
void  ShiftClockDAC_SaveConfig(void)                         ;
void  ShiftClockDAC_Sleep(void)                              ;
void  ShiftClockDAC_Wakeup(void)                             ;
void  ShiftClockDAC_EnableInt(void)                          ;
void  ShiftClockDAC_DisableInt(void)                         ;
void  ShiftClockDAC_SetIntMode(uint8 interruptSource)        ;
uint8 ShiftClockDAC_GetIntStatus(void)                       ;
void  ShiftClockDAC_WriteRegValue(uint8 shiftData) \
                                                                ;
uint8 ShiftClockDAC_ReadRegValue(void) ;
uint8    ShiftClockDAC_GetFIFOStatus(uint8 fifoId)           ;

#if(0u != ShiftClockDAC_USE_INPUT_FIFO)
    cystatus ShiftClockDAC_WriteData(uint8 shiftData) \
                                                                ;
#endif /* (0u != ShiftClockDAC_USE_INPUT_FIFO) */

#if(0u != ShiftClockDAC_USE_OUTPUT_FIFO)
    uint8 ShiftClockDAC_ReadData(void) ;
#endif /* (0u != ShiftClockDAC_USE_OUTPUT_FIFO) */


/**********************************
*   Variable with external linkage
**********************************/

extern uint8 ShiftClockDAC_initVar;


/***************************************
*           API Constants
***************************************/

#define ShiftClockDAC_LOAD                   (0x01u)
#define ShiftClockDAC_STORE                  (0x02u)
#define ShiftClockDAC_RESET                  (0x04u)

#define ShiftClockDAC_IN_FIFO                (0x01u)
#define ShiftClockDAC_OUT_FIFO               (0x02u)

#define ShiftClockDAC_RET_FIFO_FULL          (0x00u)

/* This define is obsolete */
#define ShiftClockDAC_RET_FIFO_NOT_EMPTY     (0x01u)

#define ShiftClockDAC_RET_FIFO_PARTIAL       (0x01u)
#define ShiftClockDAC_RET_FIFO_EMPTY         (0x02u)
#define ShiftClockDAC_RET_FIFO_NOT_DEFINED   (0xFEu)


/***************************************
*    Enumerated Types and Parameters
***************************************/

#define ShiftClockDAC__LEFT 0
#define ShiftClockDAC__RIGHT 1



/***************************************
*    Initial Parameter Constants
***************************************/

#define ShiftClockDAC_SR_MASK    (0x3u) /* Unsigned is added to parameter */
#define ShiftClockDAC_INT_SRC    (0u)
#define ShiftClockDAC_DIRECTION  (1u)


/***************************************
*             Registers
***************************************/

/* Control register */
#define ShiftClockDAC_SR_CONTROL_REG (* (reg8 *) \
                                           ShiftClockDAC_bSR_SyncCtl_CtrlReg__CONTROL_REG)
#define ShiftClockDAC_SR_CONTROL_PTR (  (reg8 *) \
                                           ShiftClockDAC_bSR_SyncCtl_CtrlReg__CONTROL_REG)

/* Status register */
#define ShiftClockDAC_SR_STATUS_REG      (* (reg8 *) ShiftClockDAC_bSR_StsReg__STATUS_REG)
#define ShiftClockDAC_SR_STATUS_PTR      (  (reg8 *) ShiftClockDAC_bSR_StsReg__STATUS_REG)

/* Interrupt status register */
#define ShiftClockDAC_SR_STATUS_MASK_REG (* (reg8 *) ShiftClockDAC_bSR_StsReg__MASK_REG)
#define ShiftClockDAC_SR_STATUS_MASK_PTR (  (reg8 *) ShiftClockDAC_bSR_StsReg__MASK_REG)

/* Aux control register */
#define ShiftClockDAC_SR_AUX_CONTROL_REG (* (reg8 *) ShiftClockDAC_bSR_StsReg__STATUS_AUX_CTL_REG)
#define ShiftClockDAC_SR_AUX_CONTROL_PTR (  (reg8 *) ShiftClockDAC_bSR_StsReg__STATUS_AUX_CTL_REG)

/* A1 register: only used to implement capture function */
#define ShiftClockDAC_SHIFT_REG_CAPTURE_PTR    ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__A1_REG )

#if(CY_PSOC3 || CY_PSOC5)
    #define ShiftClockDAC_IN_FIFO_VAL_LSB_PTR        ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__F0_REG )

    #define ShiftClockDAC_SHIFT_REG_LSB_PTR          ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__A0_REG )

    #define ShiftClockDAC_SHIFT_REG_VALUE_LSB_PTR    ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__A1_REG )

    #define ShiftClockDAC_OUT_FIFO_VAL_LSB_PTR       ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__F1_REG )

#else
    #if(ShiftClockDAC_SR_SIZE <= 8u) /* 8bit - ShiftReg */
        #define ShiftClockDAC_IN_FIFO_VAL_LSB_PTR        ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__F0_REG )

        #define ShiftClockDAC_SHIFT_REG_LSB_PTR          ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__A0_REG )

        #define ShiftClockDAC_SHIFT_REG_VALUE_LSB_PTR    ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__A1_REG )

        #define ShiftClockDAC_OUT_FIFO_VAL_LSB_PTR       ( (reg8 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__F1_REG )

    #elif(ShiftClockDAC_SR_SIZE <= 16u) /* 16bit - ShiftReg */
        #define ShiftClockDAC_IN_FIFO_VAL_LSB_PTR        ( (reg16 *) \
                                  ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__16BIT_F0_REG )

        #define ShiftClockDAC_SHIFT_REG_LSB_PTR          ( (reg16 *) \
                                  ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__16BIT_A0_REG )

        #define ShiftClockDAC_SHIFT_REG_VALUE_LSB_PTR    ( (reg16 *) \
                                  ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__16BIT_A1_REG )

        #define ShiftClockDAC_OUT_FIFO_VAL_LSB_PTR       ( (reg16 *) \
                                  ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__16BIT_F1_REG )


    #elif(ShiftClockDAC_SR_SIZE <= 24u) /* 24bit - ShiftReg */
        #define ShiftClockDAC_IN_FIFO_VAL_LSB_PTR        ( (reg32 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__F0_REG )

        #define ShiftClockDAC_SHIFT_REG_LSB_PTR          ( (reg32 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__A0_REG )

        #define ShiftClockDAC_SHIFT_REG_VALUE_LSB_PTR    ( (reg32 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__A1_REG )

        #define ShiftClockDAC_OUT_FIFO_VAL_LSB_PTR       ( (reg32 *) \
                                        ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__F1_REG )

    #else /* 32bit - ShiftReg */
        #define ShiftClockDAC_IN_FIFO_VAL_LSB_PTR        ( (reg32 *) \
                                  ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__32BIT_F0_REG )

        #define ShiftClockDAC_SHIFT_REG_LSB_PTR          ( (reg32 *) \
                                  ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__32BIT_A0_REG )

        #define ShiftClockDAC_SHIFT_REG_VALUE_LSB_PTR    ( (reg32 *) \
                                  ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__32BIT_A1_REG )

        #define ShiftClockDAC_OUT_FIFO_VAL_LSB_PTR       ( (reg32 *) \
                                  ShiftClockDAC_bSR_sC8_BShiftRegDp_u0__32BIT_F1_REG )

    #endif  /* (ShiftClockDAC_SR_SIZE <= 8u) */
#endif      /* (CY_PSOC3 || CY_PSOC5) */


/***************************************
*       Register Constants
***************************************/

#define ShiftClockDAC_INTERRUPTS_ENABLE      (0x10u)
#define ShiftClockDAC_LOAD_INT_EN            (0x01u)
#define ShiftClockDAC_STORE_INT_EN           (0x02u)
#define ShiftClockDAC_RESET_INT_EN           (0x04u)
#define ShiftClockDAC_CLK_EN                 (0x01u)

#define ShiftClockDAC_RESET_INT_EN_MASK      (0xFBu)
#define ShiftClockDAC_LOAD_INT_EN_MASK       (0xFEu)
#define ShiftClockDAC_STORE_INT_EN_MASK      (0xFDu)
#define ShiftClockDAC_INTS_EN_MASK           (0x07u)

#define ShiftClockDAC_OUT_FIFO_CLR_BIT       (0x02u)

#if(0u != ShiftClockDAC_USE_INPUT_FIFO)

    #define ShiftClockDAC_IN_FIFO_MASK       (0x18u)

    #define ShiftClockDAC_IN_FIFO_FULL       (0x00u)
    #define ShiftClockDAC_IN_FIFO_EMPTY      (0x01u)
    #define ShiftClockDAC_IN_FIFO_PARTIAL    (0x02u)
    
    /* This define is obsolete */
    #define ShiftClockDAC_IN_FIFO_NOT_EMPTY  (0x02u)
    
#endif /* (0u != ShiftClockDAC_USE_INPUT_FIFO) */

#define ShiftClockDAC_OUT_FIFO_MASK          (0x60u)

#define ShiftClockDAC_OUT_FIFO_EMPTY         (0x00u)
#define ShiftClockDAC_OUT_FIFO_FULL          (0x01u)
#define ShiftClockDAC_OUT_FIFO_PARTIAL       (0x02u)

/* This define is obsolete */
#define ShiftClockDAC_OUT_FIFO_NOT_EMPTY     (0x02u)

#define ShiftClockDAC_IN_FIFO_SHIFT_MASK     (0x03u)
#define ShiftClockDAC_OUT_FIFO_SHIFT_MASK    (0x05u)

#define ShiftClockDAC_DISABLED               (0u)
#define ShiftClockDAC_DEFAULT_A0             (0u)
#define ShiftClockDAC_DEFAULT_A1             (0u)


/***************************************
*       Macros
***************************************/

#define ShiftClockDAC_IS_ENABLED         (0u != (ShiftClockDAC_SR_CONTROL & ShiftClockDAC_CLK_EN))

#define ShiftClockDAC_GET_OUT_FIFO_STS   ((ShiftClockDAC_SR_STATUS & ShiftClockDAC_OUT_FIFO_MASK) >> \
                                              ShiftClockDAC_OUT_FIFO_SHIFT_MASK)

#define ShiftClockDAC_GET_IN_FIFO_STS    ((ShiftClockDAC_SR_STATUS & ShiftClockDAC_IN_FIFO_MASK)  >> \
                                              ShiftClockDAC_IN_FIFO_SHIFT_MASK)


/***************************************
*       Obsolete
***************************************/

/* Following code are OBSOLETE and must not be used 
 * starting from ShiftReg 2.20 
 */

#define ShiftClockDAC_SR_CONTROL     (ShiftClockDAC_SR_CONTROL_REG)
#define ShiftClockDAC_SR_STATUS      (ShiftClockDAC_SR_STATUS_REG)
#define ShiftClockDAC_SR_STATUS_MASK (ShiftClockDAC_SR_STATUS_MASK_REG)
#define ShiftClockDAC_SR_AUX_CONTROL (ShiftClockDAC_SR_AUX_CONTROL_REG)

#define ShiftClockDAC_IN_FIFO_SHFT_MASK  (ShiftClockDAC_IN_FIFO_SHIFT_MASK)
#define ShiftClockDAC_OUT_FIFO_SHFT_MASK (ShiftClockDAC_OUT_FIFO_SHFIT_MASK)

#define ShiftClockDAC_RET_FIFO_BAD_PARAM (0xFFu)

#endif /* (CY_SHIFTREG_ShiftClockDAC_H) */


/* [] END OF FILE */
