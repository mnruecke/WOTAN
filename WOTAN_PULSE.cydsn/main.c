/* ========================================
 * 
 * This code is supplementary material for the IWMPI 2018, Hamburg, Germany 
 *
 * Martin.Rueckert@physik.uni-wuerzburg.de
 *
 * Copyright (C) 2017 University of Wuerzburg, Experimental Physics 5, Biophysics
 * https://www.physik.uni-wuerzburg.de/ep5/magnetic-particle-imaging/
 *
 * WOTAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * WOTAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WOTAN.  If not, see <http://www.gnu.org/licenses/>.
 * ========================================
*/
#include "project.h"
#include <stdio.h>

#define     ON           0u
#define     OFF          1u
   
#define     SEQUENCE_RUN    0u
#define     SEQUENCE_HALT   1u
#define     START           0u
#define     STOP            1u


// Defines for receive chain
#define  NSAMPLES_ADC         1000
#define     N_TDS_ADC          20


/* Defines for DMA_ADC_1 */
#define DMA_ADC_1_BYTES_PER_BURST 2
#define DMA_ADC_1_REQUEST_PER_BURST 1
#define DMA_ADC_1_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_ADC_1_DST_BASE (CYDEV_SRAM_BASE)

/* Variable declarations for DMA_ADC */
/* Move these variable declarations to the top of the function */
uint8 DMA_ADC_1_Chan;
uint8 DMA_ADC_1_TD[N_TDS_ADC];

/* Defines for DMA_ADC_2 */
#define DMA_ADC_2_BYTES_PER_BURST 2
#define DMA_ADC_2_REQUEST_PER_BURST 1
#define DMA_ADC_2_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_ADC_2_DST_BASE (CYDEV_SRAM_BASE)

/* Variable declarations for DMA_ADC */
/* Move these variable declarations to the top of the function */
uint8 DMA_ADC_2_Chan;
uint8 DMA_ADC_2_TD[N_TDS_ADC];


void dma_adc_1_init(void);
void dma_adc_2_init(void);
CY_ISR_PROTO( isr_ADC_1_done );
CY_ISR_PROTO( isr_ADC_2_done );
CY_ISR_PROTO( isr_run );

uint8 count_of_runs=0; 
char  sms[80];
char  bleIn;

// Two adc buffers 
uint16 signal_adc_1[NSAMPLES_ADC]; 
uint16 signal_adc_2[NSAMPLES_ADC];


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    // Bluetooth-UART connection
    UART_BLE_Start();
    
    // Receive channel
    ADC_SAR_1_Start();
        ADC_SAR_1_IRQ_Disable();
    ADC_SAR_2_Start();
        ADC_SAR_2_IRQ_Disable();
    isr_ADC_1_StartEx( isr_ADC_1_done );
    isr_ADC_2_StartEx( isr_ADC_2_done );
    isr_RUN_StartEx(   isr_run );
    
    PWM_ClockSync_Start();  
    
    // Configure DMA channels between ADCs and memory
    dma_adc_1_init();
    dma_adc_2_init();
    count_of_runs++;    
    

    // Transmit channel
    PWM_1_Start();
    CyDelay(10); // Avoid trigger after reset or power on
    clkDuty_Start();
    
    //DEBUG
    WaveDAC8_1_Start();
    
    for(;;)
    {
        /* Pulse sequence control
           BLE trigger overwrites Manual trigger and vice versa */
        if( UART_BLE_GetRxBufferSize() > 0)
        {
            bleIn =  UART_BLE_GetChar();
            if( bleIn == 's' ) // "Single Shot" -> sends binary data
            {
                BLE_Trigger_Write(START);
            }
            if( bleIn == 'c' ) // "Continuously" -> sends binary data
            {
                BLE_Trigger_Write(START);
            }
            if( bleIn == 'h' ) // "Halt"
            {
                BLE_Trigger_Write(STOP);
            }
            if( bleIn == 'd' ) // "Debug" -> run once and send data in ASCII
            {
                BLE_Trigger_Write(START);
            }
            if( bleIn == '1' ) // Puls: 20us
            {
                PWM_1_WriteCompare(49999u);
                bleIn = 0;
            }
            if( bleIn == '2' ) // Puls: 40us
            {
                PWM_1_WriteCompare(49998u);
                bleIn = 0;
            }
            if( bleIn == '3' ) // Puls: 100us
            {
                PWM_1_WriteCompare(49995u);
                bleIn = 0;
            }
            if( bleIn == '4' ) // Puls: 200us
            {
                PWM_1_WriteCompare(49990u);
                bleIn = 0;
            }
            if( bleIn == '5' ) // Puls: 500us
            {
                PWM_1_WriteCompare(49975u);
                bleIn = 0;
            }

        }

    }
}


