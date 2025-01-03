#include "../../system/interrupt.h"
#include "../../system/system.h"
#include "../pins.h"
//#include "globals.h"
extern float threshold;
extern bool automatic;
extern bool reset;

void  INTERRUPT_Initialize (void)
{
    IOCBbits.IOCB4 = 1;
    IOCBbits.IOCB5 = 1;
    IOCBbits.IOCB6 = 1;
    IOCBbits.IOCB7 = 1;
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = 1;
    INTCONbits.GIE = 1;

    // // Disable Interrupt Priority Vectors (16CXXX Compatibility Mode)
    RCONbits.IPEN = 0;
    
    
}

/**
 * @ingroup interrupt
 * @brief Services the Interrupt Service Routines (ISR) of enabled interrupts and is called every time an interrupt is triggered.
 * @pre Interrupt Manager is initialized.
 * @param None.
 * @return None.
 */
void __interrupt() INTERRUPT_InterruptManager (void)
{
    // interrupt handler
    if (INTCONbits.RBIF == 1) {
        // handle
        unsigned char combined_value = PORTB >> 4; // Shift right by 4 to align RB4-RB7 with LSBs
        if(combined_value == 0){
            LATD2 = 1;
            automatic = true;            
        }else if(combined_value == 2){
            LATD2 = 1;
            automatic = false;            
        }else if(combined_value == 6){            
            LATD2 = ~LATD2;
            automatic = false; 
        }else if(combined_value == 4){
            asm("RESET");
        }
        INTCONbits.RBIF = 0;
        INTCONbits.RBIE = 1;
    } 
}