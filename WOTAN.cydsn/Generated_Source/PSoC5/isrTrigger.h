/*******************************************************************************
* File Name: isrTrigger.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_isrTrigger_H)
#define CY_ISR_isrTrigger_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void isrTrigger_Start(void);
void isrTrigger_StartEx(cyisraddress address);
void isrTrigger_Stop(void);

CY_ISR_PROTO(isrTrigger_Interrupt);

void isrTrigger_SetVector(cyisraddress address);
cyisraddress isrTrigger_GetVector(void);

void isrTrigger_SetPriority(uint8 priority);
uint8 isrTrigger_GetPriority(void);

void isrTrigger_Enable(void);
uint8 isrTrigger_GetState(void);
void isrTrigger_Disable(void);

void isrTrigger_SetPending(void);
void isrTrigger_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the isrTrigger ISR. */
#define isrTrigger_INTC_VECTOR            ((reg32 *) isrTrigger__INTC_VECT)

/* Address of the isrTrigger ISR priority. */
#define isrTrigger_INTC_PRIOR             ((reg8 *) isrTrigger__INTC_PRIOR_REG)

/* Priority of the isrTrigger interrupt. */
#define isrTrigger_INTC_PRIOR_NUMBER      isrTrigger__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable isrTrigger interrupt. */
#define isrTrigger_INTC_SET_EN            ((reg32 *) isrTrigger__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the isrTrigger interrupt. */
#define isrTrigger_INTC_CLR_EN            ((reg32 *) isrTrigger__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the isrTrigger interrupt state to pending. */
#define isrTrigger_INTC_SET_PD            ((reg32 *) isrTrigger__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the isrTrigger interrupt. */
#define isrTrigger_INTC_CLR_PD            ((reg32 *) isrTrigger__INTC_CLR_PD_REG)


#endif /* CY_ISR_isrTrigger_H */


/* [] END OF FILE */
