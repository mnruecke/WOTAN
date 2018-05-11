import os
import os.path

import numpy as np
import matplotlib.pyplot as plt

signal = np.loadtxt("data_1.txt", unpack=True)

TperSample = 0.0000005 #s
Fs = 1/TperSample      # 2 MHz
numOfSamples = signal.size
T = TperSample * numOfSamples


t=np.linspace(0,T,numOfSamples)
signal2 = 500*np.sin(2*np.pi*1000*t)+5
"""
plt.plot(t,signal,t,signal2)
plt.show()
"""
frequencies = np.fft.fftfreq(numOfSamples, TperSample)
FFTSignal = np.fft.fft(signal)

plt.plot(frequencies,np.abs(FFTSignal),'ro')
plt.show()