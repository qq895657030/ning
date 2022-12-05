 #include "cli/test_feedback.h"
#include "drivers/sensor.h"
#include "drivers/accgyro/accgyro.h"
#include "sensors/gyro.h"
uint16_t g_UartSum_u16 = 0;
uint8_t  g_UartScopeArr[10] = {0};

void UartView(int16_t view1,int16_t view2,int16_t view3,int16_t view4)
{
	static uint8_t  l_UartScopeCnt_u8 = 0;
	
	if(l_UartScopeCnt_u8 < 8)
    g_UartSum_u16 += g_UartScopeArr[l_UartScopeCnt_u8];	
	else if(l_UartScopeCnt_u8 == 8)
    g_UartScopeArr[8] = (uint8_t)g_UartSum_u16;
  
	UART4->DR = g_UartScopeArr[l_UartScopeCnt_u8];  
		
	if(++l_UartScopeCnt_u8 >= 9)
	{					
		l_UartScopeCnt_u8 = 0;	
		g_UartSum_u16 = 0;
		g_UartScopeArr[0] = (uint8_t)(view1);  g_UartScopeArr[1] = (uint8_t)(view1>>8);	
		g_UartScopeArr[2] = (uint8_t)(view2);  g_UartScopeArr[3] = (uint8_t)(view2>>8);	
		g_UartScopeArr[4] = (uint8_t)(view3);  g_UartScopeArr[5] = (uint8_t)(view3>>8);	
		g_UartScopeArr[6] = (uint8_t)(view4);  g_UartScopeArr[7] = (uint8_t)(view4>>8);	 	
	}
}
extern float rawSetpoint[3];//[XYZ_AXIS_COUNT];
extern float rcCommand[4];
extern float setpointRate[3], rcDeflection[3], rcDeflectionAbs[3];
extern gyro_t gyro;
extern int16_t servo[8];
extern float FAST_DATA_ZERO_INIT motor[4];
void TestData_Fdb_Func(void)
{
    	//UartView(setpointRate[0]*10,setpointRate[1]*10,setpointRate[2]*10,44);
   	// UartView(setpointRate[0]*10,setpointRate[1]*10,rawSetpoint[0]*10,rawSetpoint[1]*10);

   	// UartView(gyro.imuGyroFilter[0].state*10,gyro.imuGyroFilter[1].state*10*10,gyro.imuGyroFilter[2].state*10,rawSetpoint[1]*10);
	   	UartView(motor[0],motor[1],motor[2],motor[3]);
}