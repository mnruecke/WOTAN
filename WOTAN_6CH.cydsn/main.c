/* ========================================
 * 
 * This code is supplementary material for the IWMPI 2018, Hamburg, Germany 
 *
 * Martin.Rueckert@physik.uni-wuerzburg.de
 *
 * Copyright (C) 2017-2021 University of Wuerzburg, Experimental Physics 5, Biophysics
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


// Using the internal oscillator "IMO":
//
// to run without external crystal: go to  "Design Wide Resources(WOTAN.cydwr)" in Workspace Explorer (left)
//              goto the tab "WOTAN.cydwr" -> go to "clocks" (tabs on the bottom) -> click anywhere inside 
//              the yellow spread sheet -> change the roll-down menue in "PLL" (box in the middle)
//              to "IMO(24 MHz)" -> recompile and program (CTRL + F5)

#include "project.h"// automatically generated header file
#include <stdio.h>
#include <string.h>
#include <math.h>// => requires 'm' in: Project|Build Settings... -> ARM GCC ... -> Linker -> General -> Additional Libraries -> m

char  version[3] = "1.7";  
// Version 1.7: allowing to split CH1 and CH2 into CH1a/CH1b and CH2a/CH2b
// by halfing the sampling rate and using interleaved sampling patterns

#define  TRUE               1
#define  FALSE              0

#define START_CLOCK         0
#define STOP_CLOCK          1


#define  UART_BUF_IN         100
#define  UART_BUF_OUT        80

/* command set */
// 1) select channel

#define  KEY_SIG_1           '1'
#define  KEY_SIG_2           '2'
#define  KEY_SIG_3           '3'
#define  KEY_SIG_4           '4'
#define  KEY_SIG_5           '5'
#define  KEY_SIG_6           '6'
#define  KEY_SIG_MAIN        '7'
// 2) run sequence
#define  KEY_RUN             'r'
// 3) reset firmware
#define  KEY_RESET           'e'
// 4) set VDAC output range to 1V (default, voltage DACs only, comment code out when using current DACs)
#define  KEY_VDAC_1V         'l'
// 5) set VDAC output range to 4V (voltage DACs only, comment code out when using current DACs)
#define  KEY_VDAC_4V         'h'
// 6) writes new sequence
#define  KEY_WRITE_SEQUENCE  'p'
// 7) Use gpio P3[0] as trigger output
#define  KEY_TRIGGER_OUT     'x'
// 8) Use gpio P3[0] as trigger input
#define  KEY_TRIGGER_IN      'y'
// 9) firmware infomation
#define  KEY_VERSION         'V'
// 10) chip information
#define  KEY_SERIAL_NUMBER   'S'
// 11) Get the binary data from the two uint16 ADC buffers
#define  KEY_SEND_BYTE_DAT   'o'
// 12) (uart only) give ADC data stream in ASCII format (for display in e.g. Putty)
#define  KEY_SEND_ASCII_DAT  'd'
// 13) (uart only) run sequence and show results (2*NSAMPLES_ADC lines, ASCII formatted) via UART
#define  KEY_RUN_AND_SHOW    's'
// 14) (uart only) run sequence and swithes to the next channel (just running, no output)
#define KEY_RUN_AND_NEXT     'a'

// 15) new: Shift waveforms in multiples of sampling time (usage: 'O<channel-pair><16bit-offset>')
#define KEY_SHIFT_WAVEFORMS  'O'

//Alex Testing Commands:
// 16) separates ChargerOut- and GND via setting PWR_EN to 0
#define KEY_POWER_OFF        'f'
// 17) Turns Off the DCDC_Converter
#define KEY_DCDC_OFF         'D'
//18) Turns LED on/off
#define KEY_LED              'L'


/* end command set */

#define  TRIGGER_OUT_TRUE    1
#define  TRIGGER_OUT_FALSE   0

#define  NSAMPLES_ADC       15000               // 1 MS/s, max value: 15000
#define  NSAMPLES_DAC       NSAMPLES_ADC/4      // 250 kS/s (make sample duration for Transmit and Receive the same)
#define  SEQU_DURATION_US   NSAMPLES_ADC*3      // duration includes start- and end ramp

#define  IGNORE_FIRST_PART  TRUE               // if FALSE: shows start ramp of sequence (default: TRUE)

