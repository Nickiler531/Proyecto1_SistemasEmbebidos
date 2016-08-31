/*
 * jord.h
 *
 * Created: 22/04/2015 09:13:13 a.m.
 *  Author: NickLaptop
 */ 


#ifndef JORD_H_
#define JORD_H_

#include "OctoPlus/OctoPlus.h"

#define MOVE_CENTER move(50,50)
#define MOVE_STOP move(0,0)
#define MOVE_ROTATE_LEFT move(-50,50)
#define MOVE_ROTATE_RIGHT move(50,-50)

#define FULL_SPEED 0
#define MEDIUM_SPEED 1
#define LOW_SPEED 2

#define PALA_ARRIBA 103
#define PALA_ABAJO 43
#define PALA_MOVING 45
#define SELECCIONADOR_DERECHA 115
#define SELECCIONADOR_IZQUIERDA 40
#define PUERTA_DER_ABIERTO 80
#define PUERTA_DER_CERRADO 46
#define PUERTA_IZQ_ABIERTO 35
#define PUERTA_IZQ_CERRADO 70
#define PUERTA_IZQ_MEDIA 66
#define PUERTA_DER_MEDIA 50

#define AHEAD 1
#define LEFT 2
#define RIGHT 3
#define PICK 4
#define UTURN 5
#define BACK 6
#define RIGHT_PICK 7
#define LEFT_PICK 8
#define BACK_LEFT 9
#define BACK_RIGHT 10
#define STOP_DELAY 50
#define DELAY_AFTER_TURN 100
#define TIMEOUT_PICK 30 //Usado para ver si no hay botella en su lugar
#define TIME_NAUX 300

void init_jord(void);
void abrir_puertas(void);
void cerrar_puertas(void);
void subir_pala(void);
void bajar_pala(void);
void moving(void);
void lata(void);
void botella(void);
int ir_sensor_read(void);
int metal_sensor_read(void);
void follow_p_rever(void);
void follow_p_rever_short(void);
void follow_p(int speed);
void recoger_objeto(void);
void back_right(void);
void back_left(void);
void go_ahead(void);
void turn_left(void);
void turn_right(void);
void turn_right_pick(void);
void turn_left_pick(void);
void u_turn(void);
void navegation(int *instructions,int n);


#endif /* JORD_H_ */