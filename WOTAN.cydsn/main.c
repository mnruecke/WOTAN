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
#include <string.h>
#include <math.h>

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

struct sequenceParams { 
    float           om          [NUM_CHANNELS];
    float           om_mod      [NUM_CHANNELS];
    float           phi         [NUM_CHANNELS];
    float           phi_mod     [NUM_CHANNELS];
    float           amp         [NUM_CHANNELS];
    float           off         [NUM_CHANNELS];
    const uint8*    flash_ptr   [NUM_CHANNELS];
}
// parameter structure:
//         -> {{ paramX_CH1,        paramX_CH2,         paramX_CH3,         paramX_CH4 },...}
twMPI_params= {{723.57*OM_1HZ,    723.57*OM_1HZ,    16823.0*OM_1HZ,    16823.0*OM_1HZ},\
               {0.0*OM_1HZ,         0.0*OM_1HZ,         0.0*OM_1HZ,         0.0*OM_1HZ},\
               {0.0*PHI_90DEG,      1.0*PHI_90DEG,      0.0*PHI_90DEG,      0.0*PHI_90DEG},\
               {1.0*PHI_90DEG,      1.0*PHI_90DEG,      1.0*PHI_90DEG,      1.0*PHI_90DEG},\
               {0.47*MAX_VALUE,     0.47*MAX_VALUE,     0.47*MAX_VALUE,   0.47*MAX_VALUE},\
               {0.5*MAX_VALUE,      0.5*MAX_VALUE,      0.5*MAX_VALUE,    0.5*MAX_VALUE},\
               {FLASH_CH1,          FLASH_CH2,          FLASH_CH3,          FLASH_CH4}};

struct sequenceParams *twMPI = &twMPI_params;


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

// auxilliary variables
char    sms     [UART_BUF_OUT];
char    puttyIn [UART_BUF_IN];
int     rxBuf;
uint8   run_count_DAC_1=0;
uint8   nextRun = FALSE;

// USBUART (USBFS)
#define USBUART_BUFFER_SIZE (64u)
#define USBFS_DEVICE    (0u)
#define USBFS_TX_SIZE   60
uint16 count;
uint8 buffer[USBFS_TX_SIZE];
uint8 data_tx[4];


// Two adc buffers 
uint16 signal_adc_1[NSAMPLES_ADC]; 
uint16 signal_adc_2[NSAMPLES_ADC];

void init_components(void);
void show_default_message(void);
void show_channel_num(void);
void set_sequence_params(void);
void generate_sequence(void);
void run_sequence(char);
void display_results(void);
void uart_interface(void);
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


uint8 current_chan=0;
uint8 count_of_runs=0;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    init_components();
    if( *(FLASH_CH1 ) == 0 ) // Skip sequence generation if non-empty
        generate_sequence();
    show_default_message();
   
    for(;;) 
    {       
        // Control interface via UART for Putty or Matlab/Octave/Python
        uart_interface();
        usbfs_send_data();
    }
}
/* END MAIN() ***********************************/

void init_components(void)
{
    // ADDA Clock
    pwmSAMPLING_Start();
    
    // Transmit channels 
    IDAC8_1_Start();
        IDAC8_1_SetValue(MAX_VALUE/2); // Set dc value for avoiding base level fluctuations before first run (due to DC block capacitors)
        ShiftReg_1_WriteRegValue(CLOCK_SHIFT_CH1); // Shift Register are for avoiding simultaneous DMA requests
        ShiftReg_1_Start();
    IDAC8_2_Start();
        IDAC8_1_SetValue(MAX_VALUE/2);
        ShiftReg_2_WriteRegValue(CLOCK_SHIFT_CH2);
        ShiftReg_2_Start();
    IDAC8_3_Start();
        IDAC8_1_SetValue(MAX_VALUE/2);
        ShiftReg_3_WriteRegValue(CLOCK_SHIFT_CH3);
        ShiftReg_3_Start();
    IDAC8_4_Start();
        IDAC8_1_SetValue(MAX_VALUE/2);
        ShiftReg_4_WriteRegValue(CLOCK_SHIFT_CH4);
        ShiftReg_4_Start();

    // Receive channel
    ADC_SAR_1_Start();
        ADC_SAR_1_IRQ_Disable();
    ADC_SAR_2_Start();
        ADC_SAR_2_IRQ_Disable();
    Track_Hold_1_Start();
    Track_Hold_2_Start();
    
    refOut_Start();
    sigBuf_Start();
    
    // Components for user interface and debugging
    ChannelSel_Start();
        ChannelSel_Select(current_chan=4);
    UART_1_Start();
    CompTrigger_Start();
        isrTrigger_StartEx( isr_triggerIn );
    isr_DAC_1_StartEx( isr_DAC_1_done );
    isr_DAC_2_StartEx( isr_DAC_2_done );
    isr_DAC_3_StartEx( isr_DAC_3_done );
    isr_DAC_4_StartEx( isr_DAC_4_done );
    isr_ADC_1_StartEx( isr_ADC_1_done );
    isr_ADC_2_StartEx( isr_ADC_2_done );

    USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);
}