#define  N_TDS_ADC          20
#define  N_TDS_DAC          3               // TD1: on ramp, TD2: sequence (each with length NSAMPLES_DAC)


#define  FLASH_CH1          (const uint8 *)     0x20000  // Flash addresses for storing the DAC wave forms
#define  FLASH_CH2          (const uint8 *)     0x28000
#define  FLASH_CH3          (const uint8 *)     0x30000
#define  FLASH_CH4          (const uint8 *)     0x38000 
const uint8 * FLASH_STORAGE[4] = {FLASH_CH1, FLASH_CH2, FLASH_CH3, FLASH_CH4};
uint32 flash_offset_ch1 = 0; //Shifts wave form of CHX n sampling steps
uint32 flash_offset_ch2 = 0; 
uint32 flash_offset_ch3 = 0; 
uint32 flash_offset_ch4 = 0; 


#define  CLOCK_SHIFT_CH1     0b1100   
// channel splitting (CH1 -> CH1a / CH1b)
#define  CLOCK_SHIFT_CH1ab    0b01111000
#define  CLOCK_SHIFT_CH2     0b0011
// channel splitting (CH2 -> CH2a / CH2b)
#define  CLOCK_SHIFT_CH2ab    0b10000111
#define  CLOCK_SHIFT_CH3     0b0110   
#define  CLOCK_SHIFT_CH4     0b1001   


// Set all MPI waveform parameters
#define MAX_VALUE       254 
#define IDLE_VALUE_CH1  127
#define IDLE_VALUE_CH2  127
#define IDLE_VALUE_CH3  127
#define IDLE_VALUE_CH4  127


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
char    sms         [UART_BUF_OUT];
char    puttyIn     [UART_BUF_IN];
uint16  bytenumber = 0;
int     rxBuf;
int     rxBufBLE;
uint8   run_count_DAC_1=0;
uint8   ready_to_start_sequence = FALSE;


// USBUART (USBFS)
#define USBUART_BUFFER_SIZE 64u //Original
#define USBFS_DEVICE    (0u)
#define USBFS_TX_SIZE   60    // exakt package size
uint16 count;
uint8 buffer[USBFS_TX_SIZE];


// programm sequence
const uint size_of_header  = 8;
const uint size_of_segment = 50;
uint packages_received = 0;


// Two adc buffers 
uint16 signal_adc_1[NSAMPLES_ADC]; 
uint16 signal_adc_2[NSAMPLES_ADC];


// function prototypes
void init_components(void);
void show_default_message(void);
void show_channel_num(void);
void run_sequence(void);
void display_results(void);
void uart_interface(void);
void usbfs_interface(void);
void dma_dac_1_init(void);
void dma_dac_2_init(void);
void dma_dac_3_init(void);
void dma_dac_4_init(void);
void dma_adc_1_init(void);
void dma_adc_2_init(void);
void set_dac_range_1V(void);
void set_dac_range_4V(void);
void trigger_out(void);
void trigger_in(void);


CY_ISR_PROTO( isr_triggerIn );
CY_ISR_PROTO( isr_DAC_1_done );
CY_ISR_PROTO( isr_DAC_2_done );
CY_ISR_PROTO( isr_DAC_3_done );
CY_ISR_PROTO( isr_DAC_4_done );
CY_ISR_PROTO( isr_ADC_1_done );
CY_ISR_PROTO( isr_ADC_2_done );


uint8 current_chan=0;
uint8 count_of_runs=0;
uint8 isDAC1Busy = FALSE;


int main(void){
    // Initialization routines
    init_components();
    
    show_default_message();
    
    // Avoid errorness serial input due to initial switching noise:
    CyDelay(500);
    UART_1_ClearRxBuffer();
      
    for(;;){
        // uart interface
        uart_interface();  // for using BLE module HC-05 via UART interface
        
        // fast usbfs iterface
        usbfs_interface(); // for using fast USBUART routed to the onboard Micro-USB-B socket
        
    }//endfor(;;)
}/* END MAIN() ***********************************/



