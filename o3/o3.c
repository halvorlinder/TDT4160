#include "o3.h"
#include "gpio.h"
#include "systick.h"

/**************************************************************************//**
 * @brief Kpaaverterer nummer til string
 * Kpaaverterer et nummer mellom 0 og 99 til string
 *****************************************************************************/
void int_to_string(char *timestamp, unsigned int offset, int i) {
    if (i > 99) {
        timestamp[offset]   = '9';
        timestamp[offset+1] = '9';
        return;
    }

    while (i > 0) {
	    if (i >= 10) {
		    i -= 10;
		    timestamp[offset]++;
		
	    } else {
		    timestamp[offset+1] = '0' + i;
		    i=0;
	    }
    }
}

/**************************************************************************//**
 * @brief Kpaaverterer 3 tall til en timestamp-string
 * timestamp-argumentet må være et array med plass til (minst) 7 elementer.
 * Det kan deklareres i funksjpaaen som kaller som "char timestamp[7];"
 * Kallet blir dermed:
 * char timestamp[7];
 * time_to_string(timestamp, h, m, s);
 *****************************************************************************/
void time_to_string(char *timestamp, int h, int m, int s) {
    timestamp[0] = '0';
    timestamp[1] = '0';
    timestamp[2] = '0';
    timestamp[3] = '0';
    timestamp[4] = '0';
    timestamp[5] = '0';
    timestamp[6] = '\0';

    int_to_string(timestamp, 0, h);
    int_to_string(timestamp, 2, m);
    int_to_string(timestamp, 4, s);
}

volatile gpio_map_t* gpio = (gpio_map_t*) GPIO_BASE;
volatile systick_map_t* systick = (systick_map_t*) SYSTICK_BASE;

//Initial

int status;
int timer;
int minutes;
int sekunder;




void LED0(int paa) {
	if (paa == 1) {
		gpio->ports[GPIO_PORT_E].DOUTSET = GPIO_MODE_OUTPUT;
	}
	else {
		gpio->ports[GPIO_PORT_E].DOUTCLR = GPIO_MODE_OUTPUT;
	}
}


void GPIO_EVEN_IRQHandler(void) {
	switch (status) {
		case 1:
			status ++;
			break;
		case 2:
			status ++;
			break;
		case 3:
			status ++;
			systick->CTRL = 0b0111;
			break;
		case 4:
			status = 1;
			systick->CTRL = 0b0110;
			break;
		case 5:
			status = 1;
			LED0(0);
			systick->CTRL = 0b0110;
	}
	gpio->IFC = 0b0001 << 10;
}

void SysTick_Handler(void) {
	if (status == 4) {
		if (sekunder!=0) {
				sekunder --;
		}
		else {
			if (minutes!=0) {
				minutes --;
				sekunder = 59;
			}
			else {
				if (timer!=0) {
					timer --;
					minutes = 59;
					sekunder = 59;
				}
				else {
					status = 5;
					LED0(1);
				}
			}
		}
	}
}

void GPIO_ODD_IRQHandler(void){
	switch (status) {
		case 1:
			sekunder ++;
			break;
		case 2:
			minutes ++;
			break;
		case 3:
			timer ++;
			break;
	}
	gpio->IFC = 0b0001 << 9;
}







int main(void) {
    init();

    //initialverdier

    timer = 0;
    status = 1;
    minutes = 0;
    sekunder = 0;


    //tid
    char timestamp[7];

    time_to_string(timestamp, timer, minutes, sekunder);

    //SYSTICK
    systick->CTRL = 0b0110;
    systick->LOAD = FREQUENCY;
    systick->VAL = 0;

    //GPIO SET-UP
    gpio->ports[GPIO_PORT_B].DOUT = 0;
    gpio->ports[GPIO_PORT_B].MODEH = ((~(0b1111<<4) & (gpio->ports[GPIO_PORT_B].MODEH)) | (0b0001 << 4));
    gpio->ports[GPIO_PORT_B].MODEH = ((~(0b1111<<8) & (gpio->ports[GPIO_PORT_B].MODEH)) | (0b0001 << 8));
    //mask = ((~(0b1111<<4) & (gpio->ports[GPIO_PORT_B].MODEH)) | (0b0001 << 4));

    gpio->EXTIPSELH = ((~(0b1111<<4) & (gpio->EXTIPSELH)) | (0b0001 << 4));
    gpio->EXTIPSELH = ((~(0b1111<<8) & (gpio->EXTIPSELH)) | (0b0001 << 8));
    gpio->EXTIFALL = (0b0011 << 9);
    gpio->IFC = 0b0011 << 9;
    gpio->IEN = 0b0011 << 9;



    //Setup
    gpio->ports[GPIO_PORT_E].DOUT = 0;

    gpio->ports[GPIO_PORT_E].MODEL = ((~(0b1111<<8) & (gpio->ports[GPIO_PORT_B].MODEH)) | (0b0100 << 8));




    while (1) {
    	time_to_string(timestamp, timer, minutes, sekunder);
    	lcd_write(timestamp);
    }

    return 0;
}



