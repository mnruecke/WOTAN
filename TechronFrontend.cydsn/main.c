/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <stdio.h>
#include <math.h>
#include "project.h"


#define FALSE 0
#define TRUE  1
#define ON    1
#define OFF   0
#define DISPLAY_TIME_OUT_US 1000000


/* Defines for DMA_1 */
#define DMA_1_BYTES_PER_BURST 1
#define DMA_1_REQUEST_PER_BURST 1
#define DMA_1_SRC_BASE (CYDEV_SRAM_BASE)
#define DMA_1_DST_BASE (CYDEV_PERIPH_BASE)

/* Variable declarations for DMA_1 */
/* Move these variable declarations to the top of the function */
uint8 DMA_1_Chan;
uint8 DMA_1_TD[10];

#define SAMPLING_RATE_WAVE1         4000000
#define NSAMPLES_WAVE1              40000 // 4 MS/s; 10 ms 
uint8   wave1[ NSAMPLES_WAVE1 ];


char sms[80];

uint8 display_on=0;
int   display_timer=0;
uint8 poti_0;
uint8 poti_1;


void init_psoc(void);
void display_auto_power_off(void);
void uart_interface(void);
void set_dual_poti(uint8 w0, uint8 w1);
void config_dma1(void);
void generate_wave1(void);

CY_ISR_PROTO( poti_up );
CY_ISR_PROTO( poti_down );
CY_ISR_PROTO( wave1_done );


int main(void)
{
    init_psoc();

    for(;;)
    {
        display_auto_power_off();
        uart_interface();
    }//END for(;;)
}//END main()


void init_psoc(void){
    CyGlobalIntEnable; /* Enable global interrupts. */
   
    isr_up_StartEx( poti_up );
    isr_down_StartEx( poti_down );
    isr_dac1_done_StartEx( wave1_done );
    
    // Get and set last poti value:
    EEPROM_Start();
    poti_1 = EEPROM_ReadByte(2047);
    set_dual_poti( poti_0, poti_1 );
    
    en_Seg7_Write( ON );
    Seg7_Start();
    display_on = TRUE;
    display_timer=0;
    sprintf(sms,"G%3d",poti_1);
    Seg7_WriteString7Seg(sms,0);     
    
    UART_Start(); //Thinkpad: COM18
    UART_PutString("PSoC running...\r\n");
    
    IDAC8_1_Start();
    /* DMA Configuration for DMA_1 */
    DMA_1_Chan = DMA_1_DmaInitialize(DMA_1_BYTES_PER_BURST, DMA_1_REQUEST_PER_BURST, 
        HI16(DMA_1_SRC_BASE), HI16(DMA_1_DST_BASE));
    DMA_1_TD[0] = CyDmaTdAllocate();
    DMA_1_TD[1] = CyDmaTdAllocate();
    DMA_1_TD[2] = CyDmaTdAllocate();
    DMA_1_TD[3] = CyDmaTdAllocate();
    DMA_1_TD[4] = CyDmaTdAllocate();
    DMA_1_TD[5] = CyDmaTdAllocate();
    DMA_1_TD[6] = CyDmaTdAllocate();
    DMA_1_TD[7] = CyDmaTdAllocate();
    DMA_1_TD[8] = CyDmaTdAllocate();
    DMA_1_TD[9] = CyDmaTdAllocate();
    
    generate_wave1();
}//END init_psoc()


void display_auto_power_off(void){
    //Switch display of after ~1 s 
    if( display_on == TRUE ){     
        CyDelayUs(1);
        display_timer++;
        if( display_timer > DISPLAY_TIME_OUT_US ){
            en_Seg7_Write( OFF );
            Seg7_Stop();
            display_on = FALSE;
        }//END if( display_timer > 1000000 )
    }//END if( display_on == 1 
}//END display_time_out()


void set_dual_poti(uint8 w0, uint8 w1){
    uint32 data;
    
    // b0:       stack select bit               [16]
    // b1..b8:   potentiometer-1 wiper value    [15:8]
    // b9..b16:  potentiometer-0 wiper value    [7:0]
    data = (1<<17) + (w1<<8) + (w0);
    
    // Enable digipot
    SPI_CE_Write( 1u );
    
    // Send 17 bits; b0: first bit; b16: last bit
    for(int bit_i=16; bit_i>=0; bit_i--){
 
        // a) Next bit
        SPI_data_Write( (data >> bit_i) & 0x01 );
        
        // b) Generate a clock signal
        SPI_clock_Write( 1u );
        SPI_clock_Write( 0u );
    }//END for(int bit_i=16; bit_i>=0; bit_i--)
    
    // Disable digipot
    SPI_data_Write( 0u );
    SPI_CE_Write( 0u );
}//END set_dual_poti()


void uart_interface(void){
    char ble_in;
    while( UART_GetRxBufferSize() > 0 ){
        ble_in = UART_GetChar();
        if( ble_in == 'u' ){// poti++
            isr_up_SetPending();
        }else if( ble_in == 'd' ){// poti--
            isr_down_SetPending();
        }//END if( ble_in == 'u' )
    }//END while( UART_GetRxBufferSize() > 0 )
}//END uart_interface()


