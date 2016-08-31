/*
 * jord.c
 *
 * Created: 22/04/2015 09:12:59 a.m.
 *  Author: NickLaptop
 */ 

#include "../include/jord.h"

const int FULL_SPEED_LIST[10]={100,80,60,40,20};
const int MEDIUM_SPEED_LIST[10]={70,56,42,28,14};
const int LOW_SPEED_LIST[10]={50,40,30,20,10};

struct pwm_config servo_seleccionador;
struct pwm_config servo_pala;
struct pwm_config servo_puerta_der;
struct pwm_config servo_puerta_izq;

void init_jord(void)
{
	init_motors(PA4,PA5,PWM_PC0,PA6,PA7,PWM_PC1,PB0,PB1,PWM_PC2,PB2,PB3,PWM_PC4);
	sensor_array_init(I2C_PORT);
	sensor_dual_init(PD4,PD5);
	//sensor_array_enable_irq();
	set_pin_as_input(PC5); //Iniciar sensor IR
	set_pin_as_input(PC6); //Iniciar sensor de metales
	
	//Posición Inicial
	servo_init(&servo_puerta_izq,PWM_PD1);
	servo_init(&servo_puerta_der,PWM_PD0);
	abrir_puertas();
	delay_ms(600);
	servo_init(&servo_pala,PWM_PD2);
	moving();
	delay_ms(300);
	servo_init(&servo_seleccionador,PWM_PD3);
	botella();
}

void abrir_puertas(void)
{
	pwm_update(&servo_puerta_izq,PUERTA_IZQ_ABIERTO);
	pwm_update(&servo_puerta_der,PUERTA_DER_ABIERTO);
}

void cerrar_puertas(void)
{
	pwm_update(&servo_puerta_izq,PUERTA_IZQ_CERRADO);
	pwm_update(&servo_puerta_der,PUERTA_DER_CERRADO);
	delay_ms(500);
	pwm_update(&servo_puerta_izq,PUERTA_IZQ_MEDIA);
	pwm_update(&servo_puerta_der,PUERTA_DER_MEDIA);
	delay_ms(500);
	pwm_update(&servo_puerta_izq,PUERTA_IZQ_CERRADO);
	pwm_update(&servo_puerta_der,PUERTA_DER_CERRADO);
}

void subir_pala(void)
{
	for (int i=2;i<PALA_ARRIBA-4;i=i+2)
	{
		pwm_update(&servo_pala,PALA_ABAJO+i);
		delay_ms(23);
	}
	pwm_update(&servo_pala,PALA_ARRIBA);
}

void bajar_pala(void)
{
	pwm_update(&servo_pala,PALA_ABAJO);
}

void moving(void)
{
	pwm_update(&servo_pala,PALA_MOVING);
	delay_ms(800);
	pwm_update(&servo_puerta_izq,PUERTA_IZQ_MEDIA);
	pwm_update(&servo_puerta_der,PUERTA_DER_MEDIA);
}

void lata(void)
{
	pwm_update(&servo_seleccionador,SELECCIONADOR_DERECHA);
}

void botella(void)
{
	pwm_update(&servo_seleccionador,SELECCIONADOR_IZQUIERDA);
}

int ir_sensor_read(void)
{
	return read_pin(PC5);
}

int metal_sensor_read(void)
{
	return read_pin(PC6);
}

void follow_p_rever(void)
{
	int pos = sensor_array_position(I2C_PORT);
	int aux=sensor_array_read(I2C_PORT);
	int a0=(aux & (1<<0) ) && (1<<0);
	int a1=(aux & (1<<1) ) && (1<<1);
	int a2=(aux & (1<<2) ) && (1<<2);
	int a3=(aux & (1<<3) ) && (1<<3);
	int a4=(aux & (1<<4) ) && (1<<4);
	int a5=(aux & (1<<5) ) && (1<<5);
	int a6=(aux & (1<<6) ) && (1<<6);
	int a7=(aux & (1<<7) ) && (1<<7);
	int sum=a0+a1+a2+a3+a4+a5+a6+a7;
	move(-50,-50);
	while(sum<4)
	{
		int left_speed;
		int right_speed;
		aux=sensor_array_read(I2C_PORT);
		
		a0=(aux & (1<<0) ) && (1<<0);
		a1=(aux & (1<<1) ) && (1<<1);
		a2=(aux & (1<<2) ) && (1<<2);
		a3=(aux & (1<<3) ) && (1<<3);
		a4=(aux & (1<<4) ) && (1<<4);
		a5=(aux & (1<<5) ) && (1<<5);
		a6=(aux & (1<<6) ) && (1<<6);
		a7=(aux & (1<<7) ) && (1<<7);
		sum=a0+a1+a2+a3+a4+a5+a6+a7;
//		pos=sensor_array_position(I2C_PORT);
// 		if (pos==0)
// 		{
// 			left_speed=-LOW_SPEED_LIST[0];
// 			right_speed=-LOW_SPEED_LIST[0];
// 		}
// 		else if(pos<0)
// 		{
// 			pos=-pos;
// 			right_speed=-LOW_SPEED_LIST[0];
// 			left_speed=-LOW_SPEED_LIST[pos];
// 		}
// 		else
// 		{
// 			left_speed=-LOW_SPEED_LIST[0];
// 			right_speed=-LOW_SPEED_LIST[pos];
// 		}
// 		move(right_speed,left_speed);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_CENTER;
}

// void back_left(void) // ORIGINAL
// {
// 	int sens_RRR1;
// 	int sens_RRR2;
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	move(-50,-50);
// 	while(sens_RRR1==0 && sens_RRR2==0)
// 	{
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	}
// 	MOVE_STOP;
// 	delay_ms(STOP_DELAY);
// // 	MOVE_CENTER;
// // 	delay_ms(50);
// 	MOVE_ROTATE_LEFT;
// 	delay_ms(700);
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	int pos=sensor_array_position(I2C_PORT);
// 	while(sens_RRR1==0 && sens_RRR2==0 && pos!=0)
// 	{
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 		pos=sensor_array_position(I2C_PORT);
// 	}
// 	MOVE_STOP;
// 	delay_ms(STOP_DELAY);
// 	MOVE_CENTER;
// 	delay_ms(300);
// }

