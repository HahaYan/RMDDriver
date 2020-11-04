#ifndef _RMDDRVDRIVER_H_
#define _RMDDRVDRIVER_H_

typedef enum{
    RMD_CMD_STATE_IDLE = 0x00,
    RMD_CMD_STATE_TX,
    RMD_CMD_STATE_TX_END,
    RMD_CMD_STATE_RX,
    RMD_CMD_STATE_RX_CHECK,
    RMD_CMD_STATE_EXEC,
    RMD_CMD_STATE_ERR,
}Rmd_host_State;

enum RMDCMD{
    RD_ACCSETTING  = 0x33,      //读取加速度设定
    WR_ACCSETTING_2_RAM,        //写加速度设定到RAM
    DRIVE_STANDBY  = 0x80,      //驱动待命()      
    DRIVE_STOP     = 0x81,      //驱动断使能
    DRIVE_EN       = 0x88,      //驱动运行
    RD_ENCODER_RAW = 0x90,      //读取编码器原始角度
    RD_ENCODER_SL  = 0x94,      //读取单圈编码器位置
    SET_ENCODER_ZERO = 0x95,    //设置编码器零点
    RD_DRVSTATE1   = 0x9A,      //读驱动状态1
    RD_DRVSTATE2   = 0x9C,      //读驱动状态2
    RD_DRVSTATE3   = 0x9D,      //读驱动状态3
    POSMODE1_ST    = 0xA5,      //单圈位置闭环模式1
    POSMODE2_ST    = 0xA6,      //单圈位置闭环模式2
};

typedef struct{
    u_int8_t  ErrorState; //错误状态
    int16_t   MotorTemp;  //电机温度
    u_int16_t BusVolt;    //母线电压
    int16_t   Iq;         //转矩电流
    int16_t   Speed;      //电机转速
    u_int16_t Encoder_Raw;//编码器原始位置数据
    u_int16_t Encoder_ST; //单圈编码器位置
    int16_t   Ia;         //A相电流
    int16_t   Ib;         //B相电流
    int16_t   Ic;         //C相电流
}RMDDrvStateInfoTypedef;

typedef struct{
    u_int8_t CmdCnt;
    u_int8_t FinishedCmdCnt;
    u_int8_t CmdTransStatus;
    u_int8_t CmdTransFinshFlag;
    u_int8_t Cmd;
    u_int8_t RunDirection;
    u_int16_t tarPos_SL;
    u_int32_t SpeedLimit;
    RMDDrvStateInfoTypedef DrvStatus;
}RMDDrvCMDTypedeef;

extern RMDDrvCMDTypedeef RMDL240;
void RMDMessageHandler(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom);

#endif