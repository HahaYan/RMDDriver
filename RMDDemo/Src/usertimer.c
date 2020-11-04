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

void signal_timer_handler(int signo)
{
    switch (signo){
        case SIGALRM:
            cnt++;
            printf("Call cnt is %d.\n", cnt);
            COMWRHandler();
            break;
   }
}



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