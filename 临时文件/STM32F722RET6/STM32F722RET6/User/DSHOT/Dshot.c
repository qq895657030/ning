#include "./DSHOT/Dshot.h"



/* ����Ϊ1.67us��1670ns */
//184��TIM->ARR
//��ʱ���е����������1675ns
/*
Dshot600��˵��һλ�źžʹ����1.67΢�룬ͨ����ʱ���ռ�ձȴ���0����1.
�����ʾ0���ߵ�ƽռ��625���룬
�����ʾ1���ߵ�ƽռ��1250����
*/
int Pulse_Width=184;
int Dshot_0bit=0;
int Dshot_1bit=0;
void Dshot_pwm_set(void)
{


	Dshot_0bit=184*625/1670;
	Dshot_1bit=184*1250/1670;

}

