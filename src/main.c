#include "ps2simulator.h"
#include "keycodes.h"
#include "notemap.h"

#define LED_OFF   0x00
#define LED_GREEN 0x01
#define LED_RED   0x02

unsigned char state = 0;

unsigned char oldtrigbit = 0;
unsigned char trigbit = 0;
unsigned char lasttrig = 0;

unsigned char selectbit = 0;
unsigned char oldselectbit = 0;
unsigned char selectmode = 0x00;

unsigned char pulsebit = 0;
unsigned char oldpulsebit = 0;

unsigned char tablebit = 0;
unsigned char oldtablebit = 0;

unsigned char notemaps[NOTEMAP_ALT_COUNT][NOTEMAP_LENGTH] = {{ NOTEMAP_EMPTY }, { NOTEMAP_EMPTY }, { NOTEMAP_EMPTY }, { NOTEMAP_EMPTY }};
unsigned char notemap = 0;
unsigned char notemode = 0x00;
unsigned char noteincrbit = 0;
unsigned char oldnoteincrbit = 0;

unsigned short buffer = 0;
unsigned char k = 0;
unsigned char j = 0;

unsigned short keydata = 0, special = 0, down = 0;

sbit PS2_Data at RD6_bit;
sbit PS2_Clock at RD7_bit;
sbit PS2_Data_Direction at TRISD6_bit;
sbit PS2_Clock_Direction at TRISD6_bit;

void led(unsigned value) {
     // set led
     PORTD = (PORTD & ~0x0C) | (value << 2);
}


void keydown(unsigned char keycode) {
   // keydown
   ps2sim_send(keycode);
   // vumeter
   led(LED_OFF);
   delay_us(300);

}

void keyup(unsigned char keycode) {
   // keyup
   ps2sim_send(0xF0);
   delay_us(200);
   ps2sim_send(keycode);
   // vumeter
   delay_us(300);
   led(LED_GREEN);
}

void keypress(unsigned char keycode) {
   keydown(keycode);
   //delay_us(300);
   delay_ms(50);
   keyup(keycode);
}

// polling input
void poll(void) {
     // gather input
     state = ~PORTB;
     // notemode is lower four bits :
     //    notemode & 0x07 == note id
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
           lasttrig = notemaps[notemap][notemode];
           keydown(lasttrig);
        }
     } else {
       // keydown the one we trig'd last
       if (lasttrig) { keyup(lasttrig); }
       // reset lasttrig
       lasttrig = 0;
     }
     
     // gather input
     state = PORTC;
     // update select mode
     selectmode = (~state & 0x30) >> 4;
     // rotative encoder bit update
     oldselectbit = selectbit;
     selectbit = state & 0x80;
     // update select&start
     oldpulsebit = pulsebit;
     oldtablebit = tablebit;
     pulsebit = ~state & 0x02;
     tablebit = ~state & 0x03;
         
     // check instr status
     if (!selectbit && oldselectbit) {
         // send select key
         switch (selectmode) {
               case 0x00:
                    keypress(state & 0x40 ? NC_INSTRUP : NC_INSTRDOWN);
                    break;
               case 0x01:
                    keypress(state & 0x40 ? NC_OCTAVEUP : NC_OCTAVEDOWN);
                    break;
               case 0x02:
                    keypress(state & 0x40 ? NC_TABLEUP : NC_TABLEDOWN);
                    break;
               case 0x03:
                    keypress(state & 0x40 ? NC_INSTRUP : NC_INSTRDOWN);
                    break;
        }
     }
     // check pulse toggle
     if (!pulsebit && oldpulsebit) {
        // press pulse toggle
        keypress(NC_PULSETOGGLE);
     }
     // check table bit
     if (!tablebit && oldtablebit) {
        // press table toggle
        keypress(NC_TABLERUN);
     }
     
     // gather input
     state = PORTD;
     // update notemod incr bit
     oldnoteincrbit = noteincrbit;
     noteincrbit = state & 0x20;
     // check if we should incr the mode
     if (!noteincrbit && oldnoteincrbit) {
        // we incr the mode
        notemap = (notemap + 1) % NOTEMAP_ALT_COUNT;
        // display
        led(LED_OFF);
        delay_ms(750);
        led(LED_GREEN);
     }
     
     // allow PS/2 programmation
     /*
     if (Ps2_Key_Read(&keydata, &special, &down)) {
        // if we pressed enter
        if (!down && keydata == 13) {
           pulsebit = 0, tablebit = 0;

           // enter prog mode
           // blink green led
           led(LED_GREEN);
           delay_ms(200);
           led(LED_OFF);
           delay_ms(200);
           led(LED_GREEN);
           delay_ms(200);
           led(LED_OFF);

           // wait for function press and release
           while(1) {
               if (Ps2_Key_Read(&keydata, &special, &down)) {
                 if (!down && keydata && keydata <= 12) {
                     break;
                 }
                 // allow escape by waiting for release
                 if (pulsebit || tablebit || (!down && keydata == 34)) { goto escape; }
               }
           }

           //set notemode
           notemode = (keydata - 1) % 3;
           
           // blink
           led(LED_GREEN);
           delay_ms(1000);
           led(LED_OFF);
           
           // start programming
           down = 0, keydata = 0;
           for (i = 0 ; i < 0x10 ; i++) {
               // wait for function press and release
               down = 0, keydata = 0;
               while(1) {
                   if (Ps2_Key_Read(&keydata, &special, &down)) {
                       // allow escape by waiting for release
                       if (pulsebit || tablebit || (!down && keydata == 34)) { goto escape; }
                       // wait key
                       if (!down && keydata && !special) {
                           break;
                       }
                   }
               }
               
               // save keycode
               j = (i >> 1) + (8 * (i & 1));
               notemaps[notemap][j] = keydata;
               // save in memory
               EEPROM_Write(notemap * 0x10 + j, keydata);

               // blink
               led(LED_GREEN);
               delay_ms(200);
               led(LED_OFF);
           }
           
           // blink
           led(LED_GREEN);
           delay_ms(550);
           
           // escape
           escape:;
           
           // end
           led(LED_RED);
           delay_ms(1000);
           led(LED_OFF);
        }
     } */
     
     // wait
     delay_us(300);
}

// load modes
void memload(void) {
     // gather modes
     for (k = 0 ; k < 3 ; k++) {
         for (j = 0; j < 0x10; j++) {
           // fill the actual notemap
           notemaps[k][j] = EEPROM_Read((k * 0x10) + j);
           // wait
           delay_ms(40);
         }
     }

}

// entry point
void main(void) {
     // all input digital
     ADCON1 = 0x0F;
     CCP1CON = 0x0F;
     CMCON = 0x07;
     CVRCON = 0;
     SPPCON = 0;
     UCON = 0;
     UCFG = 0x08;
     // I/O configuration
     TRISA = 0xFF;
     TRISB = 0xFF;
     TRISC = 0xFF;
     TRISD = 0xF0;
     TRISE = 0;
     
     // waiting for loading
     led(LED_RED);
     
     // load modes from eeprom
     memload();
     // init PS/2 simulation
     ps2sim_init(&PORTD, 1, &PORTD, 0);
     // init programmable ps/2 input
     //Ps2_Config()

     // wait for 2 sec
     delay_ms(2000);
     // set green
     led(LED_GREEN);
     
     // main loop
     while(1) { poll(); }
}