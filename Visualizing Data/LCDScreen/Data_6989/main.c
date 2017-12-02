#include <msp430.h> 
#include <LCDDriver.h>

#define CALADC12_12V_30C  *((unsigned int *)0x1A1A)   // Temperature Sensor Calibration-30 C
                                                      //See device datasheet for TLV table memory mapping
#define CALADC12_12V_85C  *((unsigned int *)0x1A1C)   // Temperature Sensor Calibration-85 C

unsigned int temp;
volatile float temperatureDegC;
volatile float temperatureDegF;
int adcData;


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

//UART Config
	  // Configure GPIO
	  P2SEL0 |= BIT0 | BIT1;                    // USCI_A0 UART operation
	  P2SEL1 &= ~(BIT0 | BIT1);

	  // Disable the GPIO power-on default high-impedance mode to activate
	  // previously configured port settings
	  PM5CTL0 &= ~LOCKLPM5;

	  // Startup clock system with max DCO setting ~8MHz
	  CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
	  CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
	  CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
	  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
	  CSCTL0_H = 0;                             // Lock CS registers

	  // Configure USCI_A0 for UART mode
	  UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
	  UCA0CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
	  // Baud Rate calculation
	  // 8000000/(16*9600) = 52.083
	  // Fractional portion = 0.083
	  // User's Guide Table 21-4: UCBRSx = 0x04
	  // UCBRFx = int ( (52.083-52)*16) = 1
	  UCA0BR0 = 52;                             // 8000000/16/9600
	  UCA0BR1 = 0x00;
	  UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
	  UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
	  UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

	  while(REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT
	  REFCTL0 |= REFVSEL_0 + REFON;             // Enable internal 1.2V reference

	  /* Initialize ADC12_A */
	  ADC12CTL0 &= ~ADC12ENC;                   // Disable ADC12
	  ADC12CTL0 = ADC12SHT0_8 + ADC12ON;        // Set sample time
	  ADC12CTL1 = ADC12SHP;                     // Enable sample timer
	  ADC12CTL3 = ADC12TCMAP;                   // Enable internal temperature sensor
	  ADC12MCTL0 = ADC12VRSEL_1 + ADC12INCH_30; // ADC input ch A30 => temp sense
	  ADC12IER0 = 0x001;                        // ADC_IFG upon conv result-ADCMEMO

	  while(!(REFCTL0 & REFGENRDY));            // Wait for reference generator
	                                            // to settle
	  ADC12CTL0 |= ADC12ENC;

	  while(1)
	  {
	    ADC12CTL0 |= ADC12SC;                   // Sampling and conversion start

	    __bis_SR_register(LPM0_bits + GIE);     // LPM4 with interrupts enabled
	    __no_operation();

	    // Temperature in Celsius. See the Device Descriptor Table section in the
	    // System Resets, Interrupts, and Operating Modes, System Control Module
	    // chapter in the device user's guide for background information on the
	    // used formula.
	    temperatureDegC = (float)(((long)temp - CALADC12_12V_30C) * (85 - 30)) /
	            (CALADC12_12V_85C - CALADC12_12V_30C) + 30.0f;

	    // Temperature in Fahrenheit Tf = (9/5)*Tc + 32
	    temperatureDegF = temperatureDegC * 9.0f / 5.0f + 32.0f;

	    __no_operation();                       // SET BREAKPOINT HERE
	  }

  //Timer Interrupt Enable
        TA0CCTL0 = CCIE;                                                // enable interrupts
        TA0CTL   = TASSEL_1 + MC_1;                                     // Make the Timer interrupt run off Aclk, up mode
        TA0CCR0  = 32767;                                               // sample rate of 1 Hz

	}

	#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
	#pragma vector=ADC12_VECTOR
	__interrupt void ADC12ISR (void)
	#elif defined(__GNUC__)
	void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
	#else
	#error Compiler not supported!
	#endif
	{
	  switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
	  {
	    case ADC12IV_NONE:        break;        // Vector  0:  No interrupt
	    case ADC12IV_ADC12OVIFG:  break;        // Vector  2:  ADC12MEMx Overflow
	    case ADC12IV_ADC12TOVIFG: break;        // Vector  4:  Conversion time overflow
	    case ADC12IV_ADC12HIIFG:  break;        // Vector  6:  ADC12BHI
	    case ADC12IV_ADC12LOIFG:  break;        // Vector  8:  ADC12BLO
	    case ADC12IV_ADC12INIFG:  break;        // Vector 10:  ADC12BIN
	    case ADC12IV_ADC12IFG0:                 // Vector 12:  ADC12MEM0 Interrupt
	      temp = ADC12MEM0;                     // Move results, IFG is cleared
	      __bic_SR_register_on_exit(LPM4_bits); // Exit active CPU
	      break;
	    case ADC12IV_ADC12IFG1:   break;        // Vector 14:  ADC12MEM1
	    case ADC12IV_ADC12IFG2:   break;        // Vector 16:  ADC12MEM2
	    case ADC12IV_ADC12IFG3:   break;        // Vector 18:  ADC12MEM3
	    case ADC12IV_ADC12IFG4:   break;        // Vector 20:  ADC12MEM4
	    case ADC12IV_ADC12IFG5:   break;        // Vector 22:  ADC12MEM5
	    case ADC12IV_ADC12IFG6:   break;        // Vector 24:  ADC12MEM6
	    case ADC12IV_ADC12IFG7:   break;        // Vector 26:  ADC12MEM7
	    case ADC12IV_ADC12IFG8:   break;        // Vector 28:  ADC12MEM8
	    case ADC12IV_ADC12IFG9:   break;        // Vector 30:  ADC12MEM9
	    case ADC12IV_ADC12IFG10:  break;        // Vector 32:  ADC12MEM10
	    case ADC12IV_ADC12IFG11:  break;        // Vector 34:  ADC12MEM11
	    case ADC12IV_ADC12IFG12:  break;        // Vector 36:  ADC12MEM12
	    case ADC12IV_ADC12IFG13:  break;        // Vector 38:  ADC12MEM13
	    case ADC12IV_ADC12IFG14:  break;        // Vector 40:  ADC12MEM14
	    case ADC12IV_ADC12IFG15:  break;        // Vector 42:  ADC12MEM15
	    case ADC12IV_ADC12IFG16:  break;        // Vector 44:  ADC12MEM16
	    case ADC12IV_ADC12IFG17:  break;        // Vector 46:  ADC12MEM17
	    case ADC12IV_ADC12IFG18:  break;        // Vector 48:  ADC12MEM18
	    case ADC12IV_ADC12IFG19:  break;        // Vector 50:  ADC12MEM19
	    case ADC12IV_ADC12IFG20:  break;        // Vector 52:  ADC12MEM20
	    case ADC12IV_ADC12IFG21:  break;        // Vector 54:  ADC12MEM21
	    case ADC12IV_ADC12IFG22:  break;        // Vector 56:  ADC12MEM22
	    case ADC12IV_ADC12IFG23:  break;        // Vector 58:  ADC12MEM23
	    case ADC12IV_ADC12IFG24:  break;        // Vector 60:  ADC12MEM24
	    case ADC12IV_ADC12IFG25:  break;        // Vector 62:  ADC12MEM25
	    case ADC12IV_ADC12IFG26:  break;        // Vector 64:  ADC12MEM26
	    case ADC12IV_ADC12IFG27:  break;        // Vector 66:  ADC12MEM27
	    case ADC12IV_ADC12IFG28:  break;        // Vector 68:  ADC12MEM28
	    case ADC12IV_ADC12IFG29:  break;        // Vector 70:  ADC12MEM29
	    case ADC12IV_ADC12IFG30:  break;        // Vector 72:  ADC12MEM30
	    case ADC12IV_ADC12IFG31:  break;        // Vector 74:  ADC12MEM31
	    case ADC12IV_ADC12RDYIFG: break;        // Vector 76:  ADC12RDY
	    default: break;
	  }
	}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
    {

    adcData = showChar(temp, int LCDposition);

    if (UCA0IFG && UCTXIFG)
        {                                                               // wait for msb to go through
            UCA0TXBUF = adcData;                                            // then send lsb
        }
}