void init_components(void){
    CyGlobalIntEnable;    
    
    // Init code for splitting CH1 into CH1a and CH1b (halfing the sampling rate)
    sample_CH1a_Start(); 
    sample_CH1b_Start();
    sample_CH2a_Start(); 
    sample_CH2b_Start();
    ClockShift_1ab_WriteRegValue(CLOCK_SHIFT_CH1ab);
    ClockShift_1ab_Start();  
    ClockShift_2ab_WriteRegValue(CLOCK_SHIFT_CH2ab);
    ClockShift_2ab_Start(); 
    
    // Buffer CH1, CH2
    Buffer_1_Start();
    Buffer_2_Start();
    
    set_dac_range_4V();
    
    // ADDA Clock
    pwmSAMPLING_Start();
    
    // Transmit channels 
    IDAC8_1_Start();
        IDAC8_1_SetValue(IDLE_VALUE_CH1); // Set dc value for avoiding base level fluctuations before first run (due to DC block capacitors)
        ClockShift_1_WriteRegValue(CLOCK_SHIFT_CH1); // Shift Register are for avoiding simultaneous DMA requests
        ClockShift_1_Start();
    IDAC8_2_Start();
        IDAC8_2_SetValue(IDLE_VALUE_CH2);
        ClockShift_2_WriteRegValue(CLOCK_SHIFT_CH2);
        ClockShift_2_Start();
    IDAC8_3_Start();
        IDAC8_3_SetValue(IDLE_VALUE_CH3);
        ClockShift_3_WriteRegValue(CLOCK_SHIFT_CH3);
        ClockShift_3_Start();
    IDAC8_4_Start();
        IDAC8_4_SetValue(IDLE_VALUE_CH4);
        ClockShift_4_WriteRegValue(CLOCK_SHIFT_CH4);
        ClockShift_4_Start();
   
    //Waveforms in flash need to be extendet with dac idle values
    // for 256 additional steps to allow wave shifting of up to 256 steps 
    // (regarding command KEY_SHIFT_WAVEFORMS)
    uint8 dac_idle_ch1[NSAMPLES_ADC];
    uint8 dac_idle_ch2[NSAMPLES_ADC];
    uint8 dac_idle_ch3[NSAMPLES_ADC];
    uint8 dac_idle_ch4[NSAMPLES_ADC];
    if( *(FLASH_CH4+(NSAMPLES_ADC-1)) != IDLE_VALUE_CH4 )
    {// -> write to flash only once after programming
        LED_Write(1u);
        
        for(int i=0;i<(NSAMPLES_ADC/2);i++){
            dac_idle_ch1[i] = IDLE_VALUE_CH1;
            dac_idle_ch2[i] = IDLE_VALUE_CH2;
            dac_idle_ch3[i] = IDLE_VALUE_CH3;
            dac_idle_ch4[i] = IDLE_VALUE_CH4;
        }//END for(int i=0;i<(NSAMPLES_ADC/2);i++) 
        FLASH_Write( dac_idle_ch1, FLASH_CH1+(NSAMPLES_ADC/2), NSAMPLES_ADC);    
        FLASH_Write( dac_idle_ch2, FLASH_CH2+(NSAMPLES_ADC/2), NSAMPLES_ADC);    
        FLASH_Write( dac_idle_ch3, FLASH_CH3+(NSAMPLES_ADC/2), NSAMPLES_ADC);    
        FLASH_Write( dac_idle_ch4, FLASH_CH4+(NSAMPLES_ADC/2), NSAMPLES_ADC); 
        
        //CyDelay(10);
        LED_Write(0u);
    }//END if( *(FLASH_CH2+(3*NSAMPLES_DAC-1)) != IDLE_VALUE_CH2_CH3_CH4 )
    

    // Receive channel
    ADC_SAR_1_Start();
        ADC_SAR_1_IRQ_Disable();
    ADC_SAR_2_Start();
        ADC_SAR_2_IRQ_Disable();
    sigBuf_Start();
    
    // Sets the Trigger channel as input
    CompTrigger_Stop();
    enableTrigOut_Write( TRIGGER_OUT_TRUE );
    
    // Components for user interface and debugging
    ChannelSel_Start();
        // use real input by default
        ChannelSel_Select(current_chan=KEY_SIG_MAIN-1-'0');
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

void usbfs_interface(void)
{
    uint number_of_packages;
    uint package_number;
    uint number_of_samples;
    uint channel_number;
    char * wave_segment_ptr;
    
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
                /* Avoid interference with the UART_1 component */
                UART_1_ClearRxBuffer();
                UART_1_ClearTxBuffer();
                
                /* Wait until component is ready to send data to host. */
                while (0u == USBUART_CDCIsReady())
                {
                }
                
                /* Process firmware commands */
                
                
                // 1) select a channel
                if (    buffer[0] == KEY_SIG_1 ||\
                        buffer[0] == KEY_SIG_2 ||\
                        buffer[0] == KEY_SIG_3 ||\
                        buffer[0] == KEY_SIG_4 ||\
                        buffer[0] == KEY_SIG_5 ||\
                        buffer[0] == KEY_SIG_6 ||\
                        buffer[0] == KEY_SIG_MAIN	){
                    current_chan = buffer[0]-1-'0';
                    ChannelSel_Select( current_chan);
                    //show_channel_num();
                }//END if ( buffer[0]
                // 2) run sequence
                if ( buffer[0] == KEY_RUN)
                    run_sequence();
                // 3) reset firmware 
                if ( buffer[0] == KEY_RESET )
                    CySoftwareReset(); // If Putty is used: this ends the session!
                // 4) set VDAC output range to 1V (default, voltage DACs only, comment code out when using current DACs)
                if ( buffer[0] == KEY_VDAC_1V )
                    set_dac_range_1V();

                // 5) set VDAC output range to 4V (voltage DACs only, comment code out when using current DACs)
                if ( buffer[0] == KEY_VDAC_4V )
                    set_dac_range_4V();
                    
                // 6) writes new sequence
                if ( buffer[0] == KEY_WRITE_SEQUENCE )
                {
                    // get parameters:
                    number_of_packages  = (256*buffer[4]+buffer[5]);
                    package_number      = (256*buffer[2]+buffer[3]);
                    number_of_samples   = number_of_packages*size_of_segment;
                    channel_number      = buffer[1];
                    
                    // write wave into flash memory:
                    wave_segment_ptr    = ((char *) signal_adc_1) + size_of_segment * package_number;
                    memcpy(  wave_segment_ptr, (char *) &buffer[size_of_header], size_of_segment );                  
                    if( package_number == (number_of_packages-1) )
                    {
                        FLASH_Write( (uint8*)signal_adc_1, FLASH_STORAGE[channel_number], number_of_samples-20);
                    }
                }
                // 7) Use gpio P3[0] as trigger output
                if ( buffer[0] == KEY_TRIGGER_OUT )             
                { 
                    trigger_out();
                }
                // 8) Use gpio P3[0] as trigger input
                if ( buffer[0] == KEY_TRIGGER_IN )             
                { 
                    trigger_in();
                }     
                // 9) Firmware information
                
                if ( buffer[0] == KEY_VERSION )
                {
                    while (0u == USBUART_CDCIsReady());
                    USBUART_PutData( (uint8 *) version , 3);
                }
                // 10) Chip information
                int strlength = 34;
                char pseudoid[strlength];
                if ( buffer[0] == KEY_SERIAL_NUMBER )
                {
                    sprintf( pseudoid, "                                  ");
                    sprintf( pseudoid, "%3d %3d %3d %3d %3d %3d %3d",\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_Y_LOC,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_X_LOC,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_WAFER_NUM,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_LOT_LSB,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_LOT_MSB,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_WRK_WK,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_FAB_YR);
                    
                    while (0u == USBUART_CDCIsReady());
                    USBUART_PutData( (uint8 *)pseudoid , strlength);
                    
                }
                
                // 11a) Shift waveforms in multiples of sampling time
                if( buffer[0] == KEY_SHIFT_WAVEFORMS ){
                    if( buffer[1] == '1' ){// Shift channel CH1
                        flash_offset_ch1 = (buffer[2]<<8)+buffer[3];
                    }else if( buffer[1] == '2' ){// Shift channel CH2
                        flash_offset_ch2 = (buffer[2]<<8)+buffer[3];
                    }else if( buffer[1] == '3' ){// Shift channel CH3
                        flash_offset_ch2 = (buffer[2]<<8)+buffer[3];
                    }else if( buffer[1] == '4' ){// Shift channel CH4
                        flash_offset_ch2 = (buffer[2]<<8)+buffer[3];
                    }else if( buffer[1] == 'A' ){// Shift all channels
                        flash_offset_ch1  = (buffer[2]<<8)+buffer[3];
                        flash_offset_ch2  = (buffer[2]<<8)+buffer[3];
                        flash_offset_ch3  = (buffer[2]<<8)+buffer[3];
                        flash_offset_ch4  = (buffer[2]<<8)+buffer[3];
                    }
                }//END if( buffer[0] == KEY_SHIFT_WAVEFORMS )
                
                
                // 11) Get the binary data from the two uint16 ADC buffers
                if ( buffer[0] == KEY_SEND_BYTE_DAT )
                {
                    uint8 * adc1_ptr = (uint8 *) (signal_adc_1);
                    uint8 * adc2_ptr = (uint8 *) (signal_adc_2);
                    uint8 adc1_adc2_interleaved[USBFS_TX_SIZE];
                    
                    LED_Write(1u);
                    // turn uint16 arrays into byte stream (ADC 1 and ADC 2 separate)
                    for(int j=0;j<2*NSAMPLES_ADC/USBFS_TX_SIZE*DMA_ADC_1_BYTES_PER_BURST;j++)
                    {
                        // a) create data packet fitting in usb tx buffer
                        for(int m=0; m<=USBFS_TX_SIZE/4; m++)
                        {
                            adc1_adc2_interleaved[4*m+0]=*( adc1_ptr + ((j)*USBFS_TX_SIZE/2+(2*m+1)) );                           
                            adc1_adc2_interleaved[4*m+1]=*( adc1_ptr + ((j)*USBFS_TX_SIZE/2+(2*m+0)) );                           
                            adc1_adc2_interleaved[4*m+2]=*( adc2_ptr + ((j)*USBFS_TX_SIZE/2+(2*m+1)) );                           
                            adc1_adc2_interleaved[4*m+3]=*( adc2_ptr + ((j)*USBFS_TX_SIZE/2+(2*m+0)) );                           
                        }
                        
                        // b) send
                        while (0u == USBUART_CDCIsReady())
                        {
                        }                  
                        USBUART_PutData( adc1_adc2_interleaved , USBFS_TX_SIZE);  
                    }
                    LED_Write(0u);
                }// END send binary adc data


		        // Alex Test-Commands (can be deleted later on)
                if ( buffer[0] == KEY_DCDC_OFF )
                {
                    P_DCDC_CTRL_Write(!P_DCDC_CTRL_Read());
                    UART_1_PutString("DCDC");
                }
                
                if ( buffer[0] == KEY_POWER_OFF )
                {
                    P_PWR_EN_Write(!P_PWR_EN_Read());
                    UART_1_PutString("OFF");
                }
                
                if ( buffer[0] == KEY_LED )
                {
                    LED_Write(!LED_Read());
                }

            }
        }
    }
}

