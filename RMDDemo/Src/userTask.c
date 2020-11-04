#include<stdio.h>
#include<sys/types.h>
#include"USBSerial.h"
#include"RMDDrvDriver.h"
#include"userTask.h"
#include"Config.h"

enum DrvRunStatusOnP1ST{
    SET_TARPOS_P1ST = 0x00,
    CHECK_CURRENTPOS,
    ARRIVE_TARPOS,
};

int8_t CheckDriverStatus(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);
u_int16_t GetEncoderData_ST(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);
int8_t SetposCmd_P1ST(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom, u_int16_t tarPos);


extern  u_int8_t cnt;

/**
  * @brief  循环执行的用户task，用户的task在此函数中添加
  * @param  ptask  指向当前Task的指针
  * @retval none
 **/
void UserTaskHandler(u_int8_t *ptask)
{
    int8_t tmp = 0;

    switch(*ptask)
    {
        //1.0 检查驱动是否在线 这里直接使用了读取驱动当前单圈位置作为判断依据
        case CHECK_DRV_ONLINE:
            //1.1 读取电机当前单圈位置 
            if(GetEncoderData_ST(&RMDL240, &ttyUSB0))
            {
                printf("initpos is %d.\n", RMDL240.DrvStatus.Encoder_ST);
               *ptask = SET_DRV_BACK_2_ZERO;
            }
            //1.2 异常处理
            break;
        //2.0 设置电机回零
        case SET_DRV_BACK_2_ZERO:
            //2.1 使用单圈位置模式1发送位置指令0°
            if(ARRIVE_TARPOS == MotorRun2Tarpos_P1ST(&RMDL240, &ttyUSB0,0))
            {
                *ptask =  SET_DRV_2_60DEG;
            }
            //2.2 异常处理    
            break;
        //3.0 设置电机运行到60°
        case SET_DRV_2_60DEG:
            //3.1 使用单圈位置模式1发送位置指令60°
            if(ARRIVE_TARPOS == MotorRun2Tarpos_P1ST(&RMDL240, &ttyUSB0,6000))
            {
                *ptask =  SET_DRV_BACK_2_ZERO;
            }
            //3.2 异常处理
            break;
        default:
            break;
    }
}

/**
  * @brief  使用单圈位置模式1向RMD驱动器发送位置指令并检查执行情况
  * @param  pCmd   指向RMD指令结构体的指针，RMD控制指令的相关信息在此结构体中定义  
  * @param  pCom   指向串口结构体的指针，串口收发信息相关信息在此结构体中定义
  * @param  tarPos 目标位置 16位整型数，单位0.01° 如1000表示10.00°
  * @retval ret    指令执行状态
  *                0 使用单圈位置模式1指令发送指令中
  *                1 发送指令成功 检查当前执行情况（检查当前位置，并判断是否达到目标位置）
  *                2 到达目标位置 
 **/
int8_t MotorRun2Tarpos_P1ST(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom, u_int16_t tarPos)
{
    int8_t  ret = 0;
    static u_int8_t cntrem = 0;
    static int8_t status = SET_TARPOS_P1ST;
    int32_t PosErr = 0;
    switch(status)
    {
        case SET_TARPOS_P1ST:

            if(SetposCmd_P1ST(pCmd, pCom, tarPos))
            {
                status = CHECK_CURRENTPOS;
            }

            break;
        case CHECK_CURRENTPOS:

            if(GetEncoderData_ST(pCmd, pCom))
            {
                PosErr = (int32_t)pCmd->tarPos_SL - (int32_t)pCmd->DrvStatus.Encoder_ST;

                if(PosErr <= 99 && PosErr >= -99)
                {
                    status =  ARRIVE_TARPOS;
                }

            } 
            break;
        case ARRIVE_TARPOS:

            status = SET_TARPOS_P1ST;
            break;
    }

#if DEBUG
    if(cnt != cntrem)
    {
        cntrem = cnt;
        printf("pos process status is %d.\n",status);     
    }   
#endif

    return ret = status;
}

/**
  * @brief  更新单圈目标位置指令到pCmd指向的实体，并在指令正确执行后返回1
  * @param  pCmd   指向RMD指令结构体的指针，RMD控制指令的相关信息在此结构体中定义  
  * @param  pCom   指向串口结构体的指针，串口收发信息相关信息在此结构体中定义
  * @param  tarPos 单圈目标位置 单位0.01°
  * @retval ret 0 指令发送中
  *             1 指令发送成功并收到正确应答 未做异常处理
 **/   
int8_t SetposCmd_P1ST(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom, u_int16_t tarPos)
{
    int8_t ret = 0;

    if(pCmd->CmdTransFinshFlag)
    {
        pCmd->CmdTransFinshFlag = 0;
        return ret = 1;
    }  

    if((pCom->TransStatus == SERIAL_RDY) && (pCmd->CmdTransStatus == RMD_CMD_STATE_IDLE))
    {
        pCmd->CmdCnt += 1;
        pCmd->Cmd    = POSMODE1_ST; 
        pCmd->tarPos_SL = tarPos; 

        if(tarPos < pCmd->DrvStatus.Encoder_ST)
        {
            pCmd->RunDirection = 1;
        }
        else
        {
            pCmd->RunDirection = 0;
        }
         
    }  

    return ret;
}

/**
  * @brief  读取编码器单圈位置，在读取到位置后返回1
  * @param  pCmd   指向RMD指令结构体的指针，RMD控制指令的相关信息在此结构体中定义  
  * @param  pCom   指向串口结构体的指针，串口收发信息相关信息在此结构体中定义
  * @param  tarPos 单圈目标位置 单位0.01°
  * @retval ret 0 指令执行中
  *             1 指令发送成功并收到正确应答 未做异常处理
 **/ 
u_int16_t GetEncoderData_ST(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom)
{
    u_int16_t ret = 0;

    if(pCmd->CmdTransFinshFlag)
    {
        pCmd->CmdTransFinshFlag = 0;
        return ret = 1;
    } 

    if((pCom->TransStatus == SERIAL_RDY) && (pCmd->CmdTransStatus == RMD_CMD_STATE_IDLE))
    {
        pCmd->CmdCnt += 1;
        pCmd->Cmd    = RD_ENCODER_SL;    
    } 

    return ret;
}

