

#The idendifier of the connection for the OSC0. 0 for extenal clocking, and 1, 2 or 3 for different internal modes;
OSC0_CONNECTION_ID := 1

#The frequency of the OSC0 signal, crystal, or external reference;
OSC0_FREQUENCY := 16000000

#Is the crystal in low power mode ? Connection should be set appropriately;
OSC0_CRYSTAL_LOW_POWER := 1

#What is the capacitors condiguration ? Provide an integer on 4 bits formatted like :
#	CP16 (bit 3) | CP8 (bit 2) | CP4 (bit 1) | CP2 (bit 0)
OSC0_CAPACITORS := 5
