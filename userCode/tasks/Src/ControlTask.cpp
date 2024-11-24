//
// Created by LEGION on 2021/10/4.
//
#include "ControlTask.h"


void CtrlHandle()
{
    if (RemoteControl::rcInfo.sRight!=MID_POS){//右侧三档，急停模式
        ChassisStop();
        UserStop();
    }
    else if (RemoteControl::rcInfo.sRight==MID_POS)
    {
        switch (RemoteControl::rcInfo.sLeft)
        {
        case UP_POS:
            ChassisSetVelocity(RemoteControl::rcInfo.right_col*4.2,RemoteControl::rcInfo.right_rol*4.2,RemoteControl::rcInfo.left_rol*60);
            break;
        case MID_POS://左侧二档
            break;
        case DOWN_POS:
            break;
        default:
            break;
        }
    }
}