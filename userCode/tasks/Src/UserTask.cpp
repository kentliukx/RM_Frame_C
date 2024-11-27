//
// Created by LEGION on 2021/10/4.
//
#include "UserTask.h"
#include "ControlTask.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"

enum two_switch_state
{
    up=0x0161,
    down=0x069E
};

two_switch_state left_switch,right_switch;
int16_t collect_speed=0, best_elevate_speed=-175;

PID_Regulator_t userPidRegulator = {
        .kp = 60,
        .ki = 0,
        .kd = 0,
        .componentKpMax = 10000,
        .componentKiMax = 0,
        .componentKdMax = 0,
        .outputMax = 10000 //2006电机输出电流上限，可以调小，勿调大
};

MOTOR_INIT_t userMotorInit = {
        .speedPIDp = &userPidRegulator,
        .anglePIDp = nullptr,
        ._motorID = MOTOR_ID_1,
        .reductionRatio = 36.0f,
        .ctrlType = SPEED_Single,
};

Motor UserMotor_collect(MOTOR_ID_6,&userMotorInit);
Motor UserMotor_elevate(MOTOR_ID_5,&userMotorInit);
/***
 * 在这里放入xxx.stop()即可使舵机,电机在遥控器急停挡位断电
 */
void UserStop(){
    UserMotor_collect.Stop();
    UserMotor_elevate.Stop();
}

/***
 * 在这里写入初始化内容
 */
void UserInit(){
    UserMotor_elevate.SetTargetSpeed(0);
    UserMotor_collect.SetTargetSpeed(0);

    HAL_GPIO_WritePin(PIN1_GPIO_Port,PIN1_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PIN3_GPIO_Port,PIN3_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PIN5_GPIO_Port,PIN5_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PIN7_GPIO_Port,PIN7_Pin,GPIO_PIN_RESET);
}



/***
 * 用户自定义任务主循环
 */


void UserMotorHandle()
{
    if (RemoteControl::rcInfo.sLeft == DOWN_POS)//collect setting mode
    {
        UserMotor_elevate.SetTargetSpeed(0);
        if(left_switch==up)
        {
            collect_speed=RemoteControl::rcInfo.right_col*250;
            UserMotor_collect.SetTargetSpeed(collect_speed);
        }
        else UserMotor_collect.SetTargetSpeed(0);
    }
    else if(left_switch==up)
        UserMotor_collect.SetTargetSpeed(collect_speed);
    else
        UserMotor_collect.SetTargetSpeed(0);

    if (right_switch==up)//auto elevate
    {
        UserMotor_elevate.SetTargetSpeed(best_elevate_speed);
    }
    else if (RemoteControl::rcInfo.sLeft == MID_POS)//elevate mode
    {
        UserMotor_elevate.SetTargetSpeed(RemoteControl::rcInfo.right_col*250);
    }
    else
    {
        UserMotor_elevate.SetTargetSpeed(0);
    }
    UserMotor_collect.Handle();
    UserMotor_elevate.Handle();
}

void UserClimb()
{
    if(RemoteControl::rcInfo.sLeft == UP_POS)
    {
        //left ctrl
        HAL_GPIO_WritePin(PIN1_GPIO_Port,PIN1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(PIN3_GPIO_Port,PIN3_Pin,GPIO_PIN_RESET);
        //right ctrl
        HAL_GPIO_WritePin(PIN5_GPIO_Port,PIN5_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(PIN7_GPIO_Port,PIN7_Pin,GPIO_PIN_RESET);
    }
    else if(RemoteControl::rcInfo.sLeft == DOWN_POS)
    {
        HAL_GPIO_WritePin(PIN1_GPIO_Port,PIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(PIN3_GPIO_Port,PIN3_Pin,GPIO_PIN_SET);

        HAL_GPIO_WritePin(PIN5_GPIO_Port,PIN5_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(PIN7_GPIO_Port,PIN7_Pin,GPIO_PIN_SET);
    }
    else
    {
        //stop
        HAL_GPIO_WritePin(PIN1_GPIO_Port,PIN1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(PIN3_GPIO_Port,PIN3_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(PIN5_GPIO_Port,PIN5_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(PIN7_GPIO_Port,PIN7_Pin,GPIO_PIN_RESET);
    }
}

void UserHandle()
{
    left_switch=(two_switch_state)RemoteControl::rcInfo.optional[0];
    right_switch=(two_switch_state)RemoteControl::rcInfo.optional[1];

    if (RemoteControl::rcInfo.right_col>1.1||RemoteControl::rcInfo.right_col<-1.1)//not connected
    {
        UserStop();
    }
    else if(RemoteControl::rcInfo.sRight==DOWN_POS)//stop all motors
    {
        UserStop();
    }
    else if(RemoteControl::rcInfo.sRight==MID_POS)
    {
        UserMotorHandle();
    }
    else if(RemoteControl::rcInfo.sRight==UP_POS)
    {
        UserStop();
        UserClimb();
    }

}

