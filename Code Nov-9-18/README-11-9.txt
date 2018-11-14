Notes on code:
Any printlns use storage memory

In "Integrated Code functional", it writes/reads to/from an NFC tag and transmits the data read.


The receiver uses the code from Rx in the RFM9x folder, it receives the data and prints them out in the monitor.
I have not been able to store the data received (I tried implementing the getBuffer function to return a string instead of just
printing it out.