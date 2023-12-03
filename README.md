# 舵机控制

本项目实现了简单抽象的舵机对象及其控制函数封装

## 舵机对象属性

~~~ c
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
    float (*api_getTimerFreq)(struct Servo_OOC *);
} Servo;
~~~

其中主要分为:

1. 舵机自身属性
   1. 目标角度
   2. 初始角度偏置(比如希望舵机初始态处于中值90°
2. 定时器属性
   1. 定时器
   2. 定时器通道
   3. 定时器频率
   4. 定时器比较值
3. 硬件层封装
   1. 获取当前占空比对应的脉冲时长(对应角度值)
   2. 硬件层api --- 设定PWM的占空比
   3. 硬件层api --- 获取定时器的频率值

## 功能函数

~~~ c
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

~~~

调用较为简单, 在创建对象后, 通过此函数进行目标角度的设定.

注意: 舵机本身的运动需要时间, 函数内没有对运动时间进行处理, 实际使用时, 在调用此函数后, 应添加一定延时, 以确保运动至目标角度

改进: 加入角度反馈, 如引出舵机电位器的电压反馈, 根据反馈值来确定目前角度.



## 应用实例

~~~ c
void Servo_init() {
    Servo_LeftLeg = Servo_Create("LeftLeg", &htim1, TIM_CHANNEL_1, 0);
    Servo_RightLeg = Servo_Create("RightLeg", &htim1, TIM_CHANNEL_2, 0);

    HAL_TIM_PWM_Start(Servo_LeftLeg.TIMER, Servo_LeftLeg.TIM_Channel);
    HAL_TIM_PWM_Start(Servo_RightLeg.TIMER, Servo_RightLeg.TIM_Channel);
}

int main(void)
{
    Servo_init();

    printf("Servo1 name: %s\r\n", Servo_LeftLeg.name);
    printf("Servo2 name: %s\r\n", Servo_RightLeg.name);

  for(;;)
  {
      setAngle_180(&Servo_LeftLeg, 60);
      setAngle_180(&Servo_RightLeg, 120);
      HAL_Delay(1000);
      setAngle_180(&Servo_LeftLeg, 120);
      setAngle_180(&Servo_RightLeg, 60);
      HAL_Delay(1000);
  }
}
~~~





