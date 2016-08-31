/*
 * line_sensor.c
 *
 * Created: 04/04/2015 02:53:48 p.m.
 *  Author: Nick
 */ 


#include "OctoPlus/include/line_sensor.h"

void sensor_array_init(TWI_t *TWI_PORT)
{
	twi_master_options_t opt = {
		.speed = 50000,
		.chip  = 0x20
	};
	twi_master_setup(TWI_PORT, &opt);
}

int sensor_array_read(TWI_t *TWI_PORT)
{
	
	uint8_t data_received[10];
	twi_package_t packet_read = {
		.addr         = 0xFF,      // TWI slave memory address data
		.addr_length  = 0,    // TWI slave memory address data size
		.chip         = 0x20,      // TWI slave bus address
		.buffer       = data_received,        // transfer data destination buffer
		.length       = 1                    // transfer data size (bytes)
	};
	
	if(twi_master_read(TWI_PORT, &packet_read) == TWI_SUCCESS)
	{
		return data_received[0];
	}
}


//Function that show the current position of the sensor. the number is from 4 (line in the left) to -4 (line in the right with steps of 1 and the center is 0.
int sensor_array_position(TWI_t *TWI_PORT)
{
	int aux=sensor_array_read(TWI_PORT);
	int pos=FAIL_POSITION;
	switch(aux)
	{
		case 0b10000000:
			pos=4;
			break;
		case 0b11000000:
			pos=3;
			break;
		case 0b11100000:
			pos=3;
			break;
		case 0b01000000:
			pos=3;
			break;
		case 0b01100000:
			pos=2;
			break;
		case 0b01110000:
			pos=2;
			break;
		case 0b00100000:
			pos=2;
			break;
		case 0b00110000:
			pos=1;
			break;
		case 0b00111000:
			pos=1;
			break;
		case 0b00010000:
			pos=1;
			break;
		case 0b00011000:
			pos=0;
			break;
		case 0b00011100:
			pos=-1;
			break;
		case 0b00001000:
			pos=-1;
			break;
		case 0b00001100:
			pos=-1;
			break;
		case 0b00001110:
			pos=-2;
			break;
		case 0b00000100:
			pos=-2;
			break;
		case 0b00000110:
			pos=-2;
			break;
		case 0b00000111:
			pos=-3;
			break;
		case 0b00000010:
			pos=-3;
			break;
		case 0b00000011:
			pos=-3;
			break;
		case 0b00000001:
			pos=-4;
			break;
	}
	return pos;
}

void sensor_dual_init(ioport_pin_t PIN_A,ioport_pin_t PIN_B)
{
	SENS1 = PIN_A;
	SENS2 = PIN_B;
	set_pin_as_input(SENS1);
	set_pin_as_input(SENS2);
}

void sensor_dual_read(int *sensor1, int *sensor2)
{
	*sensor1 = read_pin(SENS1);
	*sensor2 = read_pin(SENS2);
}

void sensor_array_enable_irq(void)
{
	pmic_init();
	pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN);
	cpu_irq_disable();
	PORTE.INTCTRL |= 0x04;
	PORTE.INT1MASK = (1<<3);
	PORTE.PIN3CTRL |= (1<<1);
	cpu_irq_enable();
}




