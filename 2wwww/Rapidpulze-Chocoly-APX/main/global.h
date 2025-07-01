/*
 * messagetype.h
 *
 *  Created on: 2024年2月6日
 *      Author: mastwet
 */

#ifndef MAIN_GLOBAL_H_
#define MAIN_GLOBAL_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


typedef struct {
  int type;
  int name;
  int value;
} message_t;

//typedef struct{
//	int key_sta;
//	int key_sta_pre;
//	uint64_t key_pressed_time;
//} ;

//type 0:Note
//type 1:ControlChange

typedef struct {
  char type;
  char note_pitch;
  uint8_t note_state;
} midi_message_t;

enum RunMode{
	MIDI,
	MENU
};



//extern uint64_t timer_count;





extern QueueHandle_t timtrace_queue;


#endif /* MAIN_GLOBAL_H_ */