void show_default_message(void)
{
    UART_1_PutCRLF(2);
    sprintf(sms, "Press '%c' to run the sequence and show the results (ASCII table)", KEY_RUN_AND_SHOW);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "Press '%c' to only run the sequence (No ASCII table, currently measured: Channel %d)", KEY_RUN,  current_chan +1);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "Press '%c' to only run the alternating sequence (odd: CH3 is off, even: CH4 is off)", KEY_RUN_ALT);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "Press '%c' to show last results (ASCII table)", KEY_SEND_ASCII_DAT);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "Press '%c' to switch to next channel and run the sequence)", KEY_RUN_NEXT_SHOW);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "Press '%c', '%c', '%c' or '%c' for measuring DAC 1..4 or '%c' for measuring GPIO P0.7", KEY_DAC1, KEY_DAC2, KEY_DAC3, KEY_DAC4, KEY_SIG_IN );
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "Press '%c' to reset the device (Software reset)", KEY_RESET);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    UART_1_PutCRLF(2);
}

void show_channel_num(void)
{
    sprintf(sms,"Currently selected channel for data monitoring: %d", current_chan+1 );
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms,"Count of sequence runs after reset: %d", count_of_runs);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
}

void display_results(void)
{
    for(int i=0;i<NSAMPLES_ADC;i++)
    {
        sprintf(sms,"ADC_1 Value %d,0 us:\t  %d", i, signal_adc_1[i]);
            UART_1_PutString(sms);
        UART_1_PutCRLF(1);
        sprintf(sms,"ADC_2 Value %d,5 us:\t  %d", i, signal_adc_2[i]);
            UART_1_PutString(sms);
        UART_1_PutCRLF(1);
        
        // stop printing ascii table with any key:
        if( UART_1_GetRxBufferSize() != 0 )
        {
           UART_1_ClearRxBuffer();
           UART_1_PutCRLF(2);
           UART_1_PutString("Data listing aborted...\n\n");
           UART_1_PutCRLF(1);
           CyDelay(10);
           break;
        }
    }
    
    show_channel_num();
    
    show_default_message();  
}

