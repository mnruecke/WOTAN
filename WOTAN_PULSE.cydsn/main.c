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

#define     ON           0u
   
#define     SEQUENCE_RUN    0u
#define     SEQUENCE_HALT   1u

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    PWM_1_Start();
    UART_BLE_Start();

    for(;;)
    {
        /* Pulse sequence control
           BLE trigger overwrites Manual trigger and vice versa */
        CyDelay(100);
        UART_BLE_PutChar( UART_BLE_GetChar());

    }
}

/* [] END OF FILE */
