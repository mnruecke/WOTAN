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
% Description: control script for usbfs data transfer
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
#      this script targets the faster USBUART routed to the Micro-USB-B socket
com_port         = '\\\\.\\COM6'
baudrate         = 1 # USBFS component ignores this parameter
time_out         = 1 # [s]; this script should retrieve the 60 kB data in << 1s

# 0.2) channel settings
num_channels = 4            # number of channels
write_channels = [0,1,2,3]
nsamples_ramp_up = 3750     # number of steps for the ramp up sequence
nsamples_sequence = 3750    # number of steps for the actual sequence
nsamples_ramp_down = 3750   # number of steps for the ramp up sequence
nsamples_total = nsamples_ramp_up + nsamples_sequence + nsamples_ramp_down  # total number of steps

max_value = 180     
values = np.zeros((num_channels, nsamples_total),  dtype=np.uint8) # 2D array storing the sequence

dac_sampling_rate = 250e3
frequency_scale = 1/dac_sampling_rate

# 0.3) package settings
len_header = 8
len_data = 32
""" END - main settings """



""" generates sequence """
f = frequency_scale*np.asarray([1000,5000,10000,20000])         # frequency in Hz
f_mod = frequency_scale*np.asarray([0.0,0,0,0])      # in Hz
phi = np.asarray([0,90,0,0])         # in degree
phi_mod = np.asarray([90,90,90,90])     # in degree
amp = np.asarray([0.47 * max_value, 0.47 * max_value, 0.47 * max_value, 0.47 * max_value])        
off = np.asarray([0.5 * max_value, 0.5 * max_value, 0.5 * max_value, 0.5 * max_value])


for channel in range(num_channels):
    # ramp up
    for t in np.arange(nsamples_ramp_up):
        values[channel][t] = 1.0 / nsamples_ramp_up * t \
        * amp[channel] * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]
    # actual sequence
    for t in (np.arange(nsamples_sequence)+nsamples_ramp_up):
        values[channel][t] = \
        amp[channel] * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]
    # ramp down
    for t in (np.arange(nsamples_ramp_down)+nsamples_ramp_up+nsamples_sequence):
        values[channel][t] = (1.0 - (t-nsamples_ramp_up-nsamples_sequence) / nsamples_ramp_down) \
        * amp[channel] * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]



# Plots the generated sequence
for channel in range(num_channels):
    plt.plot(np.arange(nsamples_total)/dac_sampling_rate, values[channel])
    plt.xlabel('time [s]')
    plt.ylabel('dac value (uint8)')
plt.show()



""" write sequence """

# opening serial connection
try:
    ser = serial.Serial( com_port, baudrate, timeout=time_out)
    start = timeit.default_timer()
    
    
    # calculating the numbe of packages
    num_packages = int (np.ceil(nsamples_total / len_data)) # number of full packages    
        
    for channel in write_channels:
        for package in range(num_packages):
            
            #header
            header = np.zeros(len_header,  dtype=np.uint8)
            header[0] = ord('p')                    # 'p' for programming mode
            header[1] = channel                     # channel number
            header[2] = package >> 8                # package number MSB
            header[3] = package & 0xFF               # package number LSB
            header[4] = num_packages >> 8
            header[5] = num_packages & 0xFF
            
            header_bytes = bytes(header)    
            
            #data
            data = values[channel][len_data*package:len_data*(package+1)]
            #last package
            if data.size != 32:
                data = np.append(data, np.zeros(32-data.size,dtype=np.uint8))
                
            data_bytes = bytes(data)
    
            ser.write(header_bytes + data_bytes)
            
            """ Debugging code """
#            print("Channel:  " + str(channel) + "   Package:  " + str(package))            
#            print(data_bytes)
#            print(ser.read(32))

finally:
    ser.close()