void usbfs_send_data(void)
{
     /* Host can send double SET_INTERFACE request. */
    if (0u != USBUART_IsConfigurationChanged())
    {
        /* Initialize IN endpoints when device is configured. */
        if (0u != USBUART_GetConfiguration())
        {
            /* Enumeration is done, enable OUT endpoint to receive data 
             * from host. */
            USBUART_CDC_Init();
        }
    }

    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            /* Read received data and re-enable OUT endpoint. */
            count = USBUART_GetAll(buffer);

            if (0u != count)
            {
                LED_Write(1u);
                CyDelay(50);
                LED_Write(0u);
                
                /* Avoid interference with the UART_1 component */
                UART_1_ClearRxBuffer();
                UART_1_ClearTxBuffer();
                
                /* Wait until component is ready to send data to host. */
                while (0u == USBUART_CDCIsReady())
                {
                }
                
                if ( buffer[0] == KEY_DAC1 || buffer[0] == KEY_DAC2 || buffer[0] == KEY_DAC3 || buffer[0] == KEY_DAC4 || buffer[0] == KEY_SIG_IN)
                    ChannelSel_Select( buffer[0]-1 );
                if ( buffer[0] == KEY_RUN || buffer[0] == KEY_RUN_ALT)
                    run_sequence( buffer[0] );
                if ( buffer[0] == KEY_RESET )
                    CySoftwareReset(); // If Putty is used: this ends the session!
                    
                if ( buffer[0] == KEY_SEND_BYTE_DAT )
                {
                    LED_Write(1u);
                    // turn uint16 arrays into byte stream (ADC 1 and ADC 2 separate)
                    // ODD samples
                    for(int j=0;j<NSAMPLES_ADC/USBFS_TX_SIZE*DMA_ADC_1_BYTES_PER_BURST;j++)
                    {
                        while (0u == USBUART_CDCIsReady())
                        {
                        }                  
                        USBUART_PutData( ((uint8 *) (signal_adc_1)+(j*USBFS_TX_SIZE) ), USBFS_TX_SIZE);  
                    }
                    // EVEN samples
                    for(int j=0;j<NSAMPLES_ADC/USBFS_TX_SIZE*DMA_ADC_1_BYTES_PER_BURST;j++)
                    {
                        while (0u == USBUART_CDCIsReady())
                        {
                        }                  
                        USBUART_PutData( ((uint8 *) (signal_adc_2)+(j*USBFS_TX_SIZE) ), USBFS_TX_SIZE);  
                    }                    
                    LED_Write(0u);
                }

            }
        }
    }
} //usbfs_send_data()

void set_sequence_params(void)
{
    // clear token
    puttyIn[0]=0;
    puttyIn[1]=0;
    puttyIn[2]=0;
    
    LED_Write(1);
    
    for(int chan_i=0; chan_i<NUM_CHANNELS; chan_i++)
    {
        //om
        //om_mod
        //phi
        //phi_mod
        //amp
        twMPI->amp[0] = 50.0;// ((float) (100*(puttyIn[3]-'0') + 10*(puttyIn[4]-'0') * 1*(puttyIn[5]-'0')));
        //off
    }
    generate_sequence();
}

void generate_sequence(void)
{
    // Generate waveforms for each channel and store them in flash memory
    // (overrides 'signal_adc_1[]' for buffering the calculations)
    UART_1_PutString("* Generate sequence...");
    UART_1_PutCRLF(1);

    int time_i;
    for(int channel_i=0; channel_i<NUM_CHANNELS; channel_i++)
    {
        time_i=0-1;
        // A) ramp up sequence
        for(int i=0;i<NSAMPLES_DAC;i++)
        {
            time_i++;
            ((uint8*)signal_adc_1)[time_i]\
                    =(uint8) (1.0/((float)NSAMPLES_DAC) * (float)time_i * (float)twMPI->amp[channel_i]\
                    * sin( (float) twMPI->om[channel_i] * (float)time_i + (float)twMPI->phi[channel_i])\
                    * sin( (float) twMPI->om_mod[channel_i] * (float)time_i + (float)twMPI->phi_mod[channel_i])\
                    + (float)twMPI->off[channel_i]);
        }   
        //FLASH_Write( (uint8*)signal_adc_1, twMPI->flash_ptr[channel_i], NSAMPLES_DAC); 
        // B) sequence
        for(int i=0;i<NSAMPLES_DAC;i++)
        {
            time_i++;
            ((uint8*)signal_adc_1)[time_i]\
                    = (uint8) ((float)twMPI->amp[channel_i]\
                    * sin( (float) twMPI->om[channel_i] * ((float)time_i) + (float)twMPI->phi[channel_i])\
                    * sin( (float) twMPI->om_mod[channel_i] * ((float)time_i) + (float)twMPI->phi_mod[channel_i])\
                    + (float)twMPI->off[channel_i]);
        }   
        //FLASH_Write( (uint8*)signal_adc_1, twMPI->flash_ptr[channel_i] + NSAMPLES_DAC, NSAMPLES_DAC); 
        // C) ramp down sequence
        for(int i=0;i<NSAMPLES_DAC;i++)
        {
            time_i++;
            ((uint8*)signal_adc_1)[time_i]\
                    = (uint8) ((float)twMPI->amp[channel_i] * ((float)(((float)NSAMPLES_DAC-((float)time_i - (2*(float)NSAMPLES_DAC)  ))/((float)NSAMPLES_DAC)))\
                    * sin( (float) twMPI->om[channel_i] * ((float)time_i) + (float)twMPI->phi[channel_i])\
                    * sin( (float) twMPI->om_mod[channel_i] * ((float)time_i) + (float)twMPI->phi_mod[channel_i])\
                    + (float)twMPI->off[channel_i]);
        }   
        *(uint8*)signal_adc_1 = WAVE_EXIST; // token to indicate that a valid wave form allready exists in flash memory to avoid recalculation after reset
        FLASH_Write( (uint8*)signal_adc_1, twMPI->flash_ptr[channel_i], 3*NSAMPLES_DAC); 
    }
    // Clear 'signal_adc_1[]'
    for(int i=0;i<3*NSAMPLES_DAC;i++)
    {
        ((uint8*)signal_adc_1)[i] = 0;
    } 
    UART_1_PutString("* Sequence ready.");
    UART_1_PutCRLF(1);

}

