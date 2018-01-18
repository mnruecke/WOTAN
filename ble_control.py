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
% control script for interacting with the WOTAN-PSoC firmware via 
% bluetooth.
%
% Connect with CY8CKIT-059-kit with HC-05 bluetooth module. 
% Running 'ble_control.py' starts the measurement, reads the
% resulting data stream and saves data as ascii-table. 
%
% Default measurement with WOTAN: 
%   - duration: 15 ms with 2 MS/s at 12 bit resolution -> 60 KB uint16 data
%     (limited because of 64 KB SRAM on PSoC 5LP)
%   - transfer to PC takes approx. 5 sec at baudrate = 115200
%   - the USBFS-component on PSoC 5LP allows 12.5 Mbps (i.e. 100x faster)
%   - the HC-05 bluetooth module allows 1.3 Mbps
%
% ========================================
"""

import serial
import time
import struct
import os

import numpy as np
import matplotlib.pyplot as plt

""" 0) main settings """
# 0.1) serial port
com_port       = '\\\\.\\COM10'# bluetooth modul enumerate as com port
baudrate       = 115200 # the value is ignored for virtual com ports
time_out       = 1

# 0.2 sequence details
bytesPerSample = 2
timePerSample  = 0.0005 # sample time in ms
sequDuration   = 15 # sequence duration in ms
numOfSamples   = int(sequDuration/timePerSample)
bufInputSize   = numOfSamples * bytesPerSample

packetSize     = 4000

nameDataFiles  = '3D_mpi_data' 

""" END - main settings """

# 0.4) Bluetoot control: Command structure: 'BinaryABC'
#       'Binary':   Command prefix
#       'A':        Sequence number (Default: '1')
#       'B':        Channel number (default: 5 - signal between GPIO 0.6 and 0.7)  
#       'C':       Packet number: full sequence goes from 0 to 29 (60000 kB divided in 2000)
#               'C' is in ascii code starting from 'A': 0..29-> A...^ (german keyboard mapping)
ble_command = bytearray()
ble_command += b'Binary140'

ble_command[-1]=65 #=ord('A')

adc_data_all = bytearray()

# 1) start measurement on PSoC and get data
start = time.time()
try: # open and interact with serial port 
    ser = serial.Serial( com_port, baudrate, timeout=time_out)
    
    for i in range(15):
        #start = time.time()
        # Generate command
        if i==0:
            ble_command[-1]=ord('A') + i #packet numbering shifted by ascii value of 'A'
        else:
            ble_command[-1]=ord('A') + i
            ble_command[-2]=ord('0')
            ble_command[-3]=ord('0')
        
        # run MPI sequence on psoc
        ser.write( ble_command )
        
        # get data as byte stream 
        adc_data_bin = ser.read(packetSize)
        adc_data_all += adc_data_bin
    
    # transform byte stream into int16 array
    adc_data_int16 = struct.unpack('>'+'H'*int(len(adc_data_all)/bytesPerSample),adc_data_all)
    
finally: # close serial port
    ser.close()

stop = time.time()
print("%1.3f"% (stop-start) )

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
adc_data_corr[0::2] = adc_data_corr[0::2] *(1-adc1DIVadc2)
adc_data_corr[1::2] = adc2
    
# 4) visualize data
dat_time = np.arange(0,sequDuration,timePerSample)
dat_sig  = adc_data_corr
plt.plot( dat_time, dat_sig, dat_time, dat_sig,'+')
plt.xlabel('time [ms]')
plt.ylabel('signal [a.u.]')
plt.show()  
    
    

        
            