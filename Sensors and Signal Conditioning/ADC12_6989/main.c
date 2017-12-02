#include <msp430.h>
/* Based on Resource Explorer Code
 * Last edited: 11/9/2017
 */
/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR69xx Demo - ADC12_B, Sample A10 Temp and Convert to oC and oF
//
//  Description: A single sample is made on A10 with internal reference voltage
//  1.2V. Software manually sets ADC12SC to start sample and conversion and
//  automatically cleared at EOC. It uses ADC12OSC to convert the sameple.
//  The Mainloop sleeps the MSP430 in LPM4 to save power until ADC conversion
//  is completed. ADC12_ISR forces exit from LPMx in on exit from interrupt
//  handler so that the mainloop can execute and calculate oC and oF.
//  ACLK = n/a, MCLK = SMCLK = default DCO ~ 1.045MHz, ADC12CLK = ADC12OSC
//
//  Un-calibrated temperature measured from device to device will vary due to
//  slope and offset variance from device to device - please see datasheet.
//  Note: This example uses the TLV calibrated temperature to calculate
//  the temperature
// (the TLV CALIBRATED DATA IS STORED IN THE INFORMATION SEGMENT, SEE DEVICE DATASHEET)
//
//                MSP430FR6989
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |A10              |
//
//   William Goh
//   Texas Instruments Inc.
//   April 2014
//   Built with IAR Embedded Workbench V5.60 & Code Composer Studio V6.0
//******************************************************************************

unsigned int adcData;                                                   // Variable to hold data from the memory register of the ADC
char lsb;                                                               // Holds the Least Significant Bits on adcData
char msb;                                                               // Holds the Most Significant Bits on adcData

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                           // watchdog timer

//UART 
    // Startup clock system with max DCO setting ~8MHz
        CSCTL0_H = CSKEY >> 8;                                          // Unlock CS registers
        CSCTL1 = DCOFSEL_3 | DCORSEL;                                   // Set DCO to 8MHz
        CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;            // Selects the source for each clk
        CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;                           // Set all dividers, AClk, SMClk, MClk divided by 1
        CSCTL0_H = 0;                                                   // Lock CS registers

        P2SEL0 |= BIT0;                                                 // UART TX
        UCA0CTLW0 |= UCSWRST;                                           // State machine reset
        UCA0CTLW0 |= UCSSEL1;                                           // Uses SMCLK as source
        UCA0BR0    = 52;                                                // Modulation
        UCA0MCTLW  = UCBRF_1 | UCOS16 | 0x4900;
        UCA0BR1    = 0x00;                                              // Modulation
        UCA0CTLW0 &= ~UCSWRST;

//ADC Configuration
        P8SEL0    |= BIT7;                                              // ADC data comes in to 8.7
        ADC12MCTL0  = ADC12INCH_4;                                      // Memory Control set to input channel 4
        ADC12CTL0  = ADC12ON + ADC12SHT0_8 + ADC12MSC;                  // turn on ADC and set sample hold time multiplier(8*) and set multi signal conversion
        ADC12CTL1  = ADC12SHP + ADC12CONSEQ_2;                          // sets signal based on timer
        ADC12CTL0 |= ADC12ENC |  ADC12SC;                               // interrupt enable and start conversion

//Timer Interrupt Enable
        TA0CCTL0 = CCIE;                                                // enable interrupts
        TA0CTL   = TASSEL_1 + MC_1;                                     // Make the Timer interrupt run off Aclk, up mode
        TA0CCR0  = 32767;                                               // sample rate of 1 Hz

        __bis_SR_register(LPM4_bits + GIE);                             // enter low power mode 4, interrupts enabled
while (1){                                                              // don't return 0
         }
    return 0;                                                           // unreachable intentionally
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
    {
    adcData = ADC12MEM0;                                                // store ADC to memory
    msb = adcData >> 8;                                                 // shift 12 bit input so msb reads 0000 followed by four most sig bits of input
    lsb = adcData & 0xFF;                                               // lsb gets last eight bits of 12 bit input

    UCA0TXBUF = msb;                                                    // send msb

    if (UCA0IFG && UCTXIFG)
        {                                                               // wait for msb to go through
            UCA0TXBUF = lsb;                                            // then send lsb
        }
}
