## Logs

**Note:** the number of bytes received per second in these
logs are limited by the hardcoded delay in the proccessing
loop of the Arduino microcontrollers. I set this delay to
100 milliseconds (0.1 seconds) initially to allow time to properly
send the CAN messages and flush any ouput streams. Without
the delay, the Arduinos' buffers can overflow. I might also experiment
with different delay values to see if they yield different results.  

---

### Control Group

#### Log |  Average Bytes Received Per Second
1. 79.84
2. 79.84
**Total Average:** 79.84

---

### Experimental Group
#### Log |  Average Bytes Received Per Second
1. 69.06
2. 67.92
**Total Average:** 68.49

---

### Percent Decrease in Average Bytes Received Per Second:
#### Log |  Percent Decrease
1. 6.7484 %  
2. 7.4634 %  
**Total Average:** 7.1059 %

---

### Control v. Experimental Comparison  

#### Log #1 Comparison
![alt tag](https://raw.githubusercontent.com/zach-king/ArduinoStash/master/src/CAN/figures/SHA1/both-01.png)  

---


**Note:** these values are driven by the Arduino Genuino/Uno  board, and
may vary with different CPUs.  

### Time Measurement to Update SHA-1 Hash
The average time it took to update the SHA-1 hash (not actually created the HMAC),
was 3973.04 microseconds, or 3.97304 milliseconds. *Note:* this value is part
of the time measurement to create the full authentication messages (below).  

![alt tag](https://raw.githubusercontent.com/zach-king/ArduinoStash/master/src/CAN/figures/SHA1/time-measurement-01.png)  


### Time Measurement to Fully Sign and Send Authentication Messages
The complete time it took to create the HMAC and timestamp authentication
messages, as well as send them was 15,443.0204 microseconds, or 15.44302 milliseconds.  

![alt tag](https://raw.githubusercontent.com/zach-king/ArduinoStash/master/src/CAN/figures/SHA1/time-measurement-02.png)  