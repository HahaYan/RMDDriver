#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include <unistd.h>
#include"USBSerial.h"
#include"Config.h"
#include"RMDDrvDriver.h"
#include"userTask.h"
#include"usertimer.h"

#define RMDCOM "/dev/ttyUSB0"

int main(int argc, char* argv[])
{
    u_int8_t i;
    //static u_int8_t currentTask = SET_DRV_BACK_2_ZERO;
    static u_int8_t currentTask = CHECK_DRV_ONLINE;

    //1.0 串口初始化
    SerialPortInit(&ttyUSB0);
    //2.0 设置定时任务周期
    timer_init(0, 10000, 0, 500000);     //延时10ms后按500mS周期调用定时任务
    //3.0 主循环任务
    while(1)
    {   
        //3.1 主任务处理 
        UserTaskHandler(&currentTask);
        //3.2 RMD通讯处理
        RMDMessageHandler(&RMDL240, &ttyUSB0);
        //3.3 定时调用函数31次后退出运行
        if(cnt > 30) break; 
    }

    close(ttyUSB0.fd);

    return 0;
}
