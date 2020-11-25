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
% Script for uploading a sequence to the WOTAN board via the fast USBFS port.
% The COM number of the USBFS port must be changed under com_port
%
%
% ========================================
"""

""" --- Required parameters ---- """ 
# A) check device manager to see at which port number the board enumerates
#serialPort = '\\\\.\\COM8' 
# usbfs - lenovo thinkpad, Module M:
#serialPort = '\\\\.\\COM17' 
#serialPort = '\\\\.\\COM15'
# usbfs - lenovo thinkpad, Module J:
#serialPort = '\\\\.\\COM19' 
# Port for BLE HC-05 ("next") - lenovo thinkpad: 
#serialPort = '\\\\.\\COM10' 
# usbfs - lifebook, Module J:
#serialPort = '\\\\.\\COM11' 
serialPort = '\\\\.\\COM12' 

# B) baudrate
baudrate = 1382400
# C) uncomment line with the channel that is to be observed:
#channel = b'1'  # show output of DAC 1
#channel = b'2'  # show output of DAC 2
#channel = b'3'  # show output of DAC 3
#channel = b'4'  # show output of DAC 4
channel = b'2'  # show signal voltage between GPIO P0.6 (-) and GPIO P0.7 (+)

# D) uncomment the used interface
#interface = "UART"
interface = "USBFS"
""" ----------------------------- """

import serial
import numpy as np
import matplotlib.pyplot as plt
import timeit
import time

""" 0) main settings """
#  serial port
time_out = 10                # [s]; this script should retrieve the 60 kB data in << 1s

#  channel settings
num_channels = 4            # number of channels
nsamples_ramp_up = 3750     # number of steps for the ramp up sequence
nsamples_sequence = 3750    # number of steps for the actual sequence
nsamples_ramp_down = 3750   # number of steps for the ramp up sequence
nsamples_total = nsamples_ramp_up + nsamples_sequence + nsamples_ramp_down  # total number of steps

max_value = 255     
values = np.zeros((num_channels, nsamples_total),  dtype=np.uint8) # 2D array storing the sequence

dac_sampling_rate = 250e3
frequency_scale = 1/dac_sampling_rate

# package settings
len_header = 8
len_data = 32
""" END - main settings """



""" 1) generate sequence """

""" Parameters for each channel  
    format:
    <param_list> = [<channel 1a>, <channel 1b>, <channel 2>, <channel 3>, <channel 4>]
