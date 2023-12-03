#ifndef __SG90_H
#define __SG90_H

//include
#include "main.h"
#include "stdio.h"
#include "math.h"

#define Servo_log printf
typedef float ServoAngle;

//Servo OOP_By_C
typedef struct Servo_OOC {
    //elements
    char *name;
    ServoAngle prTarget_Angle;
    ServoAngle prAngle_Offset;

    TIM_HandleTypeDef *TIMER;
    uint32_t TIM_Channel;
    uint32_t TIMFreq;
    float prTIMCompareValue;

    ServoAngle prLastAngle;

    //functions
    float (*GetPulseTime_ms)(struct Servo_OOC *);
    void (*api_hardware_PWMSet)(struct Servo_OOC *);
    uint32_t (*api_GetTimerFreq)(struct Servo_OOC *);
} Servo;

//extern
extern Servo Servo_LeftLeg;
extern Servo Servo_RightLeg;

//Create an Oriented
Servo Servo_Create(char *name, TIM_HandleTypeDef *TIMER, uint32_t Channel, ServoAngle angle_Offset);

//Application functions
void Servo_init();

/*
 * 	@函数功能:  设定舵机角度
 *  @参数1: 要被控制的舵机对象
 *  @参数2: 舵机的目标角度
*/
void setAngle_180(struct Servo_OOC *Servo, ServoAngle target_Angle);

void setAngle_270(struct Servo_OOC *Servo, ServoAngle target_Angle);

/*
 * 	@函数功能:  设定舵机角度--pitch角模式
 *  @参数1: 要被控制的舵机对象
 *  @参数2: 舵机的起始角度
 *	@参数3: 舵机的目标角度
 *	@参数4: 舵机起始时的yaw角
 *	@参数5: 每次进动的步长, 进行角度插值
*/
void Servo_move_pitch(struct Servo_OOC *Servo, ServoAngle start_pitch, ServoAngle end_pitch, ServoAngle start_yaw, int time);

void Servo_move_yaw(struct Servo_OOC *Servo, ServoAngle start_yaw, ServoAngle end_yaw, ServoAngle start_pitch, int time);

#endif