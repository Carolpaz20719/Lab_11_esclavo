/*
 * File:   Esclavo.c
 * Author: Carolina Paz
 *
 * Created on 11 de mayo de 2022, 05:38 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF           // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF          // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF          // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF             // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF            // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF          // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF           // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF          // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF            // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include <stdint.h>
/*------------------------------------------------------------------------------
 * CONSTANTES 
 ------------------------------------------------------------------------------*/
#define _XTAL_FREQ 1000000           // Frecuencia de oscilador
#define BOTON      PORTBbits.RB0     // Boton para incrementar
#define BOTON2     PORTBbits.RB1     // Boton para decrementar

/*------------------------------------------------------------------------------
 * VARIABLES 
 ------------------------------------------------------------------------------*/
char valor;                          // Variable que servira como contador   

/*------------------------------------------------------------------------------
 * PROTOTIPO DE FUNCIONES 
 ------------------------------------------------------------------------------*/
void setup(void);

/*------------------------------------------------------------------------------
 * INTERRUPCIONES 
 ------------------------------------------------------------------------------*/
void __interrupt() isr (void){
    
    if (PIR1bits.SSPIF){               // Revisar si fue interrupcion de SPI 
        SSPBUF = valor;                // SSPBUF a valor
        PIR1bits.SSPIF = 0;            // Limpiamos bandera de interrupcion
    }
    
    if(INTCONbits.RBIF){               // Fue interrupción del PORTB
        if(!BOTON){                    // Verificamos si fue RB0 quien generó la interrupción
            valor++;}                  // Incremento a valor

        if(!BOTON2){                   // Verificamos si fue RB1 quien generó la interrupción
            valor--;}                  // Decrementamos a valor

        INTCONbits.RBIF = 0;           // Limpiamos bandera de interrupción
    }
        
    return;
}
/*------------------------------------------------------------------------------
 * CICLO PRINCIPAL
 ------------------------------------------------------------------------------*/
void main(void) {
    setup();
    while(1){}
    return;
}
/*------------------------------------------------------------------------------
 * CONFIGURACION 
 ------------------------------------------------------------------------------*/
void setup(void){
    ANSEL = 0;                  // Usaremos I/O digitales
    ANSELH = 0;                 
    
    TRISB = 0xFF;               // PORTB como entrada
    PORTB = 0;                  // Limpiar PORTB
    
    TRISA = 0b00100000;         // Colocar el pin RA5 como entrada
    PORTA = 0;                  // Limpiar el PORTA
    
    // Configuracion del reloj
    OSCCONbits.IRCF = 0b100;    // 1MHz
    OSCCONbits.SCS = 1;         // Reloj interno
    
    // Configuracion de SPI
    // Configs del esclavo
    TRISC = 0b00011000; // -> SDI y SCK entradas, SD0 como salida
    PORTC = 0;

    // SSPCON <5:0>
    SSPCONbits.SSPM = 0b0100;   // -> SPI Esclavo, SS hablitado
    SSPCONbits.CKP = 0;         // -> Reloj inactivo en 0
    SSPCONbits.SSPEN = 1;       // -> Habilitamos pines de SPI
    // SSPSTAT<7:6>
    SSPSTATbits.CKE = 1;        // -> Dato enviado cada flanco de subida
    SSPSTATbits.SMP = 0;        // -> Dato al final del pulso de reloj

    PIR1bits.SSPIF = 0;         // Limpiamos bandera de SPI
    PIE1bits.SSPIE = 1;         // Habilitamos int. de SPI
    INTCONbits.PEIE = 1;        // Se habilitan interrupciones de periféricos
    INTCONbits.GIE = 1;         // Se habilitan interrupciones globales
    
    //Interrupcion del puerto B
    OPTION_REGbits.nRBPU = 0;   // Habilitamos resistencias de pull-up del PORTB

    WPUBbits.WPUB0  = 1;        // Habilitamos resistencia de pull-up de RB0
    WPUBbits.WPUB1  = 1;        // Resistencia resistencia de pull-up de RB1
    INTCONbits.RBIE = 1;        // Habilitamos interrupciones del PORTB
    IOCBbits.IOCB0  = 1;        // Habilitamos interrupción por cambio de estado para RB0
    IOCBbits.IOCB1  = 1;        // Habilitamos interrupción por cambio de estado para RB1
    INTCONbits.RBIF = 0;        // Limpiamos bandera de interrupción
}