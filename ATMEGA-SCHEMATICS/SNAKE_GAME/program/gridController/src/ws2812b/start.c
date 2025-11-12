#include "start.h"


void gpioConfig(){
    //joyStickConf();
    //spiInitPoll();
}


void joyStickConf(){

}


void seed_prng(void){ srand(0xA5A5 ^ (uint16_t)TCNT0);}