void run_sequence(char selSequ)
{
    internTrigger_Write(STOP_CLOCK);
    
        ShiftReg_1_Stop(); ShiftReg_1_WriteRegValue(CLOCK_SHIFT_CH1); ShiftReg_1_Start();
        ShiftReg_2_Stop(); ShiftReg_2_WriteRegValue(CLOCK_SHIFT_CH2); ShiftReg_2_Start();
        ShiftReg_3_Stop(); ShiftReg_3_WriteRegValue(CLOCK_SHIFT_CH3); ShiftReg_3_Start();
        ShiftReg_4_Stop(); ShiftReg_4_WriteRegValue(CLOCK_SHIFT_CH4); ShiftReg_4_Start();

        dma_dac_1_init();
        dma_dac_2_init();
        if( selSequ == KEY_RUN_ALT)
        {
            if( (count_of_runs%2) == 0)  dma_dac_3_init(); // count_of_runs = EVEN
            else                         dma_dac_4_init(); // count_of_runs = ODD
        }
        else
        {
            dma_dac_3_init();
            dma_dac_4_init();
        }
        dma_adc_1_init();
        dma_adc_2_init();
        
    internTrigger_Write(START_CLOCK);
    
    count_of_runs++;
}


void uart_interface(void)
{
    nextRun = TRUE; // used to avoid extern trigger from blocking the userinterface
    // Control interface via UART for Putty or Matlab/Octave
    if( (rxBuf = UART_1_GetRxBufferSize())!=0 )
    {
        // 1) Read Terminal Input String into puttyIn[]
        for(int char_i=0; UART_1_GetRxBufferSize() !=0; char_i++)
        {
            puttyIn[char_i] = UART_1_GetChar();
            
            LED_Write( 1u ); // indicate received chars
            CyDelay(50);
            LED_Write( 0u );
            CyDelay(50);
        }
        
        // Set new sequence parameters via uart
        if( puttyIn[0] == 't' && puttyIn[1] == 't' && puttyIn[2] == 't')
        {
            set_sequence_params();
        }   
        
        // 3) Putty user interface
        switch( puttyIn[0] )
        {
            case KEY_RUN_AND_SHOW: // run sequence and show results (2*NSAMPLES_ADC lines, ASCII formatted)
                run_sequence(puttyIn[0]);
                display_results();
                show_channel_num();
            break;
                
            case KEY_RUN: // run sequence (just running, no output)
                run_sequence(puttyIn[0]);
            break;
                
            case KEY_RUN_ALT: // run sequence - alternating channel 3 and 4 per run  (just running, no output)
                run_sequence(puttyIn[0]);
            break;
            // 'o' : trigger streaming ADC data in binary form
            //          for showing data with Matlab/Octave instead of Putty
            case KEY_SEND_BYTE_DAT:
                LED_Write( 1u ); 
                for(int j=0;j<NSAMPLES_ADC;j++)
                {
                    // turn uint16 arrays into uint8 streams:
                    UART_1_PutChar(signal_adc_1[j] >>8   );
                    UART_1_PutChar(signal_adc_1[j] &0xFF ); 
                    UART_1_PutChar(signal_adc_2[j] >>8   );
                    UART_1_PutChar(signal_adc_2[j] &0xFF ); 
                }
                LED_Write( 0u );
            break;
                
            case KEY_RUN_NEXT_SHOW: // run sequence and switch to next channel (just running, no output)
                current_chan++;
                if(current_chan>4) current_chan=0;
                ChannelSel_Select(current_chan);
                run_sequence(puttyIn[0]);
                show_channel_num();
            break;
            
            case KEY_DAC1: // Read signal from DAC 1
                current_chan=0;
                ChannelSel_Select(current_chan);
                show_channel_num();
            break;
                
            case KEY_DAC2: // DAC 2
                current_chan=1;
                ChannelSel_Select(current_chan);
                show_channel_num();
            break;
                
            case KEY_DAC3: // DAC 3
                current_chan=2;
                ChannelSel_Select(current_chan);
                show_channel_num();
            break;
                
            case KEY_DAC4: // DAC 4
                current_chan=3;
                ChannelSel_Select(current_chan);
                show_channel_num();
            break;
                
            case KEY_SIG_IN: // Read Signal from GPIO P0.7
                current_chan=4;
                ChannelSel_Select(current_chan);
                show_channel_num();
            break;

            case KEY_SEND_ASCII_DAT: // Give ADC data stream in ASCII format (for display in e.g. Putty)
                 display_results();
            break;
                
            case KEY_RESET: // RESET
                 CySoftwareReset();   
            break;

             
        }

    }
}

