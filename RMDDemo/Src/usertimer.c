#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<signal.h>
#include<sys/time.h>
#include"usertimer.h"
#include"USBSerial.h"
#include"RMDDrvDriver.h"

u_int8_t cnt;
void signal_timer_handler(int signo);
void COMWRHandler(void);

/**
  * @brief  初始化定时器和信号量,四个输入参数分别用来设定初始化后的延时时间和调用周期
  * @param  val_sec  单位sec  初始化后延时开始周期任务的时间s
  * @param  val_usec 单位usec 初始化后延时开始周期任务的时间us
  * @param  intval_sec  单位sec 调用周期s
  * @param  intval_usec  单位sec 调用周期us
  * @retval void
 **/
void timer_init(long val_sec, long val_usec, long intval_sec, long intval_usec)
{
    struct itimerval new_value, old_value;

    signal(SIGALRM, signal_timer_handler);
    new_value.it_value.tv_sec  = val_sec;
    new_value.it_value.tv_usec = val_usec;
    new_value.it_interval.tv_sec  = intval_sec;
    new_value.it_interval.tv_usec = intval_usec;
    setitimer(ITIMER_REAL, &new_value, &old_value);
}

/**
  * @brief  周期信号handler，周期任务放置在对应的信号量下，当前只使用了SIGALRM信号
  * @param  signo 时间信号  
  * @retval void
 **/
void signal_timer_handler(int signo)
{
    switch (signo){
        case SIGALRM:
            cnt++;
            COMWRHandler();
            break;
   }
}

/**
  * @brief  串口读写处理 根据串口当前状态(SERIAL_RDY /SERIAL_TRANS_BUSY/ SERIAL_RCV_BUSY)进行相应操作
  * @param  void  
  * @retval void
 **/
void COMWRHandler(void)
{
    u_int8_t len = 0;
     
    switch(ttyUSB0.TransStatus)
    {
        case SERIAL_RDY:
            //空闲状态  
            break;
        case SERIAL_TRANS_BUSY:

            printf("sendbytes is %d.\n", ttyUSB0.TransBytesExp);
            ttyUSB0.TransBytesCnt = SerialTransmitHandel(&ttyUSB0);

            break;
        case SERIAL_RCV_BUSY:
            
            ttyUSB0.RcvBytesCnt = SerialRcvHandel(&ttyUSB0);

            if(ttyUSB0.RcvBytesCnt > 0)
            {
                printf("Drv reply %d bytes.\n", ttyUSB0.RcvBytesCnt);   
            }
            else
            {
                printf("Din't get reply.\n");
            }
            
            break;
    }
}