void uart_interface(void)
{
    uint number_of_packages;
    uint package_number;
    uint number_of_samples;
    uint channel_number;
    char * wave_segment_ptr;
    ready_to_start_sequence = TRUE; // used to avoid extern trigger from blocking the userinterface
    // Control interface via UART for Putty or Matlab/Octave
    
    if( UART_1_GetRxBufferSize() > 0) {
        puttyIn[bytenumber] = UART_1_GetChar();    
        bytenumber++;
    }
    

    if((bytenumber > 0 && puttyIn[0] != 'p') || (puttyIn[0] =='p' && bytenumber == size_of_header + size_of_segment)) {
    
    
        /* process firmware commands */
            // 1) select a channel
            if (    puttyIn[0] == KEY_SIG_1 ||\
                    puttyIn[0] == KEY_SIG_2 ||\
                    puttyIn[0] == KEY_SIG_3 ||\
                    puttyIn[0] == KEY_SIG_4 ||\
                    puttyIn[0] == KEY_SIG_5 ||\
                    puttyIn[0] == KEY_SIG_6 ||\
                    puttyIn[0] == KEY_SIG_MAIN	) {
                current_chan = puttyIn[0]-1-'0';
                ChannelSel_Select( current_chan );
                //show_channel_num();
            }
            // 2) run sequence
            if ( puttyIn[0] == KEY_RUN)
                run_sequence();
            // 3) reset firmware 
            if ( puttyIn[0] == KEY_RESET )
                CySoftwareReset(); // If Putty is used: this ends the session!
            // 4) set VDAC output range to 1V (default, voltage DACs only, comment code out when using current DACs)
            if ( puttyIn[0] == KEY_VDAC_1V )
                set_dac_range_1V();
            // 5) set VDAC output range to 4V (voltage DACs only, comment code out when using current DACs)
            if ( puttyIn[0] == KEY_VDAC_4V )
                set_dac_range_4V();    
            // 6) writes new sequence
            if ( puttyIn[0] == KEY_WRITE_SEQUENCE )
            {
                
                // get parameters:
                number_of_packages  = (256*puttyIn[4]+puttyIn[5]);
                package_number      = (256*puttyIn[2]+puttyIn[3]);
                number_of_samples   = number_of_packages*size_of_segment;
                channel_number      = puttyIn[1];
                
                // write wave into flash memory:
                wave_segment_ptr    = ((char *) signal_adc_1) + size_of_segment * package_number;
                memcpy(  wave_segment_ptr, (char *) &puttyIn[size_of_header], size_of_segment );
                if( package_number == (number_of_packages-1) )
                {
                    FLASH_Write( (uint8*)signal_adc_1, FLASH_STORAGE[channel_number], number_of_samples);
                }
                packages_received++;
                //for(int i=0; i< bytenumber; i++) UART_1_PutChar(puttyIn[i]);

            }     
            // 7) Use gpio P3[0] as trigger output
            if ( puttyIn[0] == KEY_TRIGGER_OUT )             
            { 
                trigger_out();
            }
            // 8) Use gpio P3[0] as trigger input
            if ( puttyIn[0] == KEY_TRIGGER_IN )             
            { 
                trigger_in();
            }     
            // 9) Firmware information
            if ( puttyIn[0] == KEY_VERSION )
            {
                UART_1_PutString(version);
            }
            // 10) Chip information
            if ( puttyIn[0] == KEY_SERIAL_NUMBER )
            {

                int strlength = 34;
                char pseudoid[strlength];
                    memset(pseudoid,0,strlength);
                    sprintf( pseudoid, "%3d %3d %3d %3d %3d %3d %3d",\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_Y_LOC,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_X_LOC,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_WAFER_NUM,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_LOT_LSB,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_LOT_MSB,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_WRK_WK,\
                        *(uint8 *)CYREG_FLSHID_CUST_TABLES_FAB_YR);

                UART_1_PutString(pseudoid);

            }
            
            // 11) Get the binary data from the two uint16 ADC buffers
            if ( puttyIn[0] == KEY_SEND_BYTE_DAT )
            {
                LED_Write( 1u ); 
                for(int j=0;j<NSAMPLES_ADC;j++)
                {
                    // turn uint16 arrays into uint8 streams:
                    UART_1_PutChar(signal_adc_1[j] >>8   );
                    UART_1_PutChar(signal_adc_1[j] &0xFF );
                    UART_1_PutChar(signal_adc_2[j] >>8   );
                    UART_1_PutChar(signal_adc_2[j] &0xFF );  
                    
                    CyDelayUs(350); // delay added for avoiding bluetooth buffer overrun
                }
                LED_Write( 0u );
            }// END send binary adc data

            // 12) (uart only) give ADC data stream in ASCII format (for display in e.g. Putty)
            if ( puttyIn[0] == KEY_SEND_ASCII_DAT )
            {
                display_results();
            }
            
            // 13) (uart only) run sequence and show results (2*NSAMPLES_ADC lines, ASCII formatted) via UART
            if ( puttyIn[0] == KEY_RUN_AND_SHOW )
            {
                run_sequence();
                display_results();
                show_channel_num();
            }    
            // 14) (uart only) runs sequence and switches to the next channel (just running, no output)
            if ( puttyIn[0] == KEY_RUN_AND_NEXT )
            {
                current_chan++;
                if(current_chan>4) current_chan=0;
                ChannelSel_Select(current_chan);
                run_sequence();
                show_channel_num();
            }
            
            // Alex Test-Commands (can be deleted later on)
            /*if ( puttyIn[0] == KEY_DCDC_OFF )
            {
                P_DCDC_CTRL_Write(!P_DCDC_CTRL_Read());
                UART_1_PutString("DCDC");
            }
                
            if ( puttyIn[0] == KEY_POWER_OFF )
            {
                P_PWR_EN_Write(1u);     // turn circuit off, to be turned on again via external switch.
                UART_1_PutString("OFF");    
            }
               
            if ( puttyIn[0] == KEY_LED )
            {
                LED_Write(!LED_Read());
            }*/
            
            
            puttyIn[0] = 0;
            bytenumber = 0;
            
    }
    
}

