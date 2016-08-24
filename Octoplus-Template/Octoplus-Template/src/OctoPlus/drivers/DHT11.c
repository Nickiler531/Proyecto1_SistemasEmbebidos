/*
 * DHT11.c
 *
 * Created: 8/17/2016 12:51:58 PM
 *  Author: Nick
 */ 

#include "../include/DHT11.h"

int DHT11_read(void)
{
	unsigned long info[BIT_CNT];
	int i = 0;
	int pin_state = 1;
	volatile int cnt = 0;
	int hbit_1, hbit_2; 
	
	//Init Signal from MCU
	ioport_set_pin_dir(PC7,IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PC7,0);
	delay_ms(20);
	ioport_set_pin_level(PC7,1);
	ioport_set_pin_dir(PC7,IOPORT_DIR_INPUT);
	
	//Wait init signal from DTH11
	pin_state = 1;
	for (i=0;i<3;i++)
	{
		cnt = 0;
		while(ioport_get_pin_level(PC7) == pin_state)
		{
			cnt +=1;
			if(cnt>0xFF)
			{
				return DHT11_ERROR;
			}
		}
		pin_state ^= 1;
	}
	//Start reading the 40 bytes
	for (i=0;i<BIT_CNT;i++)
	{
		hbit_1 = 0;
		hbit_2 = 0;
		//Wait first half of bit and save the duration
		while(ioport_get_pin_level(PC7) == pin_state)
		{
			hbit_1 +=1;
			if(hbit_1>0xFF)
			{
				return DHT11_ERROR;
			}
		}
		pin_state ^= 1;
		//wait the second half of the bit and save the duration
		while(ioport_get_pin_level(PC7) == pin_state)
		{
			hbit_2 +=1;
			if(hbit_2>0xFF)
			{
				return DHT11_ERROR;
			}
		}
		pin_state ^= 1;
		
		if (hbit_1<hbit_2)
		{
			info[i] = 1;
		}
		else
		{
			info[i] = 0;
		}
	}

	//Convert to numbers
	int rh_i = 0;
	int rh_d = 0;
	int temp_i = 0;
	int temp_d = 0;
	for (i=0;i<BIT_CNT;i++)
	{
		if (i < 8)
		{
			rh_i |= (info[i] << (7-i));
		}
		else if (i < 16)
		{
			rh_d |= (info[i] << (15 - i));
		}
		else if (i < 32)
		{
			temp_i |= (info[i] << (23 - i));
		}
		else
		{
			temp_d |= (info[i] << (23 - i));
		}
	}

	printf("RH = %d.%d: Temp = %d.%d\n",rh_i, rh_d, temp_i, temp_d);
	return DHT11_SUCESS;	
}

// static void convert(unsigned long * info)
// {
// 	//Convert to numbers
// 	rh_i = 0;
// 	rh_d = 0;
// 	temp_i = 0;
// 	temp_d = 0;
// 	for (i=0;i<BIT_CNT;i++)
// 	{
// 		if (i < 8)
// 		{
// 			rh_i |= (info[i] << (7-i));
// 		}
// 		else if (i < 16)
// 		{
// 			rh_d |= (info[i] << (15 - i));
// 		}
// 		else if (i < 32)
// 		{
// 			temp_i |= (info[i] << (23 - i));
// 		}
// 		else
// 		{
// 			temp_d |= (info[i] << (23 - i));
// 		}
// 	}
// }
