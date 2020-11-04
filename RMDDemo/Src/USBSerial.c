#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include"USBSerial.h"
#include"Config.h"

SerialTypedef ttyUSB0;
char *DEV_NAME  =  "/dev/ttyUSB0";
int set_opt(int, int, int, char, int);
int8_t SumCheck_u8(u_int8_t *pdata, u_int8_t length);

int SerialPortInit(SerialTypedef *pSeial)
{
    int ret = 0, tmp = -1;
    
    pSeial->fd = open_port(DEV_NAME);

    //1.0检查串口是否成功开启
    if(pSeial->fd < 0)
    {
        ret = -1;
    }
    else
    {
        //配置串口参数
        tmp = set_opt(pSeial->fd, 115200, 8,'N', 1);

        if(tmp < 0)
        {
            perror("Setting serial failed!\n");
            ret = -1;
        }
        else
        {
            pSeial->TransStatus = SERIAL_RDY;
            printf("Serial init success!\n");
        } 
    }

    return ret;
}

int SerialTransmitHandel(SerialTypedef *pSeial)
{
    int ret = 0;

    ret = write(pSeial->fd, pSeial->TransBuffer, pSeial->TransBytesExp);

    return ret;
}

int SerialRcvHandel(SerialTypedef *pSeial)
{
    int ret = 0;

    ret = read(pSeial->fd, &pSeial->RcvBuffer, BUFFSIZE);

    if(ret)
    {
        pSeial->RcvBytesCnt = ret;  
    }

    return ret;
    
}

int open_port(char *uartname)
{
    int fd = open(uartname, O_RDWR|O_NOCTTY|O_NONBLOCK);  //)_NOCTTY 打开的文件是终端设备时，不会将该终端机当做当做进程控制终端

    if(-1 == fd)
    {
        perror("Can't Open Serial Port!");
        return(-1);
    }

    //恢复串口为阻塞状态
    if(fcntl(fd, F_SETFL, 0)<0)
    {
        printf("fcntl failed!\n");
    }else{
        printf("fcntl =%d\n", fcntl(fd, F_SETFL, 0));
    }

    //测试是否为终端设备
    if(isatty(STDIN_FILENO) == 0)
    {
        printf("standard input is not a terminal device.\n");
    }else{
        printf("isatty success!\n");
    }

    printf("fd-open=%d\n", fd);

    return fd;
}


int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;

    if(tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch(nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
    }

    switch(nEvent)
    {
        case 'O':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= ~PARODD;
            break;
        case 'N':
            newtio.c_cflag &= ~PARENB;
            break;
    }

    switch(nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
            break;
        case 921600:
            cfsetispeed(&newtio, B921600);
            cfsetospeed(&newtio, B921600);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600); 
            break;                      
    }

    if(1 == nStop)
    {
        newtio.c_cflag &= ~CSTOPB;
    }else if(2 == nStop)
    {
        newtio.c_cflag |= CSTOPB;
    }

    newtio.c_cc[VTIME] = 0;    //立即返回  
    newtio.c_cc[VMIN]  = 0;    

    tcflush(fd, TCIFLUSH);

    if((tcsetattr(fd,TCSANOW, &newtio)) != 0)
    {
        perror("com set error!\n\r");
        return -1;
    }

    return 0;
}

int8_t SumCheck_u8(u_int8_t *pdata, u_int8_t length)
{
    int8_t ret = 0;
    u_int8_t i = 0;

    for(i=0;i<length;i++)
    {
        ret += *(pdata + i);
    }
    
    return ret;
}
