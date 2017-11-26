# WOTAN
Firmware and scripts for running a control unit of a 3D MPI scanner. It is based around the PSoC 5LP mixed-signal chip from Cypress Semiconductor. The firmware was designed using Creator 4.1 for the CY8CKIT-059-kit with the CY8C5888LTI-LP097. This project started as a conference contribution for the IWMPI 2018 Hamburg. 

The MPI scanner control module has 4 transmit channels (4x 250 kS/s, 8 bit, up to 4 MS/s possible) and one receive channel (2 MS/s, 12 bit). Recording time: 15 ms. Data transfer over USB-to-UART with 115200 Baud (higher transfer rates are possible with precise clock adjustments or using the USBFS component included on the PSoC with 12.5 Mbps). 

Basic operation evaluation of the PSoC firmware doesn't require any external components besides the CY8CKIT-059 breakout board, however the design uses current DACs which need to be terminated with 12 kOhm in order to give valid wave form amplitudes (output goes to saturation otherwise). The code was written with Creator 4.1.

Basic test 1 (requires putty.exe or a similar hyperterminal): connect with USB port and find  the asociated COM port in your system listed in Control Panel >> Device Manager >> Ports (for windows). Set the terminal to the COM port and 115200 baud. Pressing the reset button on the board shows a screen with options.

Basic test 2: Using the m-files for octave or matlab (uart_control_octave.m, uart_control_matlab.m) in the project folder:  after programming the board and adjusting the com-port number in the m-file to the one corresponding to your system, the script should simply run and display a graph showing 15 ms of the Rx-port with 2 MS/s. 