void trigger_out(void) {
    CompTrigger_Stop();
    enableTrigOut_Write( TRIGGER_OUT_TRUE );  
    //UART_1_PutStringConst("Trigger set to output");
    //UART_1_PutCRLF(1);
}

void trigger_in(void) {
    CompTrigger_Start();
    enableTrigOut_Write( TRIGGER_OUT_FALSE );    
    //UART_1_PutStringConst("Trigger set to input");
    //UART_1_PutCRLF(1);
}


void show_default_message(void)
{
    UART_1_PutCRLF(2);
    //sprintf(sms, "1) Press '%c' to run the sequence and show the results (ASCII table)", KEY_RUN_AND_SHOW);
    //    UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "1) Press '%c', '%c', '%c', '%c', '%c', '%c' or '%c' for changing the signal channel",\
        KEY_SIG_1, KEY_SIG_2, KEY_SIG_3, KEY_SIG_4, KEY_SIG_5, KEY_SIG_6, KEY_SIG_MAIN );
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "2) Press '%c' to run the sequence", KEY_RUN);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "3) Press '%c' to reset the firmware", KEY_RESET);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "4) Press '%c' to set the max amplitude of the DAC to 1V", KEY_VDAC_1V);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "5) Press '%c' to set the max amplitude of the DAC to 4V", KEY_VDAC_4V);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "6) write new sequence to the flash");
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "7) Press '%c' to use the Trigger as output (defualt)", KEY_TRIGGER_OUT);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "8) Press '%c' to use the Trigger as input", KEY_TRIGGER_IN);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
//    sprintf(sms, "9) Press '%c' for firmware information", version );
//        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "10) Press '%c' to get the Serial Number", KEY_SERIAL_NUMBER);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "11) Press '%c' to get the stored ADC values as bytestream", KEY_SEND_BYTE_DAT);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "12) (only UART) Press '%c' to get the stored ADC values as ASCII stream", KEY_SEND_ASCII_DAT);
        UART_1_PutString(sms); UART_1_PutCRLF(1);
    sprintf(sms, "13) (only UART) Press '%c' to run the sequence and get the stored ADC values as ASCII stream", KEY_RUN_AND_SHOW);
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
        
        CyDelay(3); // delay for avoiding bluetooth buffer overrun
        
        // stop printing ascii table with any key:
        if( UART_1_GetRxBufferSize() != 0 )
        {
           UART_1_ClearRxBuffer();
           UART_1_PutCRLF(2);
           UART_1_PutStringConst("Data listing aborted...\n\n");
           UART_1_PutCRLF(1);
           CyDelay(10);
           break;
        }
    }
    
    show_channel_num();
    
    show_default_message();  
}

