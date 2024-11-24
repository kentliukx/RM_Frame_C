//
// Created by LEGION on 2021/10/4.
//
#include "UserTask.h"
#include "ControlTask.h"

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

}

/***
 * 用户自定义任务主循环
 */
void UserHandle(){
    if (RemoteControl::rcInfo.sLeft == DOWN_POS)//collect mode
    {
        UserMotor_collect.SetTargetSpeed(RemoteControl::rcInfo.right_col*300);
        UserMotor_elevate.SetTargetSpeed(0);
    }
    else if (RemoteControl::rcInfo.sLeft == MID_POS)//elevate mode
    {
        UserMotor_elevate.SetTargetSpeed(RemoteControl::rcInfo.right_col*500);
    }

    if(RemoteControl::rcInfo.right_col<-1.1) UserStop();//stop

    UserMotor_collect.Handle();
    UserMotor_elevate.Handle();

}