"""
# Main frequencies (Hz)
f = frequency_scale*np.asarray([1000,1000,1000,1000,15000])
# Modulation frequencies (Hz)                    
f_mod = frequency_scale*np.asarray([0,0,0,0,0])                      
# Start phase for main frequencies (degree)
phi = np.asarray([0+180, 0+180, 0, 0,0]) 
# Start phase for modulation frequencies (degree)
phi_mod = np.asarray([90,90,90,90,90])   
# Amplitudes at t=15 ms (start ramp goes from t=0...15 ms:
amp_start = max_value * np.asarray([0.45, 0.45, 0.47, 0.47, 0.47])   
# Amplitudes at t=30 ms 
amp_end = max_value * np.asarray([0.45, 0.45, 0.47, 0.47, 0.47]) 
# Offset voltage for each channel (necessary for avoiding clipping)     
off = max_value * np.asarray([0.55, 0.55, 0.5, 0.5, 0.5])




""" 1A) Generate sequence for split channel CH1a/CH1b
    odd  samples: CH1a
    even samples: CH1b
    """
samplingpattern_a_ramp_up = np.arange(0,nsamples_ramp_up,2)    
samplingpattern_a_main_part = np.arange(0+nsamples_ramp_up,\
                               nsamples_ramp_up+nsamples_sequence,2)    
samplingpattern_a_ramp_down = np.arange(0+nsamples_ramp_up+nsamples_sequence,\
                               nsamples_ramp_up+nsamples_sequence+nsamples_ramp_down,2)    

samplingpattern_b_ramp_up = np.arange(1,nsamples_ramp_up,2)    
samplingpattern_b_main_part = np.arange(1+nsamples_ramp_up,\
                               nsamples_ramp_up+nsamples_sequence,2)    
samplingpattern_b_ramp_down = np.arange(1+nsamples_ramp_up+nsamples_sequence,\
                               nsamples_ramp_up+nsamples_sequence+nsamples_ramp_down,2)    
    

for channel in [0]:
    ''' CH1a '''
    # Sequence part1: ramp up
    for t in samplingpattern_a_ramp_up:
        values[channel][t] = 1.0 / nsamples_ramp_up * t \
        * amp_start[channel] * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]
    # Sequence part2: main sequence
    for t in samplingpattern_a_main_part:
        values[channel][t] = (amp_start[channel] + (amp_end[channel] - amp_start[channel])/ nsamples_sequence * (t-nsamples_ramp_up)) \
        * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]
    # Sequence part 3: ramp down
    for t in samplingpattern_a_ramp_down:
        values[channel][t] = (1.0 - (t-nsamples_ramp_up-nsamples_sequence) / nsamples_ramp_down) \
        * amp_end[channel] * np.sin(2 * np.pi * f[channel] * t + phi[channel]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel] * t + phi_mod[channel]*np.pi/180) + off[channel]    
    
    ''' CH1b '''
    # Sequence part1: ramp up
    for t in samplingpattern_b_ramp_up:
        values[channel][t] = 1.0 / nsamples_ramp_up * t \
        * amp_start[channel+1] * np.sin(2 * np.pi * f[channel+1] * t + phi[channel+1]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel+1] * t + phi_mod[channel+1]*np.pi/180) + off[channel+1]
    # Sequence part2: main sequence
    for t in samplingpattern_b_main_part:
        values[channel][t] = (amp_start[channel+1] + (amp_end[channel+1] - amp_start[channel+1])/ nsamples_sequence * (t-nsamples_ramp_up)) \
        * np.sin(2 * np.pi * f[channel+1] * t + phi[channel+1]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel+1] * t + phi_mod[channel+1]*np.pi/180) + off[channel+1]
    # Sequence part 3: ramp down
    for t in samplingpattern_b_ramp_down:
        values[channel][t] = (1.0 - (t-nsamples_ramp_up-nsamples_sequence) / nsamples_ramp_down) \
        * amp_end[channel+1] * np.sin(2 * np.pi * f[channel+1] * t + phi[channel+1]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel+1] * t + phi_mod[channel+1]*np.pi/180) + off[channel+1]    
    



""" 1B) Generate sequence for the regular channels CH2,CH3,CH4 """
for channel in [1,2,3]:
    # Sequence part1: ramp up
    for t in np.arange(nsamples_ramp_up):
        values[channel][t] = 1.0 / nsamples_ramp_up * t \
        * amp_start[channel] * np.sin(2 * np.pi * f[channel+1] * t + phi[channel+1]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel+1] * t + phi_mod[channel+1]*np.pi/180) + off[channel+1]
    # Sequence part2: main sequence
    for t in (np.arange(nsamples_sequence)+nsamples_ramp_up):
        values[channel][t] = (amp_start[channel+1] + (amp_end[channel+1] - amp_start[channel+1])/ nsamples_sequence * (t-nsamples_ramp_up)) \
        * np.sin(2 * np.pi * f[channel+1] * t + phi[channel+1]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel+1] * t + phi_mod[channel+1]*np.pi/180) + off[channel+1]
    # Sequence part 3: ramp down
    for t in (np.arange(nsamples_ramp_down)+nsamples_ramp_up+nsamples_sequence):
        values[channel][t] = (1.0 - (t-nsamples_ramp_up-nsamples_sequence) / nsamples_ramp_down) \
        * amp_end[channel+1] * np.sin(2 * np.pi * f[channel+1] * t + phi[channel+1]*np.pi/180) \
        * np.sin(2 * np.pi * f_mod[channel+1] * t + phi_mod[channel+1]*np.pi/180) + off[channel+1]


""" end generate sequence """


  
""" 2) plot sequence """
# CH1a
ch1a = values[0][0::2]
plt.plot(np.arange(0,nsamples_total,2)/dac_sampling_rate, ch1a, label="Channel CH1a")
plt.xlabel('time [s]')
plt.ylabel('dac value (uint8)')

# CH1b
ch1b = values[0][1::2]
plt.plot(np.arange(0,nsamples_total,2)/dac_sampling_rate, ch1b, label="Channel CH1b")
plt.xlabel('time [s]')
plt.ylabel('dac value (uint8)')    
    
# CH2, CH3, CH4    
for channel in [1,2,3]:
    plt.plot(np.arange(nsamples_total)/dac_sampling_rate, values[channel], label="Channel "+str(channel))
    plt.xlabel('time [s]')
    plt.ylabel('dac value (uint8)')
plt.legend()
plt.show()
""" end plot sequence """




""" 3) write sequence """
# opening serial connection
try:
    # baudrate 1 as dummy variable since it will be ignored
    ser = serial.Serial( serialPort, baudrate, timeout=time_out)
    start = timeit.default_timer()
    
    
    # calculating the number of packages
    num_packages = int (np.ceil(nsamples_total / len_data)) # number of full packages    
        
    for channel in [0,1,2,3]:
        time.sleep(0.05) # delay for avoiding bluetooth buffer overrun
        for package in range(num_packages):
            
            #header
            header = np.zeros(len_header,  dtype=np.uint8)
            header[0] = ord('p')                    # 'p' for programming mode
            header[1] = channel                     # channel number
            header[2] = package >> 8                # package number MSB
            header[3] = package & 0xFF              # package number LSB
            header[4] = num_packages >> 8           # total number of packages MSB
            header[5] = num_packages & 0xFF         # tital number of packages LSB
            
            header_bytes = bytes(header)    
            
            #data
            data = values[channel][len_data*package:len_data*(package+1)]
            #last package
            if data.size != 32:
                data = np.append(data, np.zeros(32-data.size,dtype=np.uint8))
                
                
            data_bytes = bytes(data)
    
            ser.write(header_bytes + data_bytes)
            
            if interface == "USBFS":
                time.sleep(0.0001)

            if interface == "UART":
                time.sleep(0.002) # delay for avoiding bluetooth buffer overrun

finally:
    ser.close()
""" end write seequence """

