
1. In one of the terminals, type sudo /usr/src/qusb_acq/qusb_acq -P rxdsp-south_pole -o /daq/ -d 0.5 -p 1,2 
This will start the acquisition
2. In the second terminal, type sudo  /usr/src/complex_proc/cprtd -f 350 -F 680
3. In the third terminal, type cd /usr/src/rtdgui/
Then type sudo /usr/src/rtdgui/rtdgui hf2_config.input
