# -*- coding: utf-8 -*-
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
% Description: code stub for usbfs data transfer - I
%
% control script for interacting with the WOTAN-PSoC firmware.
%
%               - incomplete - 
%
% Connect with CY8CKIT-059-kit via USBFS (fullspeed) in the WOTAN-Firmware.
% Running 'usbfs_control.py' starts the measurement, reads the
% resulting data stream and saves data as ascii-table. 
%
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

import timeit

""" 0) main settings """
# 0.1) serial port
com_port         = '\\\\.\\COM24'
baudrate         = 115200
time_out         = 10

# 0.2 sequence details
bytesPerSample   = 2
timePerSample    = 0.0005 # sample time in ms
sequDuration     = 15 # sequence duration in ms
numOfSamples     = int(sequDuration/timePerSample)
bufInputSize     = numOfSamples * bytesPerSample

adcVoltPerBit    = 0.0005 # scaling factor for ADC data

nameDataFiles    = '3D_mpi_data'  # save data to files with continuous numbering (ascii table)

""" END - main settings """

# 0.4) list of commands defined in WOTAN
p_sel_chan  = b'5' # default: '5' (measure signal between GPIO 0.6 and 0.7), '1'..'4': get DAC output 1..4 
p_run_sequ  = b'r'
p_get_data  = b'o'
p_reset     = b'e'

# 1) start measurement on PSoC and get data
try: # open and interact with serial port 
    ser = serial.Serial( com_port, baudrate, timeout=time_out)
    
    start = timeit.default_timer()
    # run MPI sequence on psoc
#    ser.write( p_sel_chan )
#    time.sleep(0.3)
#    ser.write( p_run_sequ )
#    time.sleep(0.3)
    ser.flushInput()
    time.sleep(0.001)
    ser.write( p_get_data )
    time.sleep(0.001)
    
    # get data as byte stream 
    adc_data_bin = ser.read(60*1000) #ser.read(bufInputSize)
    stop = timeit.default_timer()
    # transform byte stream into int16 array
    adc_data_int16 = struct.unpack('<'+'H'*int(len(adc_data_bin)/bytesPerSample),adc_data_bin)
    
finally: # close serial port
    ser.close()
    
adc1UNIONadc2 = np.arange(len(adc_data_int16))    
adc1UNIONadc2[0::2] = adc_data_int16[0:int(len(adc_data_int16)/2)]
adc1UNIONadc2[1::2] = adc_data_int16[int(len(adc_data_int16)/2):]
    
print(len(adc_data_bin))    
print(stop-start)

plt.plot(adc1UNIONadc2)