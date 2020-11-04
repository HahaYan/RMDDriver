#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include <unistd.h>
#include"USBSerial.h"
#include"Config.h"
#include"RMDDrvDriver.h"
#include"userTask.h"
#include"usertimer.h"

#define RMDCOM "/dev/ttyUSB0"

void RMDTestFunc1(u_int8_t *buf, u_int8_t *len, u_int16_t Pos);
void RMDTestFunc2(u_int8_t *buf);



int main(int argc, char* argv[])
{
    u_int8_t i;
    //static u_int8_t currentTask = SET_DRV_BACK_2_ZERO;
    static u_int8_t currentTask = CHECK_DRV_ONLINE;

    //1.0 串口初始化
    SerialPortInit(&ttyUSB0);
    //2.0 设置定时任务周期
    timer_init(0, 10000, 0, 500000);     //延时10ms后按100mS周期调用定时任务
    //3.0 主循环任务
    while(1)
    {   
        //3.1 主任务处理 
        UserTaskHandler(&currentTask);
        //3.2 RMD通讯处理
        RMDMessageHandler(&RMDL240, &ttyUSB0);

        if(cnt > 30) break; 
    }

    close(ttyUSB0.fd);

    return 0;
}

void RMDTestFunc1(u_int8_t *buf, u_int8_t *len, u_int16_t Pos)
{
    *buf       = 0x3E;
    *(buf + 1) = 0xA5;
    *(buf + 2) = 0x01;
    *(buf + 3) = 0x04;
    *(buf + 4) = SumCheck_u8(buf, 4);
    *(buf + 5) = 0x00;
    *(buf + 6) = (u_int8_t)Pos;
    *(buf + 7) = (u_int8_t)(Pos>>8);
    *(buf + 8) = 0x00;
    *(buf + 9) = SumCheck_u8(&buf[5],4);

    *len = 10;
}

void RMDTestFunc2(u_int8_t *buf)
{
    *buf       = 0x3E;
    *(buf + 1) = 0x33;
    *(buf + 2) = 0x01;
    *(buf + 3) = 0x00;
    *(buf + 4) = SumCheck_u8(buf, 4);    
}


void COMDataHandel(SerialTypedef *pSerial)
{
 
} 