void run_sequence(void)
{
    //UART_1_PutStringConst("Running sequence");
    internTrigger_Write(STOP_CLOCK);
    
    // Reset trigger adjustments
    ClockShift_1_Stop(); ClockShift_1_WriteRegValue(CLOCK_SHIFT_CH1); ClockShift_1_Start();
    ClockShift_2_Stop(); ClockShift_2_WriteRegValue(CLOCK_SHIFT_CH2); ClockShift_2_Start();
    ClockShift_3_Stop(); ClockShift_3_WriteRegValue(CLOCK_SHIFT_CH3); ClockShift_3_Start();
    ClockShift_4_Stop(); ClockShift_4_WriteRegValue(CLOCK_SHIFT_CH4); ClockShift_4_Start();  
    //channelsplitting (CH1 -> CH1a / CH1b)
    ClockShift_1ab_Stop(); ClockShift_1ab_WriteRegValue(CLOCK_SHIFT_CH1ab); ClockShift_1ab_Start();
    //channelsplitting (CH2 -> CH2a / CH2b)
    ClockShift_2ab_Stop(); ClockShift_2ab_WriteRegValue(CLOCK_SHIFT_CH2ab); ClockShift_2ab_Start();
    

    dma_dac_1_init();
    dma_dac_2_init();
    

    dma_dac_3_init();
    dma_dac_4_init();

    dma_adc_1_init();
    dma_adc_2_init();
        
    internTrigger_Write(START_CLOCK);
    
    count_of_runs++;
}//END run_sequence()

