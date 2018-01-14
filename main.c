
//Rainer Laaksonen & Aleksi Lahti
/* XDCtools Header files */

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
 
/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
 
/* Board Header files */
#include "Board.h"
#include <ti/drivers/i2c/I2CCC26XX.h>
 
/* jtkj Header files */
#include "wireless/comm_lib.h"
#include "sensors/mpu9250.h"
 
/* Task Stacks */
#define STACKSIZE 3072
Char gametaskStack[STACKSIZE];
Char commTaskStack[STACKSIZE];
Char taskStack[STACKSIZE];
 
/* Display */
Display_Handle hDisplay;
 
 /*global variables*/
static int config=0;
static char str[20];
static int tulos=0;
static int car=0;
static int jump=0;
static int game=0;
static int i=1;
static int k=0;
static int j=0;
static uint8_t rata;
static float ax, ay, az, gx, gy, gz;
static int axx=0;
static int azz=0;
 
/*Gyro config*/
static I2C_Handle i2cMPU;
static I2C_Params i2cMPUParams;
 
 
static PIN_Handle hMpuPin;
static PIN_State MpuPinState;
static PIN_Config MpuPinConfig[] = {
    Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};
// MPU9250 uses its own I2C interface
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
    .pinSDA = Board_I2C0_SDA1,
    .pinSCL = Board_I2C0_SCL1
};
 
/* Pin Button1 configured as power button */
static PIN_Handle hPowerButton;
static PIN_State sPowerButton;
PIN_Config cPowerButton[] = {
    Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};
PIN_Config cPowerWake[] = {
    Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PINCC26XX_WAKEUP_NEGEDGE,
    PIN_TERMINATE
};
 
/* Pin button0 configured as input */
static PIN_Handle hbutton0;
static PIN_State sbutton0;
PIN_Config cbutton0[] = {
    Board_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};
 
