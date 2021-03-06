# c h i p g u i t a r ## presentation** insert photo **The chipguitar is a controller for [LSDJ](http://www.littlesounddj.com/lsd/) built from a [Konami PS1/2 GuitarFreaks controller](https://fr.wikipedia.org/wiki/GuitarFreaks).It emulates a PS/2 keyboard and syncs itself with LSDJ.The guitar can play a total of 2x8 notes in a given mode, with a total of four modes being available. A push button on the front of the guitar cycles through modes when pressed. These modes can be reprogrammed using any PS/2 keyboard.## firmwareThe firmware is to be compiled with [MikroC](http://www.mikroe.com/mikroc/pic/) compiler.Code's comments explain how the program is built and work.The code includes a PS/2 Keyboard to Host library, also available on GitHub as [iLambda/ps2simulator](https://github.com/iLambda/ps2simulator).The entry point is contained in *main.c*.*keycodes.h* and *notemap.h* contain additional preprocessor definitions and macros.## schematics 
The board schematics are supplied in the schematics/ folder as EAGLE files (\*.sch and \*.brd).
 
## component list

### motherboard

* IC1 : [PIC18F4550](www.microchip.com/PIC18F4550) with a 44-pin DIP socket.
* C1, C2 : ceramic 33 pF
* X1 : 24 MHz crystal resonator
* R1, R2, R3, R4 : 1MΩ
* R5, R6 : 1KΩ
* R7, R8 : 1KΩ
* R9, R10, R11 : 1KΩ
* R12, R13, R14, R15 : 1KΩ

### alim

* L1 : LM7805 Voltage regulator
* C1, C2  : ceramic 100 nF

 