void back_left(void) // NUEVO
{
	int sens_RRR1;
	int sens_RRR2;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	move(-50,-50);
	while(sens_RRR1==0 && sens_RRR2==0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	// 	MOVE_CENTER;
	// 	delay_ms(50);
	MOVE_ROTATE_LEFT;
	delay_ms(700);
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	int pos=sensor_array_position(I2C_PORT);
	
		int flag=0;
		while(sens_RRR1==0 && sens_RRR2==0 && flag==0)
		{
			sensor_dual_read(&sens_RRR1,&sens_RRR2);
			pos=sensor_array_position(I2C_PORT);
			if(pos!=FAIL_POSITION && pos<=0)
			{
				flag=1;
			}
		}


	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_CENTER;
	int n_aux=0;
	while(n_aux<TIME_NAUX)
	{
		follow_p(MEDIUM_SPEED);
		delay_ms(1);
		n_aux+=1;
	}
}
// 
// void back_right(void) //ORIGINAL
// {
// 	int sens_RRR1;
// 	int sens_RRR2;
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	move(-50,-50);
// 	while(sens_RRR1==0 && sens_RRR2==0)
// 	{
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	}
// 	MOVE_STOP;
// 	delay_ms(STOP_DELAY);
// 	// 	MOVE_CENTER;
// 	// 	delay_ms(50);
// 	MOVE_ROTATE_RIGHT;
// 	delay_ms(700);
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	int pos=sensor_array_position(I2C_PORT);
// 	while(sens_RRR1==0 && sens_RRR2==0 && pos!=0)
// 	{
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 		pos=sensor_array_position(I2C_PORT);
// 	}
// 	MOVE_STOP;
// 	delay_ms(STOP_DELAY);
// 	MOVE_CENTER;
// 	delay_ms(300);
// }


