#include "./DSHOT/Dshot.h"



/* 周期为1.67us，1670ns */
//184是TIM->ARR
//定时器有点误差周期是1675ns
/*
Dshot600来说，一位信号就大概是1.67微秒，通过对时间的占空比代表0或者1.
如果表示0：高电平占据625纳秒，
如果表示1：高电平占据1250纳秒
*/
int Pulse_Width=184;
int Dshot_0bit=0;
int Dshot_1bit=0;
void Dshot_pwm_set(void)
{


	Dshot_0bit=184*625/1670;
	Dshot_1bit=184*1250/1670;

}

