#include<stdio.h>
#include<sys/types.h>
#include"userTask.h"
#include"USBSerial.h"
#include"RMDDrvDriver.h"

int8_t CheckDriverStatus(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);
u_int16_t GetEncoderData_ST(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);
int8_t DrvReturn2Zero(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);
int8_t SetposCmd_P1ST(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom, u_int16_t tarPos);


extern  u_int8_t cnt;
void UserTaskHandler(u_int8_t *ptask)
{
    int8_t tmp = 0;

    switch(*ptask)
    {
        case CHECK_DRV_ONLINE:
            if(GetEncoderData_ST(&RMDL240, &ttyUSB0))
            {
                printf("initpos is %d.\n", RMDL240.DrvStatus.Encoder_ST);
               *ptask = SET_DRV_BACK_2_ZERO;
            }
            break;
        case SET_DRV_BACK_2_ZERO:

            if(MotorRun2Tarpos_P1ST(&RMDL240, &ttyUSB0,18000))
            {
                printf("debug1213.\n");
                *ptask =  SET_DRV_2_60DEG;
            }

            break;
        case SET_DRV_2_60DEG:
            
            if(MotorRun2Tarpos_P1ST(&RMDL240, &ttyUSB0,0))
            {
                printf("debug2333.\n");
                *ptask =  SET_DRV_BACK_2_ZERO;
            }
            break;
        default:
            break;
    }
}

enum DrvRunStatusOnP1ST{
    SET_TARPOS_P1ST = 0x00,
    CHECK_CURRENTPOS,
    ARRIVE_TARPOS,
};

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
                printf("tarpos is %d.\n", pCmd->tarPos_SL);
                status = CHECK_CURRENTPOS;
            }

            break;
        case CHECK_CURRENTPOS:

            if(GetEncoderData_ST(pCmd, pCom))
            {
                PosErr = (int32_t)pCmd->tarPos_SL - (int32_t)pCmd->DrvStatus.Encoder_ST;

                if(PosErr <= 99 && PosErr >= -99)
                {
                    printf("Current deg is %d.\n", pCmd->DrvStatus.Encoder_ST);
                    status =  ARRIVE_TARPOS;
                }
  
                if(cnt != cntrem)
                {
                    printf("Current pos is %d.\n", pCmd->DrvStatus.Encoder_ST);
                }      
            } 
            break;
        case ARRIVE_TARPOS:

            status = SET_TARPOS_P1ST;
            ret = 1;
            break;
    }

    return ret;
}
enum ReturnZeroStatus{
    SET_TARPOS_0 = 0x00,
    CHECK_POS,
    RETURN_2_0,
};

int8_t DrvReturn2Zero(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom)
{
    int8_t ret = 0;
    static int8_t status = SET_TARPOS_0;

    switch(status)
    {
        case SET_TARPOS_0:

            if(SetposCmd_P1ST(pCmd, pCom, 0))
            {
                status = CHECK_POS;       
            }
            break;
        case CHECK_POS:

            if(GetEncoderData_ST(pCmd, pCom))
            {
                if(pCmd->tarPos_SL <= 99 || pCmd->tarPos_SL >= 35900)
                {
                    printf("Return to zero succeeded!\n");
                    status = RETURN_2_0;
                }
            
            } 

            break;
        case RETURN_2_0:
            status = SET_TARPOS_0;
            ret  = 1;
            break;
    }

    return ret;
}

  
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

        printf("cmd cnt is %d.\n", pCmd->CmdCnt);
    } 

    return ret;
}

