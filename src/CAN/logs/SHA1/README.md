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
1. 79.8367  
2. 79.8361  
**Total Average:** 79.8364

---

### Experimental Group
#### Log |  Average Bytes Received Per Second
1. 69.0612  
2. 67.9184  
**Total Average:** 68.4898

---

### Percent Decrease in Average Bytes Received Per Second:
#### Log |  Percent Decrease
1. 6.7484 %  
2. 7.4634 %  
**Total Average:** 7.1059

---

### Time Measurement of Authentication
The average time it took to write the HMAC data (not actually created the HMAC), 
was 3973.04 microseconds, or 3.97304 milliseconds.  

### Time Measurement to Create Full Authentication Messages   
The complete time it took to create the HMAC and timestamp authentication 
messages was 15,443.0204 microseconds, or 15.44302 milliseconds.