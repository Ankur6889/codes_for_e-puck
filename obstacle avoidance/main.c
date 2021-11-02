#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>
#include "leds.h"
#include "spi_comm.h"
#include "sensors/proximity.h"
#include "epuck1x/uart/e_uart_char.h"
#include "stdio.h"
#include "serial_comm.h"
#include "leds.h"
#include "spi_comm.h"
#include "motors.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

int speed=400;

void turn_left(void)
{
	left_motor_set_speed(-1*speed);
	right_motor_set_speed(speed);
}

void turn_right(void)
{
	left_motor_set_speed(speed);
	right_motor_set_speed(-1*speed);
}

void move_forward(void)
{
	left_motor_set_speed(speed);
	right_motor_set_speed(speed);
}

void move_backward(void)
{
	left_motor_set_speed(-1*speed)
	right_motor_set_speed(-1*speed)
}

void stop(void)
{
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}



void blue(int num)
{
	char str[100];
	int str_length;
	int val=get_calibrated_prox(num);
	str_length=sprintf(str,"Reading from Sensor %d is %d\n:",num,val);
	e_send_uart1_char(str,str_length);
}


int main(void)
{

    halInit();
    chSysInit();
    mpu_init();
    messagebus_init(&bus,&bus_lock,&bus_condvar);
    clear_leds();
    spi_comm_start();
    serial_start();
    proximity_start();
    calibrate_ir();
    motors_init();
    
    int sensor1_cal=300;
    int sensor2_cal=400;
    int sensor3_cal=800;
    int sensor4_cal=800;
    int sensor5_cal=800;
    int sensor6_cal=800;
    int sensor7_cal=400;
    int sensor8_cal=300;


    /* Infinite loop. */
    while (1) {
    	//waits 1 second

    	/**/
    	int sensor1=get_calibrated_prox(0);
    	int sensor2=get_calibrated_prox(1);
    	int sensor3=get_calibrated_prox(2);
    	int sensor4=get_calibrated_prox(3);
    	int sensor5=get_calibrated_prox(4);
    	int sensor6=get_calibrated_prox(5);
    	int sensor7=get_calibrated_prox(6);
    	int sensor8=get_calibrated_prox(7);

    	if (sensor1>sensor1_cal || sensor8>sensor8_cal )
    	{
    		turn_right();
    		blue(0);
		blue(7);
		chThdSleepMilliseconds(100);

    	}
    	else if (sensor6>sensor6_cal)
    	{
    		turn_right();
    		blue(5);
    		chThdSleepMilliseconds(100);

    	}
    	else if (sensor7>sensor7_cal)
    	{
    	    turn_right();
    	    blue(6);
    	    chThdSleepMilliseconds(100);

    	}
    	else if (sensor2>sensor2_cal)
    	{
    	    turn_left();
	    blue(1);
    	    chThdSleepMilliseconds(100);

    	}
    	else if (sensor3>sensor3_cal)
    	{
    	    turn_left();
	    blue(2);
    	    chThdSleepMilliseconds(500);

    	}
    	else
    	{
    	    move_forward();
    	    chThdSleepMilliseconds(500);
    	}





    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
