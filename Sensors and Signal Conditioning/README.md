README for Sensor and Signal Conditioning


***CONTENTS***

-Timer Configuration

-Analog to Digital Converter

-References

# Timer Config.

	The clock used in this code is configured:

		TA0CTL   += TASSEL_1	Auxiliary Clock (32 KHz)
		TA0CTL   += MC_1	Up-mode
		CCR0 = 32767
			Interrupt triggers once per second (1 Hz)

# ADC

	The ADC in this project is 12 bits. This means the resolution is Vin*(2^-12) = Vin*(0.000244140625)
	This section of the README covers both the 12 and 10 bit ADC usable by the MSP430FR6989

	Since the 12 and 10 bit ADC contain more than 8 bits, we need to used two chars (bytes) to hold the data.
	Chars lsb and msb (least and most significant bytes) contain the data. The following example shows how the data is processed.

	Example: 12 bit ADC input=	ABCDEFGHIJKL

		msb gets the ADC input shifted right by 4 bits

			msb = ABCDEFGHIJKL >> 4 = ABCDEFGH

		lsb gets the ADC input shifted left by 8 bits

			lsb = ABCDEFGHIJKL << 8 = IJKLxxxx

		msb is sent first, when the buffer is cleared, send lsb so the two consecutive bytes send the following

			ABCDEFGH IJKLxxxx

	10 bit ADC follows the same logic, seen below
	Example: 10 bit ADC input = ABCDEFGHIJ

		msb = ABCDEFGHIJ >> 2 = ABCDEFGH
		lsb = ABCDEFGHIJ << 10 = IJxxxxxx

		send msb, followed by lsb

			ABCDEFGH IJxxxxxx

	We chose the final format [ABCDEFGH IJKLxxxx] over [xxxxABCD EFGHIJKL] because in our format,
	the second byte (lsb) can be completely disregarded in cases when the hardware error is greater than (Vin*0.001953125) or Vin*(2^-9)
	or cases where that level of accuracy is not required. Power, time, and memory can be saved by excluding the lsb calculation.

	In the format [xxxxABCD EFGHIJKL], if the lsb is disregardded/not calculated, data accuracy is limited to (Vin*0.0625) or Vin*(2^-4).


# REFERENCES
	Help from:
		Russell Trafford
		Nick Gorab
		Code Composer Studio Resource Explorer