void dma_dac_1_init(void)
{
    /* DMA Configuration for DMA_DAC_1 */
    if( count_of_runs == 0)
    {
        DMA_DAC_1_Chan = DMA_DAC_1_DmaInitialize(DMA_DAC_1_BYTES_PER_BURST, DMA_DAC_1_REQUEST_PER_BURST, 
            HI16(DMA_DAC_1_SRC_BASE), HI16(DMA_DAC_1_DST_BASE));
        DMA_DAC_1_TD[0] = CyDmaTdAllocate();
        DMA_DAC_1_TD[1] = CyDmaTdAllocate();
        DMA_DAC_1_TD[2] = CyDmaTdAllocate();
    }
    CyDmaTdSetConfiguration(DMA_DAC_1_TD[0], NSAMPLES_DAC, DMA_DAC_1_TD[1], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_DAC_1_TD[1], NSAMPLES_DAC, DMA_DAC_1_TD[2], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_DAC_1_TD[2], NSAMPLES_DAC, CY_DMA_DISABLE_TD, DMA_DAC_1__TD_TERMOUT_EN | CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_DAC_1_TD[0], LO16((uint32)FLASH_CH1), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_1_TD[1], LO16((uint32)FLASH_CH1 + NSAMPLES_DAC), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_1_TD[2], LO16((uint32)FLASH_CH1 + 2*NSAMPLES_DAC), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaChSetInitialTd(DMA_DAC_1_Chan, DMA_DAC_1_TD[0]);
    CyDmaChEnable(DMA_DAC_1_Chan, 1);
}

void dma_dac_2_init(void)
{
    /* DMA Configuration for DMA_DAC_2 */
    if( count_of_runs == 0)
    {
        DMA_DAC_2_Chan = DMA_DAC_2_DmaInitialize(DMA_DAC_2_BYTES_PER_BURST, DMA_DAC_2_REQUEST_PER_BURST, 
            HI16(DMA_DAC_2_SRC_BASE), HI16(DMA_DAC_2_DST_BASE));
        DMA_DAC_2_TD[0] = CyDmaTdAllocate();
        DMA_DAC_2_TD[1] = CyDmaTdAllocate();
        DMA_DAC_2_TD[2] = CyDmaTdAllocate();
    }
    CyDmaTdSetConfiguration(DMA_DAC_2_TD[0], NSAMPLES_DAC, DMA_DAC_2_TD[1], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_DAC_2_TD[1], NSAMPLES_DAC, DMA_DAC_2_TD[2], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_DAC_2_TD[2], NSAMPLES_DAC, CY_DMA_DISABLE_TD, DMA_DAC_2__TD_TERMOUT_EN | CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_DAC_2_TD[0], LO16((uint32)FLASH_CH2), LO16((uint32)IDAC8_2_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_2_TD[1], LO16((uint32)FLASH_CH2 + NSAMPLES_DAC), LO16((uint32)IDAC8_2_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_2_TD[2], LO16((uint32)FLASH_CH2 + 2*NSAMPLES_DAC), LO16((uint32)IDAC8_2_Data_PTR));
    CyDmaChSetInitialTd(DMA_DAC_2_Chan, DMA_DAC_2_TD[0]);
    CyDmaChEnable(DMA_DAC_2_Chan, 1);
}

void dma_dac_3_init(void)
{
    /* DMA Configuration for DMA_DAC_3 */
    if( count_of_runs == 0)
    {
        DMA_DAC_3_Chan = DMA_DAC_3_DmaInitialize(DMA_DAC_3_BYTES_PER_BURST, DMA_DAC_3_REQUEST_PER_BURST, 
            HI16(DMA_DAC_3_SRC_BASE), HI16(DMA_DAC_3_DST_BASE));
        DMA_DAC_3_TD[0] = CyDmaTdAllocate();
        DMA_DAC_3_TD[1] = CyDmaTdAllocate();
        DMA_DAC_3_TD[2] = CyDmaTdAllocate();
    }
    CyDmaTdSetConfiguration(DMA_DAC_3_TD[0], NSAMPLES_DAC, DMA_DAC_3_TD[1], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_DAC_3_TD[1], NSAMPLES_DAC, DMA_DAC_3_TD[2], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_DAC_3_TD[2], NSAMPLES_DAC, CY_DMA_DISABLE_TD, DMA_DAC_3__TD_TERMOUT_EN | CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_DAC_3_TD[0], LO16((uint32)FLASH_CH3), LO16((uint32)IDAC8_3_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_3_TD[1], LO16((uint32)FLASH_CH3 + NSAMPLES_DAC), LO16((uint32)IDAC8_3_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_3_TD[2], LO16((uint32)FLASH_CH3 + 2*NSAMPLES_DAC), LO16((uint32)IDAC8_3_Data_PTR));
    CyDmaChSetInitialTd(DMA_DAC_3_Chan, DMA_DAC_3_TD[0]);
    CyDmaChEnable(DMA_DAC_3_Chan, 1);
}

void dma_dac_4_init(void)
{
    /* DMA Configuration for DMA_DAC_4 */
    if( count_of_runs == 0)
    {
        DMA_DAC_4_Chan = DMA_DAC_4_DmaInitialize(DMA_DAC_4_BYTES_PER_BURST, DMA_DAC_4_REQUEST_PER_BURST, 
            HI16(DMA_DAC_4_SRC_BASE), HI16(DMA_DAC_4_DST_BASE));
        DMA_DAC_4_TD[0] = CyDmaTdAllocate();
        DMA_DAC_4_TD[1] = CyDmaTdAllocate();
        DMA_DAC_4_TD[2] = CyDmaTdAllocate();
    }
    CyDmaTdSetConfiguration(DMA_DAC_4_TD[0], NSAMPLES_DAC, DMA_DAC_4_TD[1], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_DAC_4_TD[1], NSAMPLES_DAC, DMA_DAC_4_TD[2], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_DAC_4_TD[2], NSAMPLES_DAC, CY_DMA_DISABLE_TD, DMA_DAC_4__TD_TERMOUT_EN | CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_DAC_4_TD[0], LO16((uint32)FLASH_CH4), LO16((uint32)IDAC8_4_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_4_TD[1], LO16((uint32)FLASH_CH4 + NSAMPLES_DAC), LO16((uint32)IDAC8_4_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_4_TD[2], LO16((uint32)FLASH_CH4 + 2*NSAMPLES_DAC), LO16((uint32)IDAC8_4_Data_PTR));
    CyDmaChSetInitialTd(DMA_DAC_4_Chan, DMA_DAC_4_TD[0]);
    CyDmaChEnable(DMA_DAC_4_Chan, 1);
}

void dma_adc_1_init(void)
{
    /* DMA Configuration for DMA_ADC_1 */
    if( count_of_runs == 0)
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
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[0], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[1], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[1], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[2], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[2], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[3], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[3], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[4], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[4], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[5], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[5], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[6], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[6], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[7], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[7], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[8], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[8], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[9], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[9], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[10],  CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[10], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[11], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[11], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[12], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[12], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[13], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[13], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[14], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[14], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[15], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[15], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[16], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[16], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[17], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[17], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[18], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[18], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, DMA_ADC_1_TD[19], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_1_TD[19], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST, CY_DMA_DISABLE_TD,  DMA_ADC_1__TD_TERMOUT_EN | (CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART));
    CyDmaTdSetAddress(DMA_ADC_1_TD[0], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(0*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[1], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(1*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[2], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(2*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[3], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(3*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[4], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(4*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[5], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(5*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[6], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(6*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[7], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(7*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[8], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(8*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[9], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(9*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[10], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(0*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[11], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(1*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[12], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(2*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[13], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(3*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[14], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(4*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[15], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(5*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[16], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(6*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[17], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(7*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[18], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(8*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_1_TD[19], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)signal_adc_1 +(9*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_1_BYTES_PER_BURST)));
    CyDmaChSetInitialTd(DMA_ADC_1_Chan, DMA_ADC_1_TD[0]);
    CyDmaChEnable(DMA_ADC_1_Chan, 1);     
}

void dma_adc_2_init(void)
{
    /* DMA Configuration for DMA_ADC_2 */
    if( count_of_runs == 0)
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
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[0], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[1], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[1], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[2], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[2], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[3], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[3], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[4], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[4], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[5], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[5], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[6], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[6], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[7], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[7], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[8], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[8], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[9], CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[9], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[10],  CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[10], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[11], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[11], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[12], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[12], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[13], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[13], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[14], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[14], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[15], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[15], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[16], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[16], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[17], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[17], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[18], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[18], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, DMA_ADC_2_TD[19], CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART);
    CyDmaTdSetConfiguration(DMA_ADC_2_TD[19], NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST, CY_DMA_DISABLE_TD,  DMA_ADC_2__TD_TERMOUT_EN | (CY_DMA_TD_INC_DST_ADR * IGNORE_FIRST_PART));
    CyDmaTdSetAddress(DMA_ADC_2_TD[0], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(0*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[1], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(1*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[2], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(2*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[3], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(3*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[4], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(4*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[5], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(5*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[6], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(6*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[7], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(7*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[8], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(8*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[9], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(9*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[10], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(0*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[11], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(1*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[12], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(2*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[13], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(3*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[14], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(4*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[15], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(5*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[16], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(6*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[17], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(7*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[18], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(8*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaTdSetAddress(DMA_ADC_2_TD[19], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)signal_adc_2 +(9*NSAMPLES_ADC/(N_TDS_ADC/2) * DMA_ADC_2_BYTES_PER_BURST)));
    CyDmaChSetInitialTd(DMA_ADC_2_Chan, DMA_ADC_2_TD[0]);
    CyDmaChEnable(DMA_ADC_2_Chan, 1);     
}

CY_ISR( isr_triggerIn )
{
    if( nextRun == TRUE )
    {
        nextRun = FALSE; // avoid blocking Putty user interface 
        LED_Write( ~LED_Read()); // indicate trigger
        run_sequence(KEY_RUN); 
        CyDelayUs(SEQU_DURATION_US); // Block CPU while sequence is running to avoid interference
    }
}


CY_ISR( isr_DAC_1_done )
{

    //display_results();
    
    //UART_1_PutString("DAC_1 done!");
    //UART_1_PutCRLF(1);
    
}

CY_ISR( isr_DAC_2_done )
{
    //CyDelay(10);
    //UART_1_PutString("DAC_2 done!");
    //UART_1_PutCRLF(1);
}

CY_ISR( isr_DAC_3_done )
{
    //CyDelay(10);
    //UART_1_PutString("DAC_3 done!");
    //UART_1_PutCRLF(1);
}

CY_ISR( isr_DAC_4_done )
{
    //CyDelay(10);
    //UART_1_PutString("DAC_4 done!");
    //UART_1_PutCRLF(1);
}

CY_ISR( isr_ADC_1_done )
{
    //CyDelay(10);
    //UART_1_PutString("ADC_1 done!");
    //UART_1_PutCRLF(1);
}

CY_ISR( isr_ADC_2_done )
{
    //CyDelay(10);
    //UART_1_PutString("ADC_2 done!");
    //UART_1_PutCRLF(1);
}


/* [] END OF FILE */
