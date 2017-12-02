

# Diptrace
The dimensions of the board were decided based on how it would fit into a breadboard to further develop circuits outside of the MSP Development board. The dimension that is most important for the board is the width between the headers that will go into the breadboard. The headers are 0.7 inches apart, this is due to the size between holes in the board and the limitations of a 2 layer board.  
The Gerber files can be found in the "Gerber files" folder and the .sch and .pcb files can be found in the "PCB Files" folder along with two .png's of the board.  
# Bill of Materials
## Surface Mount Resistor
For the Surface Mount Resistor its value was established in class, 47 kOhm. The package on the breakout board is the typical resistor casing, 0402_RES. This case type and resistor value was found on Digi-Key, the link seen below. This resistor was chosen because it fulfills the board requirements and it is the least expensive.
47 kOhm: https://www.digikey.com/product-detail/en/yageo/RC0402FR-0747KL/311-47.0KLRTR-ND/726616

## Surface Mount Capacitor
There are three capacitors on the board: 10 uF, 1 nF, and 100 nF. Each capacitor was found by searching for the package type and then the capacitance value, to further narrow the selections the least expensive were chosen. For the application of this board nothing special is particularly needed, since we are not dealing with high frequency or anything too susceptible to noise.
100 nF: https://www.digikey.com/product-detail/en/avx-corporation/0402ZD104KAT2A/478-1129-2-ND/563239
10 uF : https://www.digikey.com/product-detail/en/kemet/C0805C106K8PACTU/399-4925-2-ND/1090830
1 nF  : https://www.digikey.com/product-detail/en/kemet/C0805C102K5RACTU/399-1147-2-ND/411146
