# RMD驱动函数说明

## 1.0 单圈位置模式1接口函数

```c
/**
  * @brief  使用单圈位置模式1向RMD驱动器发送位置指令并检查执行情况
  *         
  * @param  pCmd   指向RMD指令结构体的指针，RMD控制指令的相关信息在此结构体中定义  
  * @param  pCom   指向串口结构体的指针，串口收发信息相关信息在此结构体中定义
  * @param  tarPos 目标位置 16位整型数，单位0.01° 如1000表示10.00°
  * @retval ret    指令执行状态
  *                0 使用单圈位置模式1指令发送指令中
  *                1 发送指令成功 检查当前执行情况（检查当前位置，并判断是否达到目标位置）
  *                2 到达目标位置 
 **/
int8_t MotorRun2Tarpos_P1ST(RMDDrvCMDTypedeef *pCmd, SerialTypedef *pCom, u_int16_t tarPos)
    
```