#ifndef _USERTASK_H_
#define _USERTASK_H_

enum MainTask{
    CHECK_DRV_ONLINE = 0x00,
    SET_DRV_BACK_2_ZERO,
    SET_DRV_2_60DEG,
};

void UserTaskHandler(u_int8_t *ptask);
#endif
