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
int Dshot_0bit=184*625/1670;;
int Dshot_1bit=184*1250/1670;
void Dshot_pwm_set(void)
{



}


/* DSHOT */



#define MAX_TABLE 18 * 4
/* SAL_CH1~SAL_CH4 */
uint32_t Buffer_1[MAX_TABLE] = {0};
/* SAL_CH5~SAL_CH8 */
uint32_t Buffer_2[MAX_TABLE] = {0};
/*
0：高电平大概占据625ns，
1：高电平大概占据1250ns，
一个bit周期大概为1.67us。
（有一些误差是可以接受的）*/

#define ESC_CMD_BUFFER_LEN 18
#define ESC_BIT_0          Dshot_0bit   //51
#define ESC_BIT_1          Dshot_1bit //104 

uint16_t add_checksum_and_telemetry(uint16_t packet)
{
    uint16_t packet_telemetry = (packet << 1) | 0;
    uint8_t  i;
    int      csum      = 0;
    int      csum_data = packet_telemetry;

    for (i = 0; i < 3; i++)
    {
        csum ^= csum_data; // xor data by nibbles
        csum_data >>= 4;
    }
    csum &= 0xf;
    packet_telemetry = (packet_telemetry << 4) | csum;

    return packet_telemetry; // append checksum
}

void pwmWriteDigital(uint32_t *esc_cmd, uint16_t value, uint16_t channel)
{
    value                     = ((value > 2047) ? 2047 : value);
    value                     = add_checksum_and_telemetry(value);
    esc_cmd[channel + 0 * 4]  = (value & 0x8000) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 1 * 4]  = (value & 0x4000) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 2 * 4]  = (value & 0x2000) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 3 * 4]  = (value & 0x1000) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 4 * 4]  = (value & 0x0800) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 5 * 4]  = (value & 0x0400) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 6 * 4]  = (value & 0x0200) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 7 * 4]  = (value & 0x0100) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 8 * 4]  = (value & 0x0080) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 9 * 4]  = (value & 0x0040) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 10 * 4] = (value & 0x0020) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 11 * 4] = (value & 0x0010) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 12 * 4] = (value & 0x8) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 13 * 4] = (value & 0x4) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 14 * 4] = (value & 0x2) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[channel + 15 * 4] = (value & 0x1) ? ESC_BIT_1 : ESC_BIT_0;
}




uint16_t add_checksum_and_telemetry_one(uint16_t packet, uint8_t telem)
{
    uint16_t packet_telemetry = (packet << 1) | (telem & 1);
    uint8_t i;
    uint16_t csum = 0;
    uint16_t csum_data = packet_telemetry;

    for (i = 0; i < 3; i++) {
        csum ^=  csum_data;   // xor data by nibbles
        csum_data >>= 4;
    }
    csum &= 0xf;
    return (packet_telemetry << 4) | csum;    //append checksum
}



void pwmWriteDigital_one(uint32_t *esc_cmd, uint16_t value)
{
	value = ( (value > 2047) ? 2047 : value );
	value = add_checksum_and_telemetry_one(value, 0);
    esc_cmd[0]  = (value & 0x8000) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[1]  = (value & 0x4000) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[2]  = (value & 0x2000) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[3]  = (value & 0x1000) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[4]  = (value & 0x0800) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[5]  = (value & 0x0400) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[6]  = (value & 0x0200) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[7]  = (value & 0x0100) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[8]  = (value & 0x0080) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[9]  = (value & 0x0040) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[10] = (value & 0x0020) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[11] = (value & 0x0010) ? ESC_BIT_1 : ESC_BIT_0; 	
    esc_cmd[12] = (value & 0x8) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[13] = (value & 0x4) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[14] = (value & 0x2) ? ESC_BIT_1 : ESC_BIT_0;
    esc_cmd[15] = (value & 0x1) ? ESC_BIT_1 : ESC_BIT_0;

    esc_cmd[16] = 0;
    esc_cmd[17] =	0;
}





void Set_Dshot_data(void)
{
pwmWriteDigital(Buffer_1, 1500, 1);

}
 


