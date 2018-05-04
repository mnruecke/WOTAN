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
com_port         = '\\\\.\\COM8'
baudrate         = 9600 # USBFS component ignores this parameter
time_out         = 1 # [s]; this script should retrieve the 60 kB data in << 1s

# 0.2) channel settings
num_channels = 1            # number of channels
nsamples_ramp_up = 4     # number of steps for the ramp up sequence
nsamples_sequence = 24    # number of steps for the actual sequence
nsamples_ramp_down = 4   # number of steps for the ramp up sequence
nsamples_total = nsamples_ramp_up + nsamples_sequence + nsamples_ramp_down  # total number of steps

max_value = 180     
values = np.zeros((num_channels, nsamples_total),  dtype=np.uint8) # 2D array storing the sequence


# 0.3) package settings
len_header = 8
len_data = 32
""" END - main settings """



""" generates sequence """
f = [0.01,0.01,700,1600,1600]         # frequency in Hz
f_mod = [0.0,0,0,0]      # in Hz
phi = [40,90,0,0]         # in degree
phi_mod = [90,90,0,0]     # in degree
amp = [0.5 * max_value, 0.5 * max_value, 0.5 * max_value, 0.5 * max_value]        
off = [0.5 * max_value, 0.5 * max_value, 0.5 * max_value, 0.5 * max_value]


for channel in range(num_channels):
    # ramp up
    for t in range(nsamples_ramp_up):
        values[channel][t] = 1.0 / nsamples_ramp_up * t \
        * amp[channel] * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]
    # actual sequence
    for t in range(nsamples_sequence):
        values[channel][t+nsamples_ramp_up] = \
        amp[channel] * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]
    # ramp down
    for t in range(nsamples_ramp_down):
        values[channel][t+nsamples_ramp_up + nsamples_sequence] = (1.0 - t / nsamples_ramp_down) \
        * amp[channel] * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]


# Plots the generated sequence

for channel in range(num_channels):
    plt.plot(range(nsamples_total), values[channel])
plt.show()



""" write sequence """

# opening serial connection
ser = serial.Serial( com_port, baudrate, timeout=time_out)
start = timeit.default_timer()


# calculating the numbe of packages
num_packages = int (nsamples_total / len_data) # number of full packages
if nsamples_total%len_data != 0:               # add a partially filled package
    num_packages += 1
    
    
for channel in range(num_channels):
    for package in range(num_packages):
        
        #header
        header = np.zeros(len_header,  dtype=np.uint8)
        header[0] = ord('p')                    # 'p' for programming mode
        header[1] = channel                     # channel number
        header[2] = package >> 8                # package number HSB
        header[3] = package & 0xF               # package number LSB
        
        header_bytes = bytes(header)    
        
        #data
        data = values[channel][len_data*package:len_data*(package+1)]
        #last package
        if data.size != 32:
            data = np.append(data, np.zeros(32-data.size,dtype=np.uint8))
            
        data_bytes = bytes(data)

        ser.write(header_bytes + data_bytes)

ser.close()




