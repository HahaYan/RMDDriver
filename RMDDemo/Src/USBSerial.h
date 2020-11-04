#ifndef _USBSERIAL_H_
#define _USBSERIAL_H_

#define BUFFSIZE 50

enum SerialStatus{
    SERIAL_INIT,
    SERIAL_RDY,
    SERIAL_TRANS_BUSY,
    SERIAL_RCV_BUSY,
    SERIAL_TRANS_OT,
    SERIAL_RCV_OT,
    SERIAL_ERROR,
};

typedef struct{
    u_int8_t buad;
}SerialConfigTypedef;

typedef struct{
    u_int8_t TransBuffer[BUFFSIZE];
    u_int8_t RcvBuffer[BUFFSIZE];
    u_int8_t TransStatus;
    u_int8_t TransBytesCnt;
    u_int8_t TransBytesExp;
    u_int8_t RcvBytesCnt;
    u_int8_t RcvBytesExp;    //期望接收的Byte数 
    int fd;  //文件描述符
    SerialConfigTypedef SerialConfigData;
}SerialTypedef;


extern char *DEV_NAME;
extern SerialTypedef ttyUSB0;

int SerialPortInit(SerialTypedef *pSeial);
int SerialTransmitHandel(SerialTypedef *pSeial);
int SerialRcvHandel(SerialTypedef *pSeial);
int set_opt(int, int, int, char, int);
int open_port(char *uartname);
int8_t SumCheck_u8(u_int8_t *pdata, u_int8_t length);

#endif