void set_dac_range_1V(void) {
    // Comment/uncomment for switching between 
    // current DACs and voltage DACs
    
    IDAC8_1_SetRange( IDAC8_1_RANGE_255uA );
    //IDAC8_1_SetRange( IDAC8_1_RANGE_1V );
    
    IDAC8_2_SetRange( IDAC8_2_RANGE_255uA );
    //IDAC8_2_SetRange( IDAC8_1_RANGE_1V );
    
    IDAC8_3_SetRange( IDAC8_3_RANGE_255uA );
    //IDAC8_3_SetRange( IDAC8_1_RANGE_1V );
    
    IDAC8_4_SetRange( IDAC8_4_RANGE_255uA );
    //IDAC8_4_SetRange( IDAC8_1_RANGE_1V );  
}//END set_dac_range_1V()


void set_dac_range_4V(void){
    // Comment/uncomment for switching between 
    // current DACs and voltage DACs
        
    IDAC8_2_SetRange( IDAC8_2_RANGE_2mA );
    //IDAC8_1_SetRange( IDAC8_1_RANGE_4V );
    
    IDAC8_2_SetRange( IDAC8_2_RANGE_2mA );
    //IDAC8_2_SetRange( IDAC8_2_RANGE_4V );
    
    IDAC8_3_SetRange( IDAC8_3_RANGE_2mA );
    //IDAC8_3_SetRange( IDAC8_3_RANGE_4V );
    
    IDAC8_4_SetRange( IDAC8_4_RANGE_2mA );
    //IDAC8_4_SetRange( IDAC8_1_RANGE_4V ); 
}//END set_dac_range_4V()

    
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
    CyDmaTdSetAddress(DMA_DAC_1_TD[0], LO16((uint32)(FLASH_CH1+flash_offset_ch1)), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_1_TD[1], LO16((uint32)(FLASH_CH1+flash_offset_ch1) + NSAMPLES_DAC), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_1_TD[2], LO16((uint32)(FLASH_CH1+flash_offset_ch1) + 2*NSAMPLES_DAC), LO16((uint32)IDAC8_1_Data_PTR));
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
    CyDmaTdSetAddress(DMA_DAC_2_TD[0], LO16((uint32)(FLASH_CH2+flash_offset_ch2)), LO16((uint32)IDAC8_2_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_2_TD[1], LO16((uint32)(FLASH_CH2+flash_offset_ch2) + NSAMPLES_DAC), LO16((uint32)IDAC8_2_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_2_TD[2], LO16((uint32)(FLASH_CH2+flash_offset_ch2) + 2*NSAMPLES_DAC), LO16((uint32)IDAC8_2_Data_PTR));
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
    CyDmaTdSetAddress(DMA_DAC_3_TD[0], LO16((uint32)(FLASH_CH3+flash_offset_ch3)), LO16((uint32)IDAC8_3_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_3_TD[1], LO16((uint32)(FLASH_CH3+flash_offset_ch3) + NSAMPLES_DAC), LO16((uint32)IDAC8_3_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_3_TD[2], LO16((uint32)(FLASH_CH3+flash_offset_ch3) + 2*NSAMPLES_DAC), LO16((uint32)IDAC8_3_Data_PTR));
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
    CyDmaTdSetAddress(DMA_DAC_4_TD[0], LO16((uint32)(FLASH_CH4+flash_offset_ch4)), LO16((uint32)IDAC8_4_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_4_TD[1], LO16((uint32)(FLASH_CH4+flash_offset_ch4) + NSAMPLES_DAC), LO16((uint32)IDAC8_4_Data_PTR));
    CyDmaTdSetAddress(DMA_DAC_4_TD[2], LO16((uint32)(FLASH_CH4+flash_offset_ch4) + 2*NSAMPLES_DAC), LO16((uint32)IDAC8_4_Data_PTR));
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

CY_ISR( isr_triggerIn ){
    if( ready_to_start_sequence == TRUE ){
        ready_to_start_sequence = FALSE; // avoid blocking Putty user interface
        LED_Write( ~LED_Read()); // indicate trigger
        run_sequence(); 
        CyDelayUs(SEQU_DURATION_US); // Block CPU while sequence is running to avoid interference
    }
}

CY_ISR( isr_DAC_1_done ){

}//endCY_ISR DAC_1

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
