README for Visualizing Data


***CONTENTS***

-Drivers used
-RGB LED
-LCD Display
-Implementation
-References


# LCD Driver

The LCD Driver written by Russell Trafford was used to complete this lab. See the respective README for more information.


# RGB LED

In this part of the project, the ADC was used and it's value was utilized to change the color of an RGB LED. A thermistor was the sensor chose
for this assignment, and the LED changes color with respect to the temperature (blue in cold temperature, pruple below room temp., red at room temp., orange above room temp. yellow in hot temp.

To do this, all that had to be done with the ADC reading is change the pulse width of R, G, and B with simple math based on the temperature range. The chart below shows the gist of the
LEDs' change
---
LED brightness approximation with respect to temp. approximation  |
---								  								  

Temperature ('F)	LED color				  

	>90		yellow					  

	80-90		orange					  

	70-80		red					  

	60-70		purple					  

	50-60		dark blue				  

	40-50		blue					  

	30-40		light blue				  

	<30		ice (white with blue)			  

---


# LCD Display

This portion of the lab made use of the MSP430FR6989's LCD display in order to produce a human-legible output for the reading of a temperature sensor. All that had to be done was translating the input of the ADC
onto the LCD display output.

# Implementaion

This code can be utilized and implemented on a variety of different projects/applications. The code is compatible with virtually any sensor with an analog output
as long as the sensor fits the specifications of the MSP430. Only minor calculations need to be changed in order to read any sensor with this code.


# References

Help from:
	Russell Trafford
	Ben Jukus
