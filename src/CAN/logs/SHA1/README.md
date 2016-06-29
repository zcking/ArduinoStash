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
**Total Average:** 7.1059 %

---

**Note:** these values are driven by the Arduino Genuino/Uno  board, and 
may vary with different CPUs.  

### Time Measurement to Write HMAC Data
The average time it took to update the HMAC hash (not actually created the HMAC), 
was 3973.04 microseconds, or 3.97304 milliseconds. *Note:* this value is part 
of the time measurement to create the full authentication messages (below).  

### Time Measurement to Create Full Authentication Messages   
The complete time it took to create the HMAC and timestamp authentication 
messages was 15,443.0204 microseconds, or 15.44302 milliseconds.  

### Time Measurement to Authenticate a Message  
The complete time it took to authenticate the HMAC and verify 
the timestamp freshness is 34,982.2222 microseconds, or 34.9822 milliseconds.  