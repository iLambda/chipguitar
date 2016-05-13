#include "ps2simulator.h"
#include "keycodes.h"
#include "notemap.h"

unsigned char state = 0;

unsigned char lasttrig = 0;
unsigned char oldtrigbit = 0;
unsigned char trigbit = 0;
unsigned char notemode = 0x00;
unsigned char* notemap = notemap_default;

void keypress(unsigned char keycode) {
   ps2sim_send(keycode);
   delay_ms(350);
   ps2sim_send(0xF0);
   delay_us(200);
   ps2sim_send(keycode);
   delay_ms(350);
}

void keydown(unsigned char keycode) {
   // keydown
   ps2sim_send(keycode);
   // vumeter
   PORTD.B2 = 1;
   // wait
   delay_us(300);
}

void keyup(unsigned char keycode) {
   // keyup
   ps2sim_send(0xF0);
   delay_us(200);
   ps2sim_send(keycode);
   // vumeter
   PORTD.B2 = 0;
   // wait
   delay_us(300);
}

// polling input
void poll(void) {
     // gather input
     state = ~PORTB;
     // notemode is lower four bits :
     //    notemode & 0x07 == note
     //    notemode & 0x08 == is sharp
     notemode = state & 0x0F;
     
     // trigbit update
     oldtrigbit = trigbit;
     trigbit = state & 0x80;
     
     // check trig statis
     if (trigbit) {
       if (!oldtrigbit) {
           // keydown the one we trig'd last
           if (lasttrig) { keyup(lasttrig); }
           // we trig a note
           lasttrig = notemap[notemode];
           keydown(lasttrig);
        }
     } else {
       // keydown the one we trig'd last
       if (lasttrig) { keyup(lasttrig); }
       // reset lasttrig
       lasttrig = 0;
     }
     
     // wait
     delay_us(300);
}

// entry point
void main(void) {
     // I/O configuration
     ADCON1 = 0x0F;
     TRISA = 0xFF;
     TRISB = 0xFF;
     TRISC = 0xFF;
     TRISD = 0;
     TRISE = 0;
     
     // D2 is ready led
     PORTD.B2 = 1;
     
     // init PS/2
     ps2sim_init(&PORTD, 1, &PORTD, 0);

     // wait for 2 sec
     delay_ms(2000);
     // ready   
     PORTD.B2 = 0;
     
     // main loop
     while(1) { poll(); }
}