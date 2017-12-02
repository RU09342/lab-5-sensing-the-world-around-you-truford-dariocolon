//

Intro to Embedded: Dr. Tang and Russell Trafford
RBG LED: MSP430FR5994



#include <msp430.h>



#define RED BIT5;           //define some things to make life easier
#define BLUE BIT5;
#define GREEN BIT4;
#define ADC12 BIT1;
#define LED1 BIT0

void LEDInit(void);         //LED initialization function
void TimerBInit(void);      //TIMERB initialization function
void ADCInit(void);         //ADC initialization function
void GPIOInit(void);
void TimerAInit(void);




unsigned int in;
float voltage;
float tempC;
float tempF;

//int size;

//unsigned int count = 0;



int main(void)

{

    WDTCTL = WDTPW + WDTHOLD;               // Stop WDT

    LEDInit();              //initialize LED
    TimerBInit();           //initialize TIMERB
    TimerAInit();           //initialize TIMERB
    GPIOInit();             //Initialize GPIO

    // Initialize the shared reference module
    // REFMSTR=1 => REFCTL used to configure the internal reference

    while(REFCTL0 & REFGENBUSY);            // If ref generator busy, WAIT

   REFCTL0 |= REFVSEL_0 + REFON;           // Enable internal 1.2V reference

    PM5CTL0 &= ~LOCKLPM5; //Disable HIGH Z mode

    ADCInit();              //initialize ADC

    while(!(REFCTL0 & REFGENRDY));          // Wait for ref gen

    __enable_interrupt(); //Enable interrupts.

    while(1)

    {

    if(tempF > 90)

    {
	//yellow
        TB0CCR1 = 0xFF; //R
        TB0CCR2 = 0xFF; //G
        TB0CCR3 = 0x00; //B

    }

    else if(tempF < 90 && tempF >=80 )

    {
	//orange
        TB0CCR1 = 0xFF; //R
        TB0CCR2 = 0xA5; //G
        TB0CCR3 = 0x00; //B

    }

    else if(tempF < 80 && tempF >=70 )

    {

        //red

        TB0CCR1 = 0xFF; //R
        TB0CCR2 = 0x00; //G
        TB0CCR3 = 0x00; //B

    }

    else if(tempF < 70 && tempF >=60 )

    {

        //purple
        TB0CCR1 = 0x80; //Red
        TB0CCR2 = 0x00; //Green
        TB0CCR3 = 0x80; //Blue

    }

    else if(tempF < 60 && tempF >=50 )

    {

        //dark blue
        TB0CCR1 = 0x40; //R
        TB0CCR2 = 0x60; //G
        TB0CCR3 = 0xD0; //B

    }

    else if(tempF < 50 && tempF >=40 )

    {

        //blue
        TB0CCR1 = 0x00; //R
        TB0CCR2 = 0x00; //G
        TB0CCR3 = 0xFF; //B

    }

    else if(tempF < 40 && tempF >=30 )

    {

        //light blue
        TB0CCR1 = 0x10; //Red
        TB0CCR2 = 0x80; //Green
        TB0CCR3 = 0xB0; //Blue

    }



    else if(tempF < 30 )

    {

        //ice
        TB0CCR1 = 0xA0; //R
        TB0CCR2 = 0xFF; //G
        TB0CCR3 = 0xFF; //B

    }



    __bis_SR_register(LPM0 + GIE); // Enter LPM0, interrupts enabled

    __no_operation(); // For debugger

  }

}

//ADC ISR

#pragma vector=ADC12_B_VECTOR
__interrupt void ADC12ISR (void)

{
    in = ADC12MEM0;
    voltage = in * 0.000293;        //converts ADC to voltage
    tempC= voltage/ 0.01;           //converts voltage to Temp C
    tempF=((9*tempC)/5)+32;             //Temp C to Temp F
    while(!(UCA0IFG&UCTXIFG));
    __delay_cycles(100000);
    UCA0TXBUF = tempF;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    ADC12CTL0 |= ADC12SC | ADC12ENC;
}

#pragma vector=EUSCI_A0_VECTOR

__interrupt void USCI_A0_ISR(void)

{

    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))

    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            while(!(UCA0IFG&UCTXIFG));

            switch (count)
            {
            case 0:
                size = UCA0RXBUF;             //size of string is equal to RX BUF
                count = count + 1;            //increment count
                break;
            case 1:
                TB0CCR1 = 255-UCA0RXBUF;      //duty cycle for RED
                count = count + 1;            //increment count
                break;
            case 2:
                TB0CCR2 = 255-UCA0RXBUF;      //duty cycle for GREEN
                count = count + 1;            //increment count
                break;
            case 3:
                TB0CCR3 = 255 - UCA0RXBUF;    //duty cycle for BLUE
                UCA0TXBUF = (size - 0x03);    //TX = size - 3, send rest of HEX string onto next node
                __no_operation();
                count = count + 1;            //increment count
                break;
            default:
                UCA0TXBUF = UCA0RXBUF;
                __no_operation();
                count = count + 1; //increment count

		 if(count > size -1 && UCA0RXBUF == 0x0D){
                    count = 0;  //sets count back to 0, so node can receive another string of HEX
                }
                break;
            }
            break;

        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;

        default:
               break;

    }

}


//Initialize RGB LED
void LEDInit(void){


    P3DIR |= RED;     //Pin 5.1
    P3SEL1 &= ~RED;
    P3SEL0 |= RED;

    P3DIR |= GREEN;    //Pin 3.4
    P3SEL1 &= ~GREEN;
    P3SEL0 |= GREEN;

    P1DIR |= BLUE;     //Pin 1.5
    P1SEL1 &= ~BLUE;
    P1SEL0 |= BLUE;

}

//Initialize TimerB
void TimerBInit(void) {

    TB0CCTL1 = OUTMOD_3; //Set OUTMOD_3 (set/reset) for CCR1, CCR2
    TB0CCTL2 = OUTMOD_3;
    TB0CCTL3 = OUTMOD_3;

    //Set initial values for CCR1, CCR2

    TB0CCR1 = 200; //Red
    TB0CCR2 = 200; //Green
    TB0CCR3 = 200; //Blue
    TB0CCR0 = 255; //Set CCR0 for a ~1kHz clock.
    TB0CTL = TBSSEL_2 + MC_1; //Enable Timer B0 with SMCLK and up mode.
}

void TimerAInit(void) {

    TA0CCTL0 = CCIE;
    TA0CCTL1 = OUTMOD_3; //Set OUTMOD_3 (set/reset) for CCR1, CCR2
    TA0CCR1 = 256; //Red
    TA0CCR0 = 4096-1; //Set CCR0 for a ~1kHz clock.
    TA0CTL = TASSEL_2 + MC_1 + ID_3;

}



// Initialize ADC12_A

void ADCInit(void)

{
    ADC12CTL0 = ADC12SHT0_2 + ADC12ON;      // Set sample time
    ADC12CTL1 = ADC12SHP;                   // Enable sample timer
    ADC12CTL2 |= ADC12RES_2;                // 12-bit conversion results
    ADC12MCTL0 = ADC12INCH_4 | ADC12VRSEL_1;// Vref = AVCC, Input
    ADC12IER0 |= ADC12IE0;                  // Enable ADC conv complete interrupt
    P1OUT = BIT0;
}

void GPIOInit()

{
    P1OUT &= ~BIT0;                         // Clear LED to start
    P1DIR |= BIT0;                          // P1.0 output
    P5SEL1 |= ADC12;                         // Configure P1.4 for ADC
    P5SEL0 |= ADC12;
}