/* Handle power button */
Void powerButtonFxn(PIN_Handle handle, PIN_Id pinId) {
 k++;
}
 
 
/* Handle button0 */
Void button0Fxn(PIN_Handle handle, PIN_Id pinId){
    if(i<4){i++;}
    else{i=1;}
}
 
 
Void commTask(UArg arg0, UArg arg1) {
   char payload[16]; // viestipuskuri
   uint16_t senderAddr=4660;
 
   // Radio alustetaan vastaanottotilaan
   int32_t result = StartReceive6LoWPAN();
   if(result != true) {
      System_abort("Wireless receive start failed");
   }
 
   // Vastaanotetaan viestejä loopissa
   while (true) {
        // jos true, viesti odottaa
        if (GetRXFlag() == true) {
 
             // Tyhjennetään puskuri
            memset(payload,0,16);
            // Luetaan viesti puskuriin payload
           Receive6LoWPAN(&senderAddr, payload, sizeof(payload));
           // Tulostetaan vastaanotettu viesti
           
           rata=(uint8_t)payload[0];
            
            System_flush();
           Display_print0(hDisplay, 11, 1, payload);
           if(rata<=0xFF){
           j++;
           }
     
}
    // Lähetetään serverille viesti jos nappia on painettu
       if((k==1)&&(game==1)){
           sprintf(payload,"Git Gud!", IEEE80154_MY_ADDR);
           Send6LoWPAN(IEEE80154_SERVER_ADDR, payload, strlen(payload));
           k=0;
           System_printf("\nMessage sent!\n");
        // Lähetyksen jälkeen taas vastaanottotilaan
        StartReceive6LoWPAN();
       }
}}
int play_game(){
    System_printf("playing game\n");
    int loppu=0;
    int autonpaikka;
    int x=0;
    int z=j-1;
    char c,d,e,f;
    uint8_t mask01=0x01,mask02=0x02,mask04=0x04,mask08=0x08,mask10=0x10,mask20=0x20,mask40=0x40,mask80=0x80;
    char reuna1[4],tie1[6],tie2[6],reuna2[4],track0[20]="  ||   |   ||  ",track1[20]="  ||   |   ||  ",track2[20]="  ||   |   ||  ",track3[20]="  ||   |   ||  ",track4[20]="  ||   |   ||  ",track5[20]="  ||   |   ||  ",track6[20]="  ||   |   ||  ",track7[20]="  ||   |   ||  ",track8[20]="  ||   |   ||  ",track9[20]="  ||   |   ||  ";
    game=1;
    
    while(1){
    if(j>z){
    if(x>9){
        x=0;
    }
    z++;
   
    /*Radan vasen reuna*/
    if((rata&mask40)==mask40){
       
        if((rata&mask80)==mask80){
            sprintf(reuna1,"N$");}
        else{
            sprintf(reuna1,"N ");}
    }
    else{
       
        if((rata&mask80)==mask80){
            sprintf(reuna1," $");}
        else{
            sprintf(reuna1,"  ");}
    }
   
    /*Radan oikea reuna*/
    if((rata&mask02)==mask02){
       
        if((rata&mask01)==mask01){
            sprintf(reuna2,"$N");}
        else{
            sprintf(reuna2," N");}
    }
    else{
       
        if((rata&mask01)==mask01){
            sprintf(reuna2,"$ ");}
        else{
            sprintf(reuna2,"  ");}
    }
   
    /*Rata vasen*/
    if((rata&mask10)==mask10){
       
        if((rata&mask20)==mask20){
            sprintf(tie1,"XNX");}
        else{
            sprintf(tie1,"XXX");}
    }
    else{
       
        if((rata&mask20)==mask20){
            sprintf(tie1,"NNN");}
        else{
            sprintf(tie1,"   ");}
    }
    /*Rata oikea*/
    if((rata&mask08)==mask08){
       
        if((rata&mask04)==mask04){
            sprintf(tie2,"XNX");}
        else{
            sprintf(tie2,"XXX");}
    }
    else{
       
        if((rata&mask04)==mask04){
            sprintf(tie2,"NNN");}
        else{
            sprintf(tie2,"   ");}
    }
    System_printf("%d",x);
    System_flush();
    if((i==1)|(i==3)){
             car=1;
            }
            else{
                car=0;
            }
    if(az>-0.8){
        jump=1;
    }
    else{
        jump=0;
    }
    switch(x){
        case 0:
            sprintf(track0,"%2s||%3s|%3s||%2s\n", reuna1, tie1, tie2, reuna2);
           
            Display_print0(hDisplay, 0, 1, track0);
            Display_print0(hDisplay, 1, 1, track1);
            Display_print0(hDisplay, 2, 1, track2);
            Display_print0(hDisplay, 3, 1, track3);
            Display_print0(hDisplay, 4, 1, track4);
            Display_print0(hDisplay, 5, 1, track5);
            Display_print0(hDisplay, 6, 1, track6);
            Display_print0(hDisplay, 7, 1, track7);
            Display_print0(hDisplay, 8, 1, track8);
            Display_print0(hDisplay, 9, 1, track9);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track9[9];
            d=track9[5];
            e=track9[1];
            f=track9[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            
            break;
        case 1:
            sprintf(track9,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track9);
            Display_print0(hDisplay, 1, 1, track0);
            Display_print0(hDisplay, 2, 1, track1);
            Display_print0(hDisplay, 3, 1, track2);
            Display_print0(hDisplay, 4, 1, track3);
            Display_print0(hDisplay, 5, 1, track4);
            Display_print0(hDisplay, 6, 1, track5);
            Display_print0(hDisplay, 7, 1, track6);
            Display_print0(hDisplay, 8, 1, track7);
            Display_print0(hDisplay, 9, 1, track8);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track8[9];
            d=track8[5];
            e=track8[1];
            f=track8[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
        case 2:
            sprintf(track8,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track8);
            Display_print0(hDisplay, 1, 1, track9);
            Display_print0(hDisplay, 2, 1, track0);
            Display_print0(hDisplay, 3, 1, track1);
            Display_print0(hDisplay, 4, 1, track2);
            Display_print0(hDisplay, 5, 1, track3);
            Display_print0(hDisplay, 6, 1, track4);
            Display_print0(hDisplay, 7, 1, track5);
            Display_print0(hDisplay, 8, 1, track6);
            Display_print0(hDisplay, 9, 1, track7);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track7[9];
            d=track7[5];
            e=track7[1];
            f=track7[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
        case 3:
            sprintf(track7,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track7);
            Display_print0(hDisplay, 1, 1, track8);
            Display_print0(hDisplay, 2, 1, track9);
            Display_print0(hDisplay, 3, 1, track0);
            Display_print0(hDisplay, 4, 1, track1);
            Display_print0(hDisplay, 5, 1, track2);
            Display_print0(hDisplay, 6, 1, track3);
            Display_print0(hDisplay, 7, 1, track4);
            Display_print0(hDisplay, 8, 1, track5);
            Display_print0(hDisplay, 9, 1, track6);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track6[9];
            d=track6[5];
            e=track6[1];
            f=track6[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
        case 4:
            sprintf(track6,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track6);
            Display_print0(hDisplay, 1, 1, track7);
            Display_print0(hDisplay, 2, 1, track8);
            Display_print0(hDisplay, 3, 1, track9);
            Display_print0(hDisplay, 4, 1, track0);
            Display_print0(hDisplay, 5, 1, track1);
            Display_print0(hDisplay, 6, 1, track2);
            Display_print0(hDisplay, 7, 1, track3);
            Display_print0(hDisplay, 8, 1, track4);
            Display_print0(hDisplay, 9, 1, track5);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track5[9];
            d=track5[5];
            e=track5[1];
            f=track5[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
        case 5:
            sprintf(track5,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track5);
            Display_print0(hDisplay, 1, 1, track6);
            Display_print0(hDisplay, 2, 1, track7);
            Display_print0(hDisplay, 3, 1, track8);
            Display_print0(hDisplay, 4, 1, track9);
            Display_print0(hDisplay, 5, 1, track0);
            Display_print0(hDisplay, 6, 1, track1);
            Display_print0(hDisplay, 7, 1, track2);
            Display_print0(hDisplay, 8, 1, track3);
            Display_print0(hDisplay, 9, 1, track4);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track4[9];
            d=track4[5];
            e=track4[1];
            f=track4[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
        case 6:
            sprintf(track4,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track4);
            Display_print0(hDisplay, 1, 1, track5);
            Display_print0(hDisplay, 2, 1, track6);
            Display_print0(hDisplay, 3, 1, track7);
            Display_print0(hDisplay, 4, 1, track8);
            Display_print0(hDisplay, 5, 1, track9);
            Display_print0(hDisplay, 6, 1, track0);
            Display_print0(hDisplay, 7, 1, track1);
            Display_print0(hDisplay, 8, 1, track2);
            Display_print0(hDisplay, 9, 1, track3);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track3[9];
            d=track3[5];
            e=track3[1];
            f=track3[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
        case 7:
            sprintf(track3,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track3);
            Display_print0(hDisplay, 1, 1, track4);
            Display_print0(hDisplay, 2, 1, track5);
            Display_print0(hDisplay, 3, 1, track6);
            Display_print0(hDisplay, 4, 1, track7);
            Display_print0(hDisplay, 5, 1, track8);
            Display_print0(hDisplay, 6, 1, track9);
            Display_print0(hDisplay, 7, 1, track0);
            Display_print0(hDisplay, 8, 1, track1);
            Display_print0(hDisplay, 9, 1, track2);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track2[9];
            d=track2[5];
            e=track2[1];
            f=track2[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
        case 8:
            sprintf(track2,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track2);
            Display_print0(hDisplay, 1, 1, track3);
            Display_print0(hDisplay, 2, 1, track4);
            Display_print0(hDisplay, 3, 1, track5);
            Display_print0(hDisplay, 4, 1, track6);
            Display_print0(hDisplay, 5, 1, track7);
            Display_print0(hDisplay, 6, 1, track8);
            Display_print0(hDisplay, 7, 1, track9);
            Display_print0(hDisplay, 8, 1, track0);
            Display_print0(hDisplay, 9, 1, track1);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track1[9];
            d=track1[5];
            e=track1[1];
            f=track1[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
        case 9:
            sprintf(track1,"%s||%s|%s||%s\n", reuna1, tie1, tie2, reuna2);
            Display_print0(hDisplay, 0, 1, track1);
            Display_print0(hDisplay, 1, 1, track2);
            Display_print0(hDisplay, 2, 1, track3);
            Display_print0(hDisplay, 3, 1, track4);
            Display_print0(hDisplay, 4, 1, track5);
            Display_print0(hDisplay, 5, 1, track6);
            Display_print0(hDisplay, 6, 1, track7);
            Display_print0(hDisplay, 7, 1, track8);
            Display_print0(hDisplay, 8, 1, track9);
            Display_print0(hDisplay, 9, 1, track0);
            if((car==0)&&(jump==0)){
                Display_print0(hDisplay, 10, 9, "CAR\0");
                autonpaikka=2;
            }
            else if((car==1)&&(jump==0)){
                Display_print0(hDisplay, 10, 5, "CAR\0");
                autonpaikka=1;
            }
            else if((car==0)&&(jump==1)){
                Display_print0(hDisplay, 10, 14, "CAR\0");
                autonpaikka=0;
            }
            else if((car==1)&&(jump==1)){
                Display_print0(hDisplay, 10, 0, "CAR\0");
                autonpaikka=4;
            }
            else{
                Display_print0(hDisplay, 10, 5, "NoCAR\0");
            }
            c=track0[9];
            d=track0[5];
            e=track0[1];
            f=track0[13];
            if((autonpaikka==2)&&(c!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==1)&&(d!=' ')){
                loppu=1;
                game=0;
            }
            else if((autonpaikka==0)&&(e!=' ')){
                tulos=(tulos+50);
            }
            else if((autonpaikka==4)&&(f!=' ')){
                tulos=(tulos+50);
            }
            else{
                tulos++;
            }
            break;
           
       
        default:
            break;
       
    }
    x++;
    if(loppu==1){
        return(0);
        break;
    }
    }
    
    Task_sleep(100000 / Clock_tickPeriod);
    }
}
 
Void gametask(UArg arg0, UArg arg1){
   
    int y=(i-1);
    int tulokset[6] = {0};
 
    /* Init Display */
    Display_Params displayParams;
    displayParams.lineClearMode = DISPLAY_CLEAR_BOTH;
    Display_Params_init(&displayParams);
 
    hDisplay = Display_open(Display_Type_LCD, &displayParams);
    if (hDisplay == NULL) {
        System_abort("Error initializing Display\n");
    }
    Display_clear(hDisplay);
    Display_print0(hDisplay, 2, 1, "GENERIC");
    Display_print0(hDisplay, 3, 1, "CAR");
    Display_print0(hDisplay, 4, 1, "GAME");
    Display_print0(hDisplay, 8, 1, "IGN 10/10");
    Display_print0(hDisplay, 9, 1, "GameOfTheYear");
    Display_print0(hDisplay, 10, 1, "MASTERPIECE!");
   
    Task_sleep(3000000 / Clock_tickPeriod);
    Display_clear(hDisplay);
   
    // main loop
    while (1) {
   
    if(i==(y+1)){
        y=i;
        switch(i){
            case 1:
                Display_print0(hDisplay, 0, 12, "move");
                Display_print0(hDisplay, 2, 1, "*Play*");
                Display_print0(hDisplay, 4, 1, " Score");
                Display_print0(hDisplay, 6, 1, " Calibrate");
                Display_print0(hDisplay, 8, 1, " Exit");
                Display_print0(hDisplay, 10, 10, "select");
                break;
            case 2:
                Display_print0(hDisplay, 0, 12, "move");
                Display_print0(hDisplay, 2, 1, " Play");
                Display_print0(hDisplay, 4, 1, "*Score*");
                Display_print0(hDisplay, 6, 1, " Calibrate");
                Display_print0(hDisplay, 8, 1, " Exit");
                Display_print0(hDisplay, 10, 10, "select");
                break;
            case 3:
                Display_print0(hDisplay, 0, 12, "move");
                Display_print0(hDisplay, 2, 1, " Play");
                Display_print0(hDisplay, 4, 1, " Score");
                Display_print0(hDisplay, 6, 1, "*Calibrate*");
                Display_print0(hDisplay, 8, 1, " Exit");
                Display_print0(hDisplay, 10, 10, "select");
                break;
            case 4:
                Display_print0(hDisplay, 0, 12, "move");
                Display_print0(hDisplay, 2, 1, " Play");
                Display_print0(hDisplay, 4, 1, " Score");
                Display_print0(hDisplay, 6, 1, " Calibrate");
                Display_print0(hDisplay, 8, 1, "*Exit*");
                Display_print0(hDisplay, 10, 10, "select");
                y=0;
                break;
            default:
                break;
        }}
        if(k==1){
            switch(i){
            case 1:
                Display_clear(hDisplay);
                play_game();
                Display_clear(hDisplay);
                Display_print0(hDisplay, 0, 12, "menu");
                Display_print0(hDisplay, 2, 3, "Game over");
                Display_print0(hDisplay, 6, 5, "Score");
                sprintf(str,"%d",tulos);
                //sijoittaa tuloksen tuloksiin
                tulokset[4]=tulos;
                tulos=0;
                Display_print0(hDisplay, 8, 6, str);
                break;
            case 2:
                //järjestää tulokset
                //qsort(tulokset, 5, sizeof(int), cmpfunc);
                //tulostaa tulokset
                Display_clear(hDisplay);
                Display_print0(hDisplay, 0, 6, "TOP SCORES");
                Display_print0(hDisplay, 2, 6, tulokset[0]);
                Display_print0(hDisplay, 4, 6, tulokset[1]);
                Display_print0(hDisplay, 6, 6, tulokset[2]);
                Display_print0(hDisplay, 8, 6, tulokset[3]);
                Display_print0(hDisplay, 10, 6, tulokset[4]);
                break;
            case 3:
                Display_clear(hDisplay);
                Display_print0(hDisplay, 3, 1, "calibrating");
                Display_print0(hDisplay, 4, 1, "wait 5s");
                config=1;
                Task_sleep(5000000 / Clock_tickPeriod);
                Display_clear(hDisplay);
                break;
            case 4:
                k=0;
                Display_clear(hDisplay);
                Display_close(hDisplay);
                Task_sleep(100000 / Clock_tickPeriod);
                Display_print0(hDisplay, 3, 1, "Shutting down");
                PIN_close(hPowerButton);
             
                PINCC26XX_setWakeup(cPowerWake);
                Power_shutdown(NULL,0);
                PIN_close(hPowerButton);
                break;
            default:
                break;
        }
        k=0;
    }
   
        // Do not remove sleep-call from here!
        Task_sleep(100000 / Clock_tickPeriod);
    }}
 
// SENSOR TASK
Void sensorFxn(UArg arg0, UArg arg1) {

    // initiate_mpu
    I2C_Params_init(&i2cMPUParams);
    i2cMPUParams.bitRate = I2C_400kHz;
    i2cMPUParams.custom = (uintptr_t)&i2cMPUCfg;
    
    // MPU OPEN I2C
    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) {
        System_abort("Error Initializing I2CMPU\n");
    }
    
    // MPU POWER ON
     PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_ON);

    // WAIT 100MS FOR THE SENSOR TO POWER UP
	Task_sleep(100000 / Clock_tickPeriod);
    System_printf("MPU9250: Power ON\n");
    System_flush();
    
    mpu9250_setup(&i2cMPU);

    // LOOP FOREVER
    while (1){
        // MPU ASK DATA
        // Accelerometer values: ax,ay,az
        // Gyroscope values: gx,gy,gz
 
        mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
 
        // WAIT 100MS
        Task_sleep(100000 / Clock_tickPeriod);
    }
   
    // MPU9250 POWER OFF
    //     Because of loop forever, code never goes here
    PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_OFF);
    
}
 
int main(void){
   
    Task_Handle hgametask;
    Task_Params gametaskparams;
    Task_Handle hCommTask;
    Task_Params commTaskParams;
    Task_Handle task;
    Task_Params taskParams;
    
    // Initialize board
    Board_initGeneral();
    Board_initI2C();
 
    // OPEN MPU POWER PIN
 
    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL) {
        System_abort("Pin open failed!");
    }
 
    /* Power Button */
    hPowerButton = PIN_open(&sPowerButton, cPowerButton);
    if(!hPowerButton) {
        System_abort("Error initializing power button shut pins\n");
    }
    if (PIN_registerIntCb(hPowerButton, &powerButtonFxn) != 0) {
        System_abort("Error registering power button callback function");
    }
   
   
    /* Move Button(button0) */
    hbutton0 = PIN_open(&sbutton0, cbutton0);
    if(!hbutton0) {
        System_abort("Error initializing move button\n");
    }
    if (PIN_registerIntCb(hbutton0, &button0Fxn) != 0) {
        System_abort("Error registering move button callback function");
    }
   
   
    /* Init Main Task */
    Task_Params_init(&gametaskparams);
    gametaskparams.stackSize = STACKSIZE;
    gametaskparams.stack = &gametaskStack;
    gametaskparams.priority=3;
 
    hgametask = Task_create(gametask, &gametaskparams, NULL);
    if (hgametask == NULL) {
        System_abort("Task create failed!");
    }
    
    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = &taskStack;
    taskParams.priority=2;

    task = Task_create((Task_FuncPtr)sensorFxn, &taskParams, NULL);
    if (task == NULL) {
    	System_abort("Task create failed!");
    }
   
   
    /* Init Communication Task */
    Task_Params_init(&commTaskParams);
    commTaskParams.stackSize = STACKSIZE;
    commTaskParams.stack = &commTaskStack;
    commTaskParams.priority=1;
 
    Init6LoWPAN();
   
    hCommTask = Task_create(commTask, &commTaskParams, NULL);
    if (hCommTask == NULL) {
        System_abort("Task create failed!");
    }

    // Send OK to console
    System_printf("Hello world!\n");
    System_flush();
 
    /* Start BIOS */
    BIOS_start();
 
    return (0);
}