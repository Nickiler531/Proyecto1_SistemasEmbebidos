/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include "OctoPlus/OctoPlus.h"



// ISR(PORTE_INT0_vect) //  Sensor gestos
// {
// }
// ISR(PORTE_INT1_vect) // sensor Linea
// {
// }



int main (void)
{
	init_OctoPlus();
	animation();
	
	int rh_i, rh_d, temp_i, temp_d;
	char aux;
	
	for (;;)
	{
		//ioport_set_pin_level(LED1,0);
		delay_ms(500);
		DHT11_read();
		
// 		scanf("%c", &aux);
// 		if(aux == 'a')
// 		{
// 			ioport_set_pin_level(LED1,1);
// 			DHT11_read();
// 		}
// 		else
// 		{
// 			printf("Error\n");
// 		}
	}
}