void config_dma1(void){
    CyDmaTdSetConfiguration(DMA_1_TD[0], NSAMPLES_WAVE1/10, DMA_1_TD[1], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[1], NSAMPLES_WAVE1/10, DMA_1_TD[2], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[2], NSAMPLES_WAVE1/10, DMA_1_TD[3], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[3], NSAMPLES_WAVE1/10, DMA_1_TD[4], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[4], NSAMPLES_WAVE1/10, DMA_1_TD[5], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[5], NSAMPLES_WAVE1/10, DMA_1_TD[6], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[6], NSAMPLES_WAVE1/10, DMA_1_TD[7], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[7], NSAMPLES_WAVE1/10, DMA_1_TD[8], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[8], NSAMPLES_WAVE1/10, DMA_1_TD[9], CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[9], NSAMPLES_WAVE1/10, CY_DMA_DISABLE_TD, DMA_1__TD_TERMOUT_EN | CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_1_TD[0], LO16((uint32)&wave1[ 0 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[1], LO16((uint32)&wave1[ 1 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[2], LO16((uint32)&wave1[ 2 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[3], LO16((uint32)&wave1[ 3 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[4], LO16((uint32)&wave1[ 4 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[5], LO16((uint32)&wave1[ 5 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[6], LO16((uint32)&wave1[ 6 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[7], LO16((uint32)&wave1[ 7 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[8], LO16((uint32)&wave1[ 8 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaTdSetAddress(DMA_1_TD[9], LO16((uint32)&wave1[ 9 * NSAMPLES_WAVE1/10 ]), LO16((uint32)IDAC8_1_Data_PTR));
    CyDmaChSetInitialTd(DMA_1_Chan, DMA_1_TD[0]);
    CyDmaChEnable(DMA_1_Chan, 1); 
}//END config_dma1()


void generate_wave1(void){
    for(int time=0; time<NSAMPLES_WAVE1; time++){
        wave1[ time ] = (uint8) ( -127.0*cos(\
            2*M_PI *(float)time *(float)time * 200 \
            / (float)NSAMPLES_WAVE1\
            / (float)NSAMPLES_WAVE1) + 127.0 );
    }//END for(int i=0; i<NSAMPLES_WAVE1; i++)
}//END generate_wave1()


CY_ISR( poti_up ){
    // Switch on display for 1 s
    en_Seg7_Write(1);
    Seg7_Start();
    display_timer=0;
    CyDelay(50);
    
    //Single step:
    if( display_on == FALSE ){
        // Poti value remains unchanged if display was switched off
        display_on = TRUE;
    }else{
        poti_1++;
    }//END if( display_on == 0 )
    sprintf(sms,"G%3d",poti_1);
    Seg7_WriteString7Seg(sms,0); 
    
    //Multiple steps after pressing key >200 ms:
    int timer=0;
    while( Poti_up_Read() == 0 ){
        CyDelay(1);
        timer++;
        if( timer > 200 ){
            CyDelay(5);
            poti_1++;
            sprintf(sms,"G%3d",poti_1);
            Seg7_WriteString7Seg(sms,0);            
        }//END if( timer > 200 )
    }//END while( Poti_down_Read() == 0 )
    
    //Set and save new value:
    set_dual_poti( poti_1, poti_1 );
    EEPROM_WriteByte( poti_1, 2047);
    
    //Run test sequence (e.g., for calibration purposes)
    config_dma1();
}//END CY_ISR( poti_up )


CY_ISR( poti_down ){
    // Switch on display for 1 s
    en_Seg7_Write( ON );
    Seg7_Start();
    display_timer=0;
    CyDelay(50);


    // Single step:
    if( display_on == FALSE ){
        //Poti value remains unchanged if display was switched off
        display_on = TRUE;
    }else{
        poti_1--;
    }//END if( display_on == 0 )
    sprintf(sms,"G%3d",poti_1);
    Seg7_WriteString7Seg(sms,0); 
    
    // Multiple steps after pressing key >200 ms):
    int timer=0;
    while( Poti_down_Read() == 0 ){
        CyDelay(1);
        timer++;
        if( timer > 200 ){
            CyDelay(5);
            poti_1--;
            sprintf(sms,"G%3d",poti_1);
            Seg7_WriteString7Seg(sms,0);            
        }//END if( timer > 200 )
    }//END while( Poti_down_Read() == 0 )
    
    //Set and save new value:
    set_dual_poti( poti_1, poti_1 );
    EEPROM_WriteByte( poti_1, 2047);
    
    //Run test sequence (e.g., for calibration purposes)
    config_dma1();
}//END CY_ISR( poti_down )


CY_ISR( wave1_done ){
//    LED_Write(1);
//    CyDelay(100);
//    LED_Write(0);
}//END CY_ISR( wave1_done )



/* [] END OF FILE */
