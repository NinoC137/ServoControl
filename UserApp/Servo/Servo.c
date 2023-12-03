#include "Servo.h"

Servo Servo_LeftLeg;
Servo Servo_RightLeg;

static float getPulseTime_ms(struct Servo_OOC *Servo) {
    return Servo->prTIMCompareValue / (float)Servo->TIMFreq;
}

static void api_PWMCompareWrite(struct Servo_OOC *Servo) {
    //设定定时器比较值, 以此产生不同的占空比
            __HAL_TIM_SetCompare(Servo->TIMER, Servo->TIM_Channel, Servo->prTIMCompareValue);
}

static uint32_t api_getTimerFreq(struct Servo_OOC *Servo){
    //默认各个时钟都是相同的频率值 故此处用PCLK1来统一代替, 不做区分
    return HAL_RCC_GetPCLK1Freq() / Servo->TIMER->Init.Prescaler;
}

void setAngle_180(struct Servo_OOC *Servo, ServoAngle target_Angle) {
    Servo->prLastAngle = target_Angle;
    target_Angle += Servo->prAngle_Offset;

    /********角度限幅, 使用反馈控制时可按需定义*************/
//    if(target_Angle <= 20)
//        target_Angle = 20;
//    if(target_Angle >= 75)
//        target_Angle = 75;
    /************************************************/

    Servo->prTarget_Angle = target_Angle;
    //换算角度至脉冲时长对应的占空比比较值
    Servo->prTIMCompareValue = (5e-4f * (float) Servo->TIMFreq +
                                  (Servo->prTarget_Angle / 180.00f) * (2e-3f * (float) Servo->TIMFreq));

    if (Servo->prTIMCompareValue > Servo->TIMER->Init.Period) {
        Servo_log("Counter Period overflow!\r\n");
    } else {
        api_PWMCompareWrite(Servo);
    }
}

void setAngle_270(struct Servo_OOC *Servo, ServoAngle target_Angle) {
    Servo->prLastAngle = target_Angle;
    target_Angle += Servo->prAngle_Offset;

    /********角度限幅, 使用反馈控制时可按需定义*************/
//    if(target_Angle <= 20)
//        target_Angle = 20;
//    if(target_Angle >= 75)
//        target_Angle = 75;
    /************************************************/

    Servo->prTarget_Angle = target_Angle;
    //换算角度至脉冲时长对应的占空比比较值
    Servo->prTIMCompareValue = (5e-4f * (float) Servo->TIMFreq +
                                  (Servo->prTarget_Angle / 270.00f) * (2e-3f * (float) Servo->TIMFreq));

    if (Servo->prTIMCompareValue > Servo->TIMER->Init.Period) {
        Servo_log("Counter Period overflow!\r\n");
    } else {
        api_PWMCompareWrite(Servo);
    }
}

void Servo_move_pitch(struct Servo_OOC *Servo, ServoAngle start_pitch, ServoAngle end_pitch, ServoAngle start_yaw, int time) {
    int i;
    float width;
    if (start_yaw >= 0) {
        width = (100 / cosf(fabsf(start_pitch))) * tanf(fabsf(start_yaw));
    } else {
        width = -(100 / cosf(fabsf(start_pitch))) * tanf(fabsf(start_yaw));
    }
    float temp = (end_pitch - start_pitch) / 100;
    for (i = 0; i < time; i++) {
        start_pitch += temp;
        if (start_pitch > end_pitch)break;
        setAngle_180(&Servo_LeftLeg, start_pitch);
        api_PWMCompareWrite(&Servo_LeftLeg);
        start_yaw = atanf(width / (100 / cosf(fabsf(start_pitch))));
        setAngle_270(&Servo_RightLeg, start_yaw);
        api_PWMCompareWrite(&Servo_RightLeg);
        HAL_Delay(10);
    }
}

void Servo_move_yaw(struct Servo_OOC *Servo, ServoAngle start_yaw, ServoAngle end_yaw, ServoAngle start_pitch, int time) {
    int i;
    float high;
    if (start_pitch >= 0) {
        high = (100 / cosf(fabsf(start_yaw))) * tanf(fabsf(start_pitch));

    } else {
        high = -(100 / cosf(fabsf(start_yaw))) * tanf(fabsf(start_pitch));
    }
    float temp = (end_yaw - start_yaw) / 100;
    for (i = 0; i < time; i++) {
        start_yaw += temp;
        if (start_yaw > end_yaw)break;
        setAngle_270(&Servo_RightLeg, start_yaw);
        api_PWMCompareWrite(&Servo_RightLeg);
        start_pitch = atanf(high / (100 / cosf(fabsf(start_yaw))));
        setAngle_180(&Servo_LeftLeg, start_pitch);
        api_PWMCompareWrite(&Servo_LeftLeg);
        HAL_Delay(10);
    }
}

Servo Servo_Create(char *name, TIM_HandleTypeDef *TIMER, uint32_t Channel, ServoAngle angle_Offset) {
    Servo servo_temple;

    servo_temple.name = name;
    servo_temple.TIMER = TIMER;
    servo_temple.TIM_Channel = Channel;

    servo_temple.TIMFreq = api_getTimerFreq(&servo_temple);

    //default arguments
    servo_temple.prTarget_Angle = 0;
    servo_temple.prTIMCompareValue = 5e-4f * (float) servo_temple.TIMFreq;
    servo_temple.prAngle_Offset = angle_Offset;

    //functions define
    servo_temple.GetPulseTime_ms = getPulseTime_ms;
    servo_temple.api_hardware_PWMSet = api_PWMCompareWrite;
    servo_temple.api_GetTimerFreq = api_getTimerFreq;

    return servo_temple;
}

void Servo_init() {
    Servo_LeftLeg = Servo_Create("LeftLeg", &htim1, TIM_CHANNEL_1, 0);
    Servo_RightLeg = Servo_Create("RightLeg", &htim1, TIM_CHANNEL_2, 0);

    HAL_TIM_PWM_Start(Servo_LeftLeg.TIMER, Servo_LeftLeg.TIM_Channel);
    HAL_TIM_PWM_Start(Servo_RightLeg.TIMER, Servo_RightLeg.TIM_Channel);
}