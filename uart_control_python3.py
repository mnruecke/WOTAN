# -*- coding: utf-8 -*-
"""
%  ========================================
 % 
 % This code is supplementary material for the IWMPI 2018, Hamburg, Germany 
 %
 % Martin.Rueckert@physik.uni-wuerzburg.de
 %
 % Copyright (C) 2017 University of Wuerzburg, Experimental Physics 5, Biophysics
 % https://www.physik.uni-wuerzburg.de/ep5/magnetic-particle-imaging/
 %
 % WOTAN is free software: you can redistribute it and/or modify
 % it under the terms of the GNU General Public License version 3 as
 % published by the Free Software Foundation.
 %
 % WOTAN is distributed in the hope that it will be useful,
 % but WITHOUT ANY WARRANTY; without even the implied warranty of
 % MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 % GNU General Public License for more details.
 %
 % You should have received a copy of the GNU General Public License
 % along with WOTAN.  If not, see <http://www.gnu.org/licenses/>.
 % ========================================
% Description: 
%
% control script for interacting with the WOTAN-PSoC firmware.
%
% Connect with CY8CKIT-059-kit via USB-to-UART in the WOTAN-Firmware.
% Running 'uart_control_python.py' starts the measurement, reads the
% resulting data stream and saves data as ascii-table. 
%
% Default measurement with WOTAN: 
%   - duration: 15 ms with 2 MS/s at 12 bit resolution -> 60 KB uint16 data
%     (limited because of 64 KB SRAM on PSoC 5LP)
%   - transfer to PC takes approx. 5 sec at baudrate = 115200
%   - the USBFS-component on PSoC 5LP allows 12.5 Mbps (i.e. 100x faster)
%
% ========================================
"""

import serial
import time
import struct
import os
import os.path

import numpy as np
import matplotlib.pyplot as plt

""" 0) main settings """
# 0.1) serial port
com_port       = '\\\\.\\COM21'
baudrate       = 115200
time_out       = 10

# 0.2 sequence details
bytesPerSample = 2
timePerSample  = 0.0005 # sample time in ms
sequDuration   = 15 # sequence duration in ms
numOfSamples   = int(sequDuration/timePerSample)
bufInputSize   = numOfSamples * bytesPerSample

nameDataFiles  = '3D_mpi_data' 

""" END - main settings """

# 0.4) list of commands defined in WOTAN
p_sel_chan  = b'5' # default: '5' (measure signal between GPIO 0.6 and 0.7), '1'..'4': get DAC output 1..4 
p_run_sequ  = b'r'
p_get_data  = b'o'
p_reset     = b'e'

# 1) start measurement on PSoC and get data
try: # open and interact with serial port 
    ser = serial.Serial( com_port, baudrate, timeout=time_out)
    
    # run MPI sequence on psoc
    ser.write( p_sel_chan )
    time.sleep(0.3)
    ser.write( p_run_sequ )
    time.sleep(0.3)
    ser.flushInput()
    time.sleep(0.3)
    ser.write( p_get_data )
    time.sleep(0.3)
    
    # get data as byte stream 
    adc_data_bin = ser.read(bufInputSize)
    # transform byte stream into int16 array
    adc_data_int16 = struct.unpack('>'+'H'*int(len(adc_data_bin)/bytesPerSample),adc_data_bin)
    
finally: # close serial port
    ser.close()


if len(adc_data_bin) == numOfSamples*bytesPerSample: # check if run was successful
    # 3) data correction routines:
    # find and correct scaling difference between ADC 1 (even samples)
    # and ADC 2 (odd samples)
    # (this method fails if signal has steps or goes into saturation!)
    adc1 = adc_data_int16[0::2]
    adc2 = adc_data_int16[1::2]
    
    adc1DIVadc2 = 0;
    for sp in range(len(adc1)):
        adc1DIVadc2 += (adc1[sp]-adc2[sp])/(adc1[sp]+adc2[sp])*2/len(adc1)
    
    adc_data_corr = np.zeros(len(adc_data_int16))
    adc_data_corr[0::2] = adc1
    adc_data_corr[0::2] = adc_data_corr[0::2]# *(1-adc1DIVadc2)
    adc_data_corr[1::2] = adc2
        
    # 4) visualize data
    dat_time = np.arange(0,sequDuration,timePerSample)
    dat_sig  = adc_data_corr
    plt.plot( dat_time, dat_sig, dat_time, dat_sig,'+')
    plt.xlabel('time [ms]')
    plt.ylabel('signal [a.u.]')
    plt.show()  
    
    
    # 5) save data as ascii table
    # write data in file with continuous numbering
    cnt = 0
    data_file_name = nameDataFiles + '_' + str(cnt) + '.txt'
    while os.path.isfile( data_file_name ): #prevents overriding files
        cnt += 1
        data_file_name = nameDataFiles + '_' + str(cnt) + '.txt'
    with open( data_file_name , 'w') as f:
        for dat in adc_data_corr:
            f.write("%s\n" % int(dat))
        print( 'Data written to: ' + data_file_name +
              '  (' + str(len(adc_data_int16)) + ' samples)')
else:
    print("\n\n\nPSoC doesn't seem ready. Please try again. " +\
          "(WOTAN firmware requires approx. 5 sec. after " +\
          "programming for sequence calculation.)\n\n\n")
        
            