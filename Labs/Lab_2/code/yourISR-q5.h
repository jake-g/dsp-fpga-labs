/* HW2 QUESTION 5
 * Jake and Jisoo
 */

/*
 * yourISR.h
 *
 *  Created on: Mar 11, 2014
 *      Author: zhwj814
 *  Updated on : Mar 31, 2015 by jsup517
 */

#ifndef YOURISR_H_
#define YOURISR_H_


#endif /* YOURISR_H_ */
#include "system_init.h"

//Value for interrupt ID
extern alt_u32 switch0_id;
extern alt_u32 switch1_id;
extern alt_u32 key0_id;
extern alt_u32 key1_id;
extern alt_u32 key2_id;
extern alt_u32 key3_id;
extern alt_u32 leftready_id;
extern alt_u32 rightready_id;
extern alt_u32 uart_id;

//leftChannel and rightChannel are the instant values of the value reading from ADC
extern int leftChannel;
extern int rightChannel;
extern int convResultBufferSize;
extern int convIndex;

//Default ADC Sampling frequency = 8k
extern int sampleFrequency;

//Buffer that store values from different channels
extern alt_16 leftChannelData[BUFFERSIZE];
extern alt_16 rightChannelData[BUFFERSIZE];
extern int convResultBuffer[CONVBUFFSIZE];
extern alt_16 datatest[256];
/*uart-global
 * RxHead: integer indicator tells you the index of where the
 * newest char data you received from host computer
 *
 * rx_buffer-> A ring buffer to collect uart data sent by host computer
 * */
extern unsigned short RxHead;
extern unsigned char rx_buffer[RX_BUFFER_SIZE];

/*Channel indicators: indicate the index of the most recent collected data*/
extern int leftCount;
extern int lefttestCount;
extern int rightCount;
extern int calBuffersize;
extern int leftBufferFull;

/*temporary use*/
extern int tempCount;

/*Send flag: 1->ok, now send data to the host
 *           0->ok, will not send any data to the host
 *Recv flag: 1->ok, now check and store data from the host
 *           0->ok, will not receive any data from the host
 */
extern int uartStartSendFlag;
extern int uartStartRecvFlag;
/*for uart receive purpose*/
extern int sr;

/*0->do not update sampling frequency*/
/*1->ok, update sampling frequency to AIC23*/
extern int setFreqFlag;

/*uart object*/
extern int uart;

//Stereo SWITCH
int stereo_on = 1;
static void handle_switch0_interrupt(void* context, alt_u32 id) {
	 volatile int* switch0ptr = (volatile int *)context;
	 *switch0ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH0_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH0_BASE, 0);

	 // Set mute state
	 stereo_on = IORD_ALTERA_AVALON_PIO_DATA(SWITCH0_BASE);
	 printf("Stereo ON: %d\n", stereo_on);
}

static void handle_switch1_interrupt(void* context, alt_u32 id) {
	 volatile int* switch1ptr = (volatile int *)context;
	 *switch1ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH1_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH1_BASE, 0);

}

/* Enable the flag to send recent
 * channel buffer to host computer.
 */
static void handle_key0_interrupt(void* context, alt_u32 id) {
	 volatile int* key0ptr = (volatile int *)context;
	 *key0ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE, 0);

	 uartStartSendFlag = 1;
}

/* Enable the flag to update the
 * ADC sampling frequency on AIC23.
 */
static void handle_key1_interrupt(void* context, alt_u32 id) {
	 volatile int* key1ptr = (volatile int *)context;
	 *key1ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY1_BASE, 0);

}

static void handle_key2_interrupt(void* context, alt_u32 id) {
	 volatile int* key2ptr = (volatile int *)context;
	 *key2ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY2_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY2_BASE, 0);

}

static void handle_key3_interrupt(void* context, alt_u32 id) {
	 volatile int* key3ptr = (volatile int *)context;
	 *key3ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY3_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY3_BASE, 0);

	 IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, 0x10);
}


int unsigned2signed(alt_16 unsign){
	int result;
	if(unsign>32767)
		result = unsign - 65536;
	else
		result = unsign;
	return result;
}

alt_16 signed2unsigned(int sign){
	alt_16 result;
	if(sign<0)
		result = sign + 65536;
	else
		result = sign;
	return result;
}


/*
 * Stereo Mix Logic
 * Left when stereo_on = 0
 * Right + Left when stereo_on = 1
 */
static void handle_leftready_interrupt_test(void* context, alt_u32 id) {
	 volatile int* leftreadyptr = (volatile int *)context;
	 *leftreadyptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(LEFTREADY_BASE);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(LEFTREADY_BASE, 0);
	 /*******Read, playback, store data*******/
	 leftChannel = IORD_ALTERA_AVALON_PIO_DATA(LEFTDATA_BASE);
	 IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE, (leftChannel + stereo_on * rightChannel));
	 leftCount = (leftCount+1)%256;
//	 /****************************************/

}

/*
 * Stereo Mix Logic
 * Right when stereo_on = 0
 * Right - Left when stereo_on = 1
 */
static void handle_rightready_interrupt_test(void* context, alt_u32 id) {
	 volatile int* rightreadyptr = (volatile int *)context;
	 *rightreadyptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(RIGHTREADY_BASE);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(RIGHTREADY_BASE, 0);
	 /*******Read, playback, store data*******/
	 rightChannel = IORD_ALTERA_AVALON_PIO_DATA(RIGHTDATA_BASE);
	 IOWR_ALTERA_AVALON_PIO_DATA(RIGHTSENDDATA_BASE, (rightChannel - stereo_on * leftChannel));
	 rightCount = (rightCount+1) % BUFFERSIZE;
	 /****************************************/
}
