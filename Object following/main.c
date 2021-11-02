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
#include "sensors/VL53L0X/VL53L0X.h"

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

int nearest_sensor(int a,int b,int c,int d,int e,int f,int g,int h)
{
	if (a>b && a>c && a>d && a>e && a>f && a>g && a>h)
	{
		return 1;
	}
	else if (b>a && b>c && b>d && b>e && b>f && b>g && b>h)
	{
		return 2;
	}
	else if (c>a && c>b && c>d && c>e && c>f && c>g && c>h)
	{
		return 3;
	}
	else if (d>a && d>b && d>c && d>e && d>f && d>g && d>h)
	{
		return 4;
	}
	else if (e>a && e>b && e>c && e>d && e>f && e>g && e>h)
	{
		return 5;
	}
	else if (f>a && f>b && f>c && f>d && f>e && f>g && f>h)
	{
		return 6;
	}
	else if (g>a && g>b && g>c && g>d && g>e && g>f && g>h)
	{
		return 7;
	}
	else if (h>a && h>b && h>c && h>d && h>e && h>f && h>g)
	{
		return 8;
	}
}
void bt_prox(int num)
{
	char str[100];
	int str_length;
	int val=get_calibrated_prox(num);
	str_length=sprintf(str,"Reading from Sensor %d is %d\n:",num,val);
	e_send_uart1_char(str,str_length);
}

void bt_distance(void)
{
	char str[100];
	int str_length;
	uint16_t val=VL53L0x_get_dist_mm(void);
	str_length=sprintf(str,"Current Distance :%d",val);
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
    VL53L0X_start();
    
    /* Values for Sensor Calibration 
    int sensor1_cal=300;
    int sensor2_cal=400;
    int sensor3_cal=800;
    int sensor4_cal=800;
    int sensor5_cal=800;
    int sensor6_cal=800;
    int sensor7_cal=400;
    int sensor8_cal=300;
    */

    while (1) 
    {
    	int sensor1=get_calibrated_prox(0);
    	int sensor2=get_calibrated_prox(1);
    	int sensor3=get_calibrated_prox(2);
    	int sensor4=get_calibrated_prox(3);
    	int sensor5=get_calibrated_prox(4);
    	int sensor6=get_calibrated_prox(5);
    	int sensor7=get_calibrated_prox(6);
    	int sensor8=get_calibrated_prox(7);
    	
    	int sensor1_sum=0;
    	int sensor2_sum=0;
    	int sensor3_sum=0;
    	int sensor4_sum=0;
    	int sensor5_sum=0;
    	int sensor6_sum=0;
    	int sensor7_sum=0;
    	int sensor8_sum=0;
    	    	
    for (int i=1;i<=10;i++)
    {
    	sensor1_sum=sensor1_sum+sensor1;
    	sensor2_sum=sensor2_sum+sensor2;
		sensor3_sum=sensor3_sum+sensor3;
		sensor4_sum=sensor4_sum+sensor4;
		sensor5_sum=sensor5_sum+sensor5;
		sensor6_sum=sensor6_sum+sensor6;
		sensor7_sum=sensor7_sum+sensor7;
		sensor8_sum=sensor8_sum+sensor8;
    }
    
    int sensor1_av=sensor1_sum/10;
    int sensor2_av=sensor2_sum/10;
    int sensor3_av=sensor3_sum/10;
    int sensor4_av=sensor4_sum/10;
    int sensor5_av=sensor5_sum/10;
    int sensor6_av=sensor6_sum/10;
    int sensor7_av=sensor7_sum/10;
    int sensor8_av=sensor8_sum/10;
    
    int nearest = nearest_sensor(sensor1_av,sensor2_av,sensor3_av,sensor4_av,sensor5_av,sensor6_av,sensor7_av,sensor8_av);
    
    if (nearest==2 || nearest == 3 || nearest == 4 )
    {
    	turn_right();
    	set_front_led(unsigned int 0);
    	set_body_led(unsigned int 1);
    }
    else if ( nearest==5 || nearest == 6 || nearest == 7)
    {
    	turn_left();
    	set_front_led(unsigned int 0);
    	set_body_led(unsigned int 1);
    }
    else
    {
    	uint16_t distance=VL53L0X_get_dist_mm();
    	set_front_led(unsigned int 1);
    	set_body_led(unsigned int 0);
    	if (distance > 30 )
    	{
    		move_forward();
    		bt_distance();
	  	}
    	else if (distance <20)
    	{
    		move_backward();
    		bt_distance();
    	}
    }
        
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