void back_right(void) //NUEVO
{
	int sens_RRR1;
	int sens_RRR2;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	move(-50,-50);
	while(sens_RRR1==0 && sens_RRR2==0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	// 	MOVE_CENTER;
	// 	delay_ms(50);
	MOVE_ROTATE_RIGHT;
	delay_ms(700);
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	int pos=sensor_array_position(I2C_PORT);
	
	int flag=0;
	while(sens_RRR1==0 && sens_RRR2==0 && flag==0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
		pos=sensor_array_position(I2C_PORT);
		if(pos!=FAIL_POSITION && pos>=0)
		{
			flag=1;
		}
	}
	
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_CENTER;
	int n_aux=0;
	while(n_aux<TIME_NAUX)
	{
		follow_p(MEDIUM_SPEED);
		delay_ms(1);
		n_aux+=1;
	}
}

void follow_p(int speed)
{
	int aux=sensor_array_position(I2C_PORT);
	int left_speed;
	int right_speed;
	//printf("pos: %i\n",aux);
	if(aux!=FAIL_POSITION)
	{
		switch(speed)
		{
			case FULL_SPEED:
			if (aux==0)
			{
				left_speed=FULL_SPEED_LIST[0];
				right_speed=FULL_SPEED_LIST[0];
			}
			else if(aux<0)
			{
				aux=-aux;
				left_speed=FULL_SPEED_LIST[0];
				right_speed=FULL_SPEED_LIST[aux];
			}
			else
			{
				right_speed=FULL_SPEED_LIST[0];
				left_speed=FULL_SPEED_LIST[aux];
			}
			break;
			case MEDIUM_SPEED:
			if (aux==0)
			{
				left_speed=MEDIUM_SPEED_LIST[0];
				right_speed=MEDIUM_SPEED_LIST[0];
			}
			else if(aux<0)
			{
				aux=-aux;
				left_speed=MEDIUM_SPEED_LIST[0];
				right_speed=MEDIUM_SPEED_LIST[aux];
			}
			else
			{
				right_speed=MEDIUM_SPEED_LIST[0];
				left_speed=MEDIUM_SPEED_LIST[aux];
			}
			break;
			case LOW_SPEED:
			if (aux==0)
			{
				left_speed=LOW_SPEED_LIST[0];
				right_speed=LOW_SPEED_LIST[0];
			}
			else if(aux<0)
			{
				aux=-aux;
				left_speed=LOW_SPEED_LIST[0];
				right_speed=LOW_SPEED_LIST[aux];
			}
			else
			{
				right_speed=LOW_SPEED_LIST[0];
				left_speed=LOW_SPEED_LIST[aux];
			}
			break;
		}
		
		//printf("left %i	right %i\n",left_speed,right_speed);
		move(left_speed,right_speed);
	}
}
 

void recoger_objeto(void)
{
	int n=0;
	int pos;
	int sin_botella=0;
	
	MOVE_STOP;
	abrir_puertas();
	botella();
	delay_ms(400);
	MOVE_CENTER;
	
	while(ir_sensor_read()==0 && sin_botella==0)
	{
		pos=sensor_array_position(I2C_PORT);
		if(pos==FAIL_POSITION)
		{
			led1(ON);
			n+=1;
		}
		else
		{
			led1(OFF);
			n=0;
		}
		delay_ms(5);
		if(n>TIMEOUT_PICK)
		{
			sin_botella=1;
		}
		follow_p(LOW_SPEED);
	}
	if(sin_botella==0)
	{
		MOVE_STOP;
		delay_ms(300);
		bajar_pala();
		delay_ms(400);
		cerrar_puertas();
		int i=0;
		while(i<1000)
		{
			if(metal_sensor_read()==1)
			{
				lata();
				i=1000;
			}
			delay_ms(10);
			i=i+10;
		}
		delay_ms(500);
		abrir_puertas();
		delay_s(1);
		subir_pala();
		delay_s(1);
		moving();
		delay_s(1);
		MOVE_CENTER;
	}
	
	else
	{
		MOVE_STOP;
		delay_ms(200);
		moving();
		delay_s(1);
		move(-50,-50);
	}
}

void go_ahead(void)
{
	int sens_RRR1;
	int sens_RRR2;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	while(sens_RRR1==0 && sens_RRR2==0)
	{
		follow_p(LOW_SPEED);
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	move(50,50);
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	delay_ms(20);
	while(sens_RRR1==1 || sens_RRR2==1)
	{
		follow_p(MEDIUM_SPEED);
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	delay_ms(30);
}
// 
// void turn_left(void) // ORIGINAL
// {
// 	int sens_RRR1;
// 	int sens_RRR2;
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	while(sens_RRR1==0 && sens_RRR2==0)
// 	{
// 		follow_p(MEDIUM_SPEED);
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	}
// 	MOVE_STOP;
// 	delay_ms(STOP_DELAY);
// 	MOVE_ROTATE_LEFT;
// 	delay_ms(700);
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	int pos=sensor_array_position(I2C_PORT);
// 	while(sens_RRR1==0 && sens_RRR2==0 && pos!=0)
// 	{
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 		pos=sensor_array_position(I2C_PORT);
// 	}
// 	MOVE_STOP;
// 	delay_ms(STOP_DELAY);
// 	MOVE_CENTER;
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	while(sens_RRR1==1 || sens_RRR2==1)
// 	{
// 		follow_p(MEDIUM_SPEED);
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	}
// 	delay_ms(DELAY_AFTER_TURN);
// }

void turn_left(void) // NUEVO
{
	int sens_RRR1;
	int sens_RRR2;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	while(sens_RRR1==0 && sens_RRR2==0)
	{
		follow_p(MEDIUM_SPEED);
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_ROTATE_LEFT;
	delay_ms(700);
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	int pos=sensor_array_position(I2C_PORT);
	int flag=0;
	while(sens_RRR1==0 && sens_RRR2==0 && flag==0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
		pos=sensor_array_position(I2C_PORT);
		if(pos!=FAIL_POSITION && pos<=0)
		{
			flag=1;
		}
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	
	
	MOVE_CENTER;
	int n_aux=0;
	while(n_aux<TIME_NAUX)
	{
		follow_p(MEDIUM_SPEED);
		delay_ms(1);
		n_aux+=1;
	}
}
// 
// void turn_right(void) // ORIGINAL
// {
// 	int sens_RRR1;
// 	int sens_RRR2;
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	while(sens_RRR1==0 && sens_RRR2==0)
// 	{
// 		follow_p(MEDIUM_SPEED);
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	}
// 	MOVE_STOP;
// 	delay_ms(STOP_DELAY);
// 	MOVE_ROTATE_RIGHT;
// 	delay_ms(700);
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	int pos=sensor_array_position(I2C_PORT);
// 	while(sens_RRR1==0 && sens_RRR2==0 && pos!=0)
// 	{
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 		pos=sensor_array_position(I2C_PORT);
// 	}
// 	MOVE_STOP;
// 	delay_ms(STOP_DELAY);
// 	MOVE_CENTER;
// 	sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	while(sens_RRR1==1 || sens_RRR2==1)
// 	{
// 		follow_p(MEDIUM_SPEED);
// 		sensor_dual_read(&sens_RRR1,&sens_RRR2);
// 	}
// 	delay_ms(DELAY_AFTER_TURN);
// }

void turn_right(void) // NUEVO
{
	int sens_RRR1;
	int sens_RRR2;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	while(sens_RRR1==0 && sens_RRR2==0)
	{
		follow_p(MEDIUM_SPEED);
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_ROTATE_RIGHT;
	delay_ms(700);
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	int pos=sensor_array_position(I2C_PORT);
	int flag=0;
	while(sens_RRR1==0 && sens_RRR2==0 && flag==0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
		pos=sensor_array_position(I2C_PORT);
		if(pos!=FAIL_POSITION && pos>=0)
		{
			flag=1;
		}
	}
	
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	
	MOVE_CENTER;
	int n_aux=0;
	while(n_aux<TIME_NAUX)
	{
		follow_p(MEDIUM_SPEED);
		delay_ms(1);
		n_aux+=1;
	}
}

void turn_right_pick(void)
{
	int sens_RRR1;
	int sens_RRR2;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	while(sens_RRR1==0 && sens_RRR2==0)
	{
		follow_p(MEDIUM_SPEED);
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_ROTATE_RIGHT;
	delay_ms(700);
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	int pos=sensor_array_position(I2C_PORT);
	while(sens_RRR1==0 && sens_RRR2==0 && pos!=0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
		pos=sensor_array_position(I2C_PORT);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_CENTER;
}

void turn_left_pick(void)
{
	int sens_RRR1;
	int sens_RRR2;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	while(sens_RRR1==0 && sens_RRR2==0)
	{
		follow_p(MEDIUM_SPEED);
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_ROTATE_LEFT;
	delay_ms(700);
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	int pos=sensor_array_position(I2C_PORT);
	while(sens_RRR1==0 && sens_RRR2==0 && pos!=0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
		pos=sensor_array_position(I2C_PORT);
	}
	MOVE_STOP;
	delay_ms(STOP_DELAY);
	MOVE_CENTER;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	while(sens_RRR1==1 || sens_RRR2==1)
	{
		follow_p(MEDIUM_SPEED);
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	delay_ms(DELAY_AFTER_TURN);
}

void u_turn(void)
{
	int sens_RRR1;
	int sens_RRR2;
	MOVE_STOP;
	delay_ms(500);
	MOVE_ROTATE_RIGHT;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	while(sens_RRR1==0 && sens_RRR2==0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	MOVE_STOP;
	delay_ms(500);
	MOVE_ROTATE_RIGHT;
	delay_ms(700);
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	int pos=sensor_array_position(I2C_PORT);
	while(sens_RRR1==0 && sens_RRR2==0 && pos!=0)
	{
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
		pos=sensor_array_position(I2C_PORT);
	}
	MOVE_STOP;
	delay_ms(300);
	MOVE_CENTER;
	sensor_dual_read(&sens_RRR1,&sens_RRR2);
	while(sens_RRR1==1 || sens_RRR2==1)
	{
		follow_p(MEDIUM_SPEED);
		sensor_dual_read(&sens_RRR1,&sens_RRR2);
	}
	delay_ms(100);
}

void navegation(int *instructions,int n)
{
	MOVE_CENTER;
	int i=0;;
	while(i<n)
	{
		switch(instructions[i])
		{
			case AHEAD:
			go_ahead();
			break;
			case LEFT:
			turn_left();
			break;
			case RIGHT:
			turn_right();
			break;
			case PICK:
			follow_p_rever();
			recoger_objeto();
			break;
			case UTURN:
			u_turn();
			break;
			case BACK:
			follow_p_rever();
			break;
			case BACK_LEFT:
			back_left();
			break;
			case BACK_RIGHT:
			back_right();
			break;
			case RIGHT_PICK:
			turn_right_pick();
			break;
			case LEFT_PICK:
			turn_left_pick();
			break;
		}
		i++;
	}
}

