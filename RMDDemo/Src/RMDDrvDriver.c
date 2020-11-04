#include<stdio.h>
#include<sys/types.h>
#include"USBSerial.h"
#include"RMDDrvDriver.h"



RMDDrvCMDTypedeef RMDL240;
void UpdateCmdData2TransBuff(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);
void DrvStandbyCmdHanler(SerialTypedef *pCom);
void RDEncoderSTCmdDeal(SerialTypedef *pCom); 
void DrvReplyMessageHandler(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);
void PosMode1STCmdDeal(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);

void RMDMessageHandler(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom)
{
    static u_int8_t TransCntRem = 0;
    switch(pCmd->CmdTransStatus)
    {
        case RMD_CMD_STATE_IDLE:

            if(pCmd->CmdCnt != pCmd->FinishedCmdCnt)
            {
                pCmd->FinishedCmdCnt = pCmd->CmdCnt;
                pCmd->CmdTransStatus = RMD_CMD_STATE_TX;
            }

            break;
        case RMD_CMD_STATE_TX:
            UpdateCmdData2TransBuff(pCmd, pCom); 
            pCom->TransStatus = SERIAL_TRANS_BUSY;   //启动传输
            pCmd->CmdTransStatus = RMD_CMD_STATE_TX_END;
            break;
        case RMD_CMD_STATE_TX_END:

            if(pCom->TransBytesCnt == pCom->TransBytesExp)
            {   
               pCmd->CmdTransStatus = RMD_CMD_STATE_RX; 
               pCom->TransStatus    = SERIAL_RCV_BUSY;   
            }

            break;
        case RMD_CMD_STATE_RX:

            if(pCom->RcvBytesCnt == pCom->RcvBytesExp)
            {
                pCmd->CmdTransStatus = RMD_CMD_STATE_RX_CHECK;
                pCom->RcvBytesCnt = 0;
            }

            break;
        case RMD_CMD_STATE_RX_CHECK:
            
            TransCntRem = SumCheck_u8(pCom->RcvBuffer,4);

            if(pCom->RcvBuffer[4] == TransCntRem)
            {
                pCmd->CmdTransStatus = RMD_CMD_STATE_EXEC;
                pCom->TransStatus = SERIAL_RDY;
            }
            else
            {
                pCmd->CmdTransStatus = RMD_CMD_STATE_ERR;
                pCom->TransStatus = SERIAL_RDY;
            }
            
           
            break;
        case RMD_CMD_STATE_EXEC:

            DrvReplyMessageHandler(pCmd, pCom);
            pCmd->CmdTransStatus = RMD_CMD_STATE_IDLE;
            pCmd->CmdTransFinshFlag = 1;
            break;
        case RMD_CMD_STATE_ERR:

            break;
    }
}

void UpdateCmdData2TransBuff(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom)
{

    *pCom->TransBuffer     = 0x3E;
    *(pCom->TransBuffer+2) = 0x01;
    switch(pCmd->Cmd)
    {
        case DRIVE_STANDBY:
            RDEncoderSTCmdDeal(pCom);
            break;
        case DRIVE_EN:
            break;
        case POSMODE1_ST:
            PosMode1STCmdDeal(pCmd, pCom);
            break;
        case RD_ENCODER_SL:
            RDEncoderSTCmdDeal(pCom); 
            break;
        default:
            break;
    }
}

void PosMode1STCmdDeal(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom)
{
    *(pCom->TransBuffer+1) = POSMODE1_ST;
    *(pCom->TransBuffer+3) = 0x04;
    *(pCom->TransBuffer+4) = SumCheck_u8(pCom->TransBuffer, 4);
    *(pCom->TransBuffer+5) = pCmd->RunDirection;
    *(pCom->TransBuffer+6) = (u_int8_t)pCmd->tarPos_SL;
    *(pCom->TransBuffer+7) = (u_int8_t)(pCmd->tarPos_SL>>8);
    *(pCom->TransBuffer+8) = 0x00;
    *(pCom->TransBuffer+9) = SumCheck_u8(&pCom->TransBuffer[5], 4);
    pCom->TransBytesExp    = 10;
    pCom->RcvBytesExp      = 13;       

}

void RDEncoderSTCmdDeal(SerialTypedef *pCom)
{
    *(pCom->TransBuffer+1) = RD_ENCODER_SL;
    *(pCom->TransBuffer+3) = 0x00;
    *(pCom->TransBuffer+4) = SumCheck_u8(pCom->TransBuffer, 4);
    pCom->TransBytesExp    = 5;
    pCom->RcvBytesExp      = 8;
}

void DrvReplyMessageHandler(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom)
{
    //1.0 检查数据校验和

    switch(pCmd->Cmd)
    {
        case DRIVE_STANDBY:
            break;
        case DRIVE_EN:
            break;
        case POSMODE1_ST:
            break;
        case RD_ENCODER_SL:
            pCmd->DrvStatus.Encoder_ST = ((u_int16_t)pCom->RcvBuffer[6]<<8) + pCom->RcvBuffer[5];            
            break;
        default:
            break;
    } 

}



