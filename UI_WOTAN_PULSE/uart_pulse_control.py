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
%   - this is a code stub for running "WOTAN_PULSE"
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
com_port       = '\\\\.\\COM4'
baudrate       = 115200
time_out       = 1

bufInputSize   = 4000
bytesPerSample = 2

""" END - main settings """


# 1) start measurement on PSoC and get data
try: # open and interact with serial port 
    ser = serial.Serial( com_port, baudrate, timeout=time_out)
    
    # run MPI sequence on psoc
    ser.write(b'A') # A..E -> Gain 1,4,8,16,32
    time.sleep(0.3)
    
    ser.write( b's' )
    time.sleep(0.001)
    
    # get data as byte stream 
    adc_data_bin = ser.read(bufInputSize)
    # transform byte stream into int16 array
    adc_data_int16 = struct.unpack('>'+'H'*int(len(adc_data_bin)/bytesPerSample),adc_data_bin)
    
finally: # close serial port
    ser.close()


""" Visualize results """
plt.plot(adc_data_int16)