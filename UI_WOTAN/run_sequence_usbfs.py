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
baudrate         = 9600 # USBFS component ignores this parameter
time_out         = 1 # [s]; this script should retrieve the 60 kB data in << 1s

# 0.2 sequence details
bytesPerSample   = 2
timePerSample    = 0.0005 # sample time in ms
sequDuration     = 15 # sequence duration in ms (not counting start- and end ramp)
numOfSamples     = int(sequDuration/timePerSample)
bufInputSize     = numOfSamples * bytesPerSample

adcVoltPerBit    = 0.0005 # scaling factor for ADC data

nameDataFiles    = 'data'  # save data to files with continuous numbering (ascii table)

""" END - main settings """

# 0.4) list of commands defined in WOTAN
p_run_sequ  = b'r'
p_get_data  = b'o'
p_trig_dir  = b'x' # "Trigger direction", Values: 'x', 'y', 'x' use P3.0 as trigger output, 'y': use P3.0 as Trigger input

# 1) start measurement on PSoC and get data
try: # open and interact with serial port 
    ser = serial.Serial( com_port, baudrate, timeout=time_out)
    
    start = timeit.default_timer()
#   run MPI sequence on pso
    ser.write( b'5' )
    time.sleep(0.001)
    ser.write( p_trig_dir )
    time.sleep(0.001)
    ser.write( p_run_sequ )
    time.sleep(0.030) # time between trigger and end of data acquisition
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
    


if len(adc_data_bin) == numOfSamples*bytesPerSample: # check if data was received completely
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
    dat_sig  = adc_data_corr * adcVoltPerBit
    plt.plot( dat_time, dat_sig, dat_time, dat_sig,'+')
    plt.xlabel('time [ms]')
    plt.ylabel('signal [V]')
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
          "flash programming for sequence calculation.)\n\n\n")
        
print( "Data transfer time: " + "{0:0.3f}".format(stop-start) + "s"  )

