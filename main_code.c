
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "mcc_generated_files/system/system.h"

float threshold = 70;
bool automatic = true;
bool reset = false;

void ADC_Init(void) {
    ANCON0 = 0x01;  // AN0 is analog (AN0-AN7)
    TRISAbits.TRISA0 = 1;  // Set RA0 as an input

    ADCON0 = 0x01;  // Channel 0 (AN0), ADC is enabled, GO/DONE bit is 0
    ADCON1 = 0x00;  // All channels are configured to use VDD & VSS as reference
    ADCON2 = 0xAD;  // Right Justified, FOSC/16, Acquisition time 20 TAD

    ADRESH = 0;  // Clear the ADC result register high
    ADRESL = 0;  // Clear the ADC result register low
}

unsigned int ADC_Read(void) {
    ADCON0bits.GO = 1;  // Start conversion
    while (ADCON0bits.GO_nDONE);  // Wait for the conversion to finish
    return ((unsigned int)((ADRESH << 8) | ADRESL));  // Return combined 10-bit result
}

float ADC_ReadPercentage(void) {
    unsigned int adcValue = ADC_Read();
    return ((((float)adcValue) / 4096)) * 100;  // Convert to percentage (12-bit ADC)
}
void UART_Init(void) {
    // Configure TXSTA register
    TXSTAbits.BRGH = 1; // high baud rate
    TXSTAbits.TXEN = 1; // Enable transmitter
    TXSTAbits.SYNC = 0; // Asynchronous mode
    TXSTAbits.TX9 = 0; // 8-bit transmission
    BAUDCON1bits.BRG16 = 0;
    // Configure SPBRG register for 9600 baud rate
    // Formula: SPBRG = (Fosc / (64 * BaudRate)) - 1
    // For Fosc = 64MHz, BaudRate = 9600, SPBRG = (64000000 / (64 * 9600)) - 1 = 31.25 -> 31
    SPBRG = 308;

    // Configure RCSTA register
    RCSTAbits.SPEN = 1; // Enable serial port
    RCSTAbits.CREN = 0; // Enable continuous receive
    RCSTAbits.RX9 = 0; // 8-bit reception

    // Configure TRISC register
    TRISCbits.TRISC6 = 0; // TX pin as output
    TRISCbits.TRISC7 = 1; // RX pin as input
}

void UART_Write(const int *data) {
    while (*data != '\0') {      // Iterate through each character in the string until the null terminator
        TXREG = *data;           // Load the data to be transmitted
        data++;                  // Move to the next character in the string
        while (!TXSTAbits.TRMT); // Wait until the transmit buffer is empty
    }
}
int main(void) {
    SYSTEM_Initialize();
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD2 = 1;
    LATDbits.LATD3 = 0;

    ADC_Init();  // Initialize the ADC and Uart
    UART_Init();
    float adc_value = ADC_ReadPercentage();
    float thresholdM = threshold;
    while (1) {
        adc_value = ADC_ReadPercentage();
        if(automatic){
            if(adc_value > threshold){
                LATD2 = 0;
                __delay_ms(999);__delay_ms(999);__delay_ms(999); __delay_ms(999);          
            }else{
                LATD2 = 1;
            }
            
        }else{
            if(adc_value > thresholdM){
                // send email with dryness level               
                UART_Write(30);
                thresholdM += 10;
            }else if(adc_value < threshold){
                thresholdM = threshold;
            }
        }
        
        __delay_ms(100);
    }
}