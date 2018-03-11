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

#ifndef WOTAN_H
#define WOTAN_H
    
#include "project.h"    
    
#include <stdio.h>
#include <string.h>
#include <math.h>

// Auxilliary macros
#define  TRUE               1
#define  FALSE              0

#define START_CLOCK         0
#define STOP_CLOCK          1

// UART interface
#define  UART_BUF_IN         100
#define  UART_BUF_OUT        80
#define  KEY_RUN             'r'
#define  KEY_RUN_ALT         'v'
#define  KEY_RUN_AND_SHOW    's'
#define  KEY_RUN_NEXT_SHOW   'a'
#define  KEY_SEND_ASCII_DAT  'd'
#define  KEY_SEND_BYTE_DAT   'o'
#define  KEY_SET_PARAMS      't'
#define  KEY_RESET           'e'
#define  KEY_DAC1            '1'
#define  KEY_DAC2            '2'
#define  KEY_DAC3            '3'
#define  KEY_DAC4            '4'
#define  KEY_SIG_IN          '5'

 
// Sequence parameters
#define  NSAMPLES_ADC       15000               // 1 MS/s, max value: 15000
#define  NSAMPLES_DAC       NSAMPLES_ADC/4      // 250 kS/s (make sample duration for Transmit and Receive the same)
#define  SEQU_DURATION_US   NSAMPLES_ADC*3      // duration includes start- and end ramp

#define  IGNORE_FIRST_PART  TRUE               // if FALSE: shows start ramp of sequence (default: TRUE)

#define  N_TDS_ADC          20
#define  N_TDS_DAC          3               // TD1: on ramp, TD2: sequence (each with length NSAMPLES_DAC)

#define  FLASH_CH1          (const uint8 *)     0x08000  // Flash addresses for storing the DAC wave forms
#define  FLASH_CH2          (const uint8 *)     0x10000
#define  FLASH_CH3          (const uint8 *)     0x20000
#define  FLASH_CH4          (const uint8 *)     0x30000 

#define  CLOCK_SHIFT_CH1     0b1100   
#define  CLOCK_SHIFT_CH2     0b0110   
#define  CLOCK_SHIFT_CH3     0b0011   
#define  CLOCK_SHIFT_CH4     0b1001   

// Set all MPI waveform parameters
#define NUM_CHANNELS    4
#define CH1             0
#define CH2             1
#define CH3             2
#define CH4             3
#define FREQ_CORRECTION 1.0 // XTAL: +/- 30ppm
#define OM_1HZ          2*M_PI/250000.0*FREQ_CORRECTION
#define PHI_90DEG       M_PI/2.0
#define MAX_VALUE       180 // 180 => CH1, CH2: 1.82 V-pp; CH3, Ch4: 1.78 V-pp
#define WAVE_EXIST      MAX_VALUE/2 // token = first sample in each wave form    
    
    
// DMA channels
 /* Defines for DMA_DAC_1 */
#define DMA_DAC_1_BYTES_PER_BURST 1
#define DMA_DAC_1_REQUEST_PER_BURST 1   
#define DMA_DAC_1_SRC_BASE (FLASH_CH1)
#define DMA_DAC_1_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_DAC */
/* Move these variable declarations to the top of the function */
uint8 DMA_DAC_1_Chan;
uint8 DMA_DAC_1_TD[N_TDS_DAC];

/* Defines for DMA_DAC_2 */
#define DMA_DAC_2_BYTES_PER_BURST 1
#define DMA_DAC_2_REQUEST_PER_BURST 1
#define DMA_DAC_2_SRC_BASE (FLASH_CH2)
#define DMA_DAC_2_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_DAC_2 */
/* Move these variable declarations to the top of the function */
uint8 DMA_DAC_2_Chan;
uint8 DMA_DAC_2_TD[N_TDS_DAC];

/* Defines for DMA_DAC_3 */
#define DMA_DAC_3_BYTES_PER_BURST 1
#define DMA_DAC_3_REQUEST_PER_BURST 1
#define DMA_DAC_3_SRC_BASE (FLASH_CH3)
#define DMA_DAC_3_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_DAC_3 */
/* Move these variable declarations to the top of the function */
uint8 DMA_DAC_3_Chan;
uint8 DMA_DAC_3_TD[N_TDS_DAC];

/* Defines for DMA_DAC_4 */
#define DMA_DAC_4_BYTES_PER_BURST 1
#define DMA_DAC_4_REQUEST_PER_BURST 1
#define DMA_DAC_4_SRC_BASE (FLASH_CH4)
#define DMA_DAC_4_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_DAC */
/* Move these variable declarations to the top of the function */
uint8 DMA_DAC_4_Chan;
uint8 DMA_DAC_4_TD[N_TDS_DAC];


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


// All userdefined function prototypes:
void init_components(void);
void show_default_message(void);
void show_channel_num(void);
void set_sequence_params(void);
void generate_sequence(void);
void run_sequence(char);
void display_results(void);
void uart_interface(void);
void ble_uart_interface(void);
void usbfs_send_data(void);
void dma_dac_1_init(void);
void dma_dac_2_init(void);
void dma_dac_3_init(void);
void dma_dac_4_init(void);
void dma_adc_1_init(void);
void dma_adc_2_init(void);
CY_ISR_PROTO( isr_triggerIn );
CY_ISR_PROTO( isr_DAC_1_done );
CY_ISR_PROTO( isr_DAC_2_done );
CY_ISR_PROTO( isr_DAC_3_done );
CY_ISR_PROTO( isr_DAC_4_done );
CY_ISR_PROTO( isr_ADC_1_done );
CY_ISR_PROTO( isr_ADC_2_done );


// auxilliary variables and flags
char    sms         [UART_BUF_OUT];
char    puttyIn     [UART_BUF_IN];
int     rxBuf;
int     rxBufBLE;



// USBUART (USBFS)
#define USBUART_BUFFER_SIZE (64u)
#define USBFS_DEVICE    (0u)
#define USBFS_TX_SIZE   60
uint16 count;
uint8 buffer[USBFS_TX_SIZE];
uint8 data_tx[4];


// Bluetooth (command: 'BinaryABC' with A: packet number (1..30), B: command (1..not-implemented-yet), C: channel number (1..5)
#define BLE_MODULE_FALSE    1u
#define BLE_MODULE_TRUE     0u 
#define BAUD_1300000        0u
#define BAUD_9600           1u
#define DATA_ORDER          "Binary"
#define COMMAND_NUMBER      strlen(DATA_ORDER)
#define CHANNEL_NUMBER      strlen(DATA_ORDER)+1
#define PACKET_NUMBER       strlen(DATA_ORDER)+2
#define PACKET_SIZE         500
#define BYTES_PER_PACKET    4
char    puttyInBLE          [UART_BUF_IN];


// Two adc buffers 
uint16 signal_adc_1[NSAMPLES_ADC]; 
uint16 signal_adc_2[NSAMPLES_ADC];




#endif /* WOTAN_H */  

/* [] END OF FILE */