void dma_adc_1_init(void)
{
    /* DMA Configuration for DMA_ADC_1 */
    if( count_of_runs == 0) // avoid multiple allocation of resources
    {
        DMA_ADC_1_Chan = DMA_ADC_1_DmaInitialize(DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_REQUEST_PER_BURST, 
            HI16(DMA_ADC_1_SRC_BASE), HI16(DMA_ADC_1_DST_BASE));
        DMA_ADC_1_TD[0] = CyDmaTdAllocate();
        DMA_ADC_1_TD[1] = CyDmaTdAllocate();
        DMA_ADC_1_TD[2] = CyDmaTdAllocate();
        DMA_ADC_1_TD[3] = CyDmaTdAllocate();
        DMA_ADC_1_TD[4] = CyDmaTdAllocate();
        DMA_ADC_1_TD[5] = CyDmaTdAllocate();
        DMA_ADC_1_TD[6] = CyDmaTdAllocate();
        DMA_ADC_1_TD[7] = CyDmaTdAllocate();
        DMA_ADC_1_TD[8] = CyDmaTdAllocate();
        DMA_ADC_1_TD[9] = CyDmaTdAllocate();
        DMA_ADC_1_TD[10] = CyDmaTdAllocate();
        DMA_ADC_1_TD[11] = CyDmaTdAllocate();
        DMA_ADC_1_TD[12] = CyDmaTdAllocate();
        DMA_ADC_1_TD[13] = CyDmaTdAllocate();
        DMA_ADC_1_TD[14] = CyDmaTdAllocate();
        DMA_ADC_1_TD[15] = CyDmaTdAllocate();
        DMA_ADC_1_TD[16] = CyDmaTdAllocate();
        DMA_ADC_1_TD[17] = CyDmaTdAllocate();
        DMA_ADC_1_TD[18] = CyDmaTdAllocate();
        DMA_ADC_1_TD[19] = CyDmaTdAllocate();
    }
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[0], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[1], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[1], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[2], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[2], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[3], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[3], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[4], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[4], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[5], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[5], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[6], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[6], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[7], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[7], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[8], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[8], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[9], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[9], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[10],  CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[10], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[11], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[11], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[12], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[12], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[13], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[13], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[14], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[14], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[15], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[15], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[16], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[16], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[17], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[17], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[18], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[18], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[19], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[19], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST, CY_DMA_DISABLE_TD, DMA_ADC_1__TD_TERMOUT_EN | (CY_DMA_TD_INC_DST_ADR));
    CyDmaTdSetAddress(DMA_ADC_1_TD[0], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(0*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[1], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(1*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[2], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(2*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[3], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(3*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[4], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(4*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[5], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(5*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[6], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(6*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[7], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(7*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[8], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(8*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[9], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(9*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[10], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(10*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[11], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(11*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[12], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(12*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[13], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(13*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[14], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(14*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[15], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(15*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[16], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(16*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[17], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(17*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[18], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(18*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[19], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(19*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaChSetInitialTd(DMA_ADC_1_Chan, DMA_ADC_1_TD[0]);
    CyDmaChEnable(DMA_ADC_1_Chan, 1);     
}

void dma_adc_2_init(void)
{
    /* DMA Configuration for DMA_ADC_2 */
    if( count_of_runs == 0) // avoid multiple allocation of resources
    {
        DMA_ADC_2_Chan = DMA_ADC_2_DmaInitialize(DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_REQUEST_PER_BURST, 
            HI16(DMA_ADC_2_SRC_BASE), HI16(DMA_ADC_2_DST_BASE));
        DMA_ADC_2_TD[0] = CyDmaTdAllocate();
        DMA_ADC_2_TD[1] = CyDmaTdAllocate();
        DMA_ADC_2_TD[2] = CyDmaTdAllocate();
        DMA_ADC_2_TD[3] = CyDmaTdAllocate();
        DMA_ADC_2_TD[4] = CyDmaTdAllocate();
        DMA_ADC_2_TD[5] = CyDmaTdAllocate();
        DMA_ADC_2_TD[6] = CyDmaTdAllocate();
        DMA_ADC_2_TD[7] = CyDmaTdAllocate();
        DMA_ADC_2_TD[8] = CyDmaTdAllocate();
        DMA_ADC_2_TD[9] = CyDmaTdAllocate();
        DMA_ADC_2_TD[10] = CyDmaTdAllocate();
        DMA_ADC_2_TD[11] = CyDmaTdAllocate();
        DMA_ADC_2_TD[12] = CyDmaTdAllocate();
        DMA_ADC_2_TD[13] = CyDmaTdAllocate();
        DMA_ADC_2_TD[14] = CyDmaTdAllocate();
        DMA_ADC_2_TD[15] = CyDmaTdAllocate();
        DMA_ADC_2_TD[16] = CyDmaTdAllocate();
        DMA_ADC_2_TD[17] = CyDmaTdAllocate();
        DMA_ADC_2_TD[18] = CyDmaTdAllocate();
        DMA_ADC_2_TD[19] = CyDmaTdAllocate();
    }
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[0], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[1], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[1], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[2], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[2], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[3], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[3], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[4], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[4], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[5], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[5], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[6], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[6], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[7], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[7], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[8], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[8], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[9], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[9], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[10],  CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[10], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[11], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[11], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[12], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[12], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[13], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[13], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[14], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[14], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[15], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[15], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[16], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[16], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[17], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[17], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[18], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[18], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[19], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[19], NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST, CY_DMA_DISABLE_TD, DMA_ADC_2__TD_TERMOUT_EN | (CY_DMA_TD_INC_DST_ADR));
    CyDmaTdSetAddress(DMA_ADC_2_TD[0], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(0*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[1], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(1*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[2], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(2*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[3], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(3*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[4], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(4*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[5], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(5*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[6], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(6*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[7], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(7*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[8], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(8*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[9], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(9*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[10], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(10*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[11], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(11*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[12], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(12*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[13], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(13*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[14], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(14*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[15], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(15*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[16], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(16*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[17], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(17*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[18], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(18*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[19], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(19*NSAMPLES_ADC/(N_TDS_ADC) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaChSetInitialTd(DMA_ADC_2_Chan, DMA_ADC_2_TD[0]);
    CyDmaChEnable(DMA_ADC_2_Chan, 1);     
}

CY_ISR( isr_ADC_1_done )
{ 
    // Deactivate puls shape pwm
    if( (bleIn == 's') || (bleIn == 'd')) // Single shot and debug mode
        BLE_Trigger_Write( STOP );
    
    // Stop ADC clock
    pwmSamp_Write( STOP );
    
    // Reset DMA channels for next run
    dma_adc_1_init();
    dma_adc_2_init();
    count_of_runs++;   
}

CY_ISR( isr_ADC_2_done )
{   
    // Send data
    if( bleIn != 'd' )
    {
        // Send data in binary form (16 bit big endian)
        for(int j=0; j<NSAMPLES_ADC; j++)
        {
            // turn uint16 arrays into uint8 streams:
            UART_BLE_PutChar(signal_adc_1[j] >>8   );
            UART_BLE_PutChar(signal_adc_1[j] &0xFF ); 
            UART_BLE_PutChar(signal_adc_2[j] >>8   );
            UART_BLE_PutChar(signal_adc_2[j] &0xFF ); 
        }
    }
    else
    {
        // Send data as commented ascii strings for debugging
        for(int i=0; i<100; i++)
        {
            sprintf(sms, "ADC 1, sample %4d: %6d \n\r", i, signal_adc_1[i]);
            UART_BLE_PutString( sms );
            sprintf(sms, "ADC 2, sample %4d: %6d \n\r", i, signal_adc_2[i]);
            UART_BLE_PutString( sms );
            
            // interrupt data stream via BLE
            if( UART_BLE_GetRxBufferSize() ) 
            {
                UART_BLE_ClearRxBuffer();// stop and disregard input
                break;
            }
        }
        
        sprintf(sms, "\n\rCount of runs after reset: %4d\n\r", count_of_runs-1);
        UART_BLE_PutString( sms );
    }
}

CY_ISR( isr_run )
{
    // Start ADC clock
    pwmSamp_Write( START );
}

/* [] END OF FILE */
