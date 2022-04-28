#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include"i2c_master_noint.h"
#include"ssd1306.h"
#include"font.h"

// DEVCFG0
#pragma config DEBUG = OFF       // disable debugging
#pragma config JTAGEN = OFF      // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF         // disable flash write protect
#pragma config BWP = OFF         // disable boot write protect
#pragma config CP = OFF          // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL //  use fast frc oscillator with pll
#pragma config FSOSCEN = OFF  // disable secondary oscillator
#pragma config IESO = OFF     // disable switching clocks
#pragma config POSCMOD = OFF   // primary osc disabled
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD       // disable clock switch and FSCM
#pragma config WDTPS = PS1048576    // use largest wdt value
#pragma config WINDIS = OFF         // use non-window mode wdt
#pragma config FWDTEN = OFF         // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz fast rc internal oscillator
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0     // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF  // allow multiple reconfigurations
void blink();

int main() {

  __builtin_disable_interrupts(); // disable interrupts while initializing
                                  // things

  // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
  __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

  // 0 data RAM access wait states
  BMXCONbits.BMXWSDRM = 0x0;

  // enable multi vector interrupts
  INTCONbits.MVEC = 0x1;

  // disable JTAG to get pins back
  DDPCONbits.JTAGEN = 0;

  // B4 is input
  TRISBbits.TRISB4 = 1;

  // A4 is output
  TRISAbits.TRISA4 = 0;


    i2c_master_setup();
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    __builtin_enable_interrupts();

    char message[50];
    int time;
    int count=0;
    while (1) {


     blink();

     _CP0_SET_COUNT(0);
     ssd1306_update();
     time = _CP0_GET_COUNT();

     sprintf(message, "ME433 YANG");
     ssd1306_drawMessage(10, 10, message);
     sprintf(message, "Count: %d", count);
     ssd1306_drawMessage(10, 18, message);
     count++;
     sprintf(message, "fps = %0.2f", 24000000.0/time);
     ssd1306_drawMessage(60, 25, message);
     ssd1306_update();

    }
}
void blink(){
      // Create a LED heartbeat
      LATAbits.LATA4 = 1;
      _CP0_SET_COUNT(0);
      while (_CP0_GET_COUNT() < 24000000 / 5 ) {
      }
      LATAbits.LATA4 = 0;
      _CP0_SET_COUNT(0);
      while (_CP0_GET_COUNT() < 24000000 / 5) {
      }
}