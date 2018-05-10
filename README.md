# WOTAN
Firmware and scripts for running a control unit of a 3D MPI scanner. It is based around the PSoC 5LP mixed-signal chip from Cypress Semiconductor. The firmware was designed using Creator 4.1 for the CY8CKIT-059-kit with the CY8C5888LTI-LP097. This project started as a conference contribution for the IWMPI 2018 Hamburg. 

![Image of 3D TWMPI control unit](https://github.com/mnruecke/WOTAN/blob/master/wotan.png)

## Basic Features
The MPI scanner control module has 4 transmit channels (4x 250 kS/s, 8 bit, up to 4 MS/s possible) and one receive channel (2 MS/s, 12 bit). Recording time: 15 ms. Data transfer over USB-to-UART with 115200 Baud. The onboard programmer allows higher transfer rates with precise clock adjustments. Using the USBFS component included on the PSoC allows up to 12.5 Mbps (micro B connector). The common HC-05 bluetooth module can also be connected to the control unit (up to 1.3 Mbps).

## Getting Started
1. For programming the module, Creator 4.1 or higher is necessary, which can be downloaded for free from Cypress Semiconductor (www.cypress.com).

2. Opening the workspace "WOTAN" shows two projects.
*WOTAN* is the version used for the TWMPI scanner.
It is recommended to add an external oscillator crystal (the accuracy of the chip intern oscillator is only ~0.25% and shows a strong temperature dependence).
Arbitrary sequences can be generated and transfered to the PSoC via the USBFS using the *.\UI\_WOTAN\\generate\_sequency\_usbfs.py* Python script.
*WOTAN_PULSE* is a version that contains only the receive chain and a trigger output.

3. For writing the firmware to the chip connect programmer to the PSoc and the PC via USB port, select *WOTAN* as active project (right mouse click) and press CTRL-F5 for compiling and programming. 

4. Open the device manager in windows in order to see as which com-port the USBUART of the onboard programmer enumerates.

5. Start *putty.exe* or any hyperterminal of your choice and connect it to this com-port number. The baudrate is 115200.

6. Pressing the reset button on the programmer or pressing 'e' in the hyperterminal resets the programm and all available options are shown in the hyperterminal

7. The numbers 1..4 select the receive channel to the DAC outputs 1..4, Pressing '5' measures the signal at GPIO 0.7. Pressing 's' starts a measurement of 15 ms and shows the results as ascii table (pressing any key will interupt the output).

8. The measurement can be easily automated. Example scripts ready to run (after changing the com-port number) are included for octave, matlab and python 3 in *.\\UI\_WOTAN\\example_scripts\\*. The matlab version will run without additional packages after changing the com-port number in the script. Octave needs the signal-toolbox. Python requires the 'pyserial' package, in addition to 'numpy' and 'matplotlib' for data visualization. The later are typically included in scientific oriented python environments like anaconda or spyider. Packages can be easily added to python by using the package manager pip.  

9. The micro B connector on the board is connected to a fast USBUART which works the same way as the USBUART on the programmer, but with 12.5 Mbps.

10. For using the HC-05 bluetooth module: a) connect it as indicated in the schematic in the Creator IDE and b) go into *main.c* and comment the function *uart_interface()*  and uncomment the function *ble_interface()* instead. The HC-05 module or similar bluetooth modems enumerate as a virtual com port and it can be used the same way as the USBUART included on the programmer. 

11. Push the boundaries of MPI hardware development beyond good and evil


## Running the module via python scripts
The folder *.\\UI_WOTAN* contains two python scripts for creating and running sequences.
The Firmware for the PSoC Chip allows to transfer an arbitrary sequence via the USBFS interface (see *.\\UI\_WOTAN\\generate_sequence_usbfs.py*). 
The sequence is then stored in the Flash. On first startup after loading the firmware to PSoC it generates an default sequence.
The PSoc can be controlled using the Python script *.\\UI\_WOTAN\\run_sequence_usbfs.py*. 

## .\\UI\_WOTAN\\generate\_sequency\_usbfs.py
Python script that allows to send an arbitrary sequence to the PSoc with Firmware *WOTAN* via the fast USBFS interface, so no programmer is needed to change the sequence.
The *com_port* variable must be adapted to the Port of the PSoC on the computer which can be found in Windows under *Device Manager*.
It is possible to generate a sequence for each output channel in the form  

amp * sin(2pi * f + phi) * sin(2pi * f\_mod + phi\_mod) + off

where
*amp* is the amplitude  
*f* is the main frequency of the sequence  
*phi* is the phase of the sequence  
*f_mod* is the frequency of the modulated sin function  
*phi_mod* is the phase of the modulated sin function  
*off* is the offset  

The sequence starts with a ramp up and ends with a ramp down interval

## .\\UI\_WOTAN\\run\_sequence\_usbfs.py
Python script that allows to start a sequence and to receive the data of the receive channel via the fast USBFS interface.
The *com_port* variable must be adapted to the Port of the PSoC on the computer which can be found in Windows under *Device Manager*. The script plots the data and saves the data as ascii table with continuous numbering in the same folder as the script.


