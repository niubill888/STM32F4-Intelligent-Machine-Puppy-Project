#include "drv_sg90.h"
 //250   200      150    100    50
 //50   100     150     200    250
uint8_t dog_state[5]={50,100,150,200,250};
//uint8_t oc1=0,oc2=0,oc3=0,oc4=0;//3,4是pb,,,

void dog_forward(void)
{


    
	TIM_SetCompare1(TIM3, dog_state[4]);  // 通道1（PA6）右边前脚180度
	TIM_SetCompare2(TIM3, dog_state[2]);  // 通道2（PA7）右边后脚90
	TIM_SetCompare3(TIM3, dog_state[0]);  // 通道3（PB0）左边前脚180
	TIM_SetCompare4(TIM3, dog_state[2]);  // 通道4（PB1）左边后脚90
	drv_systick_ms(200);
	
	// 步骤2：前腿向前摆（舵机1、2转动到0°收起），带动整体前移
    TIM_SetCompare1(TIM3, dog_state[0]);  // 通道1（PA6）右边前脚0度
    TIM_SetCompare3(TIM3, dog_state[4]);  // 通道2（PA7）右边后脚0度
    // 等待动作完成，可根据实际情况调整延时
	drv_systick_ms(100);


    // 步骤3：后腿抬起（舵机3、4转动到180°伸展），前腿支撑（舵机1、2保持90°）
    TIM_SetCompare2(TIM3, dog_state[4]);  // 通道3（PB0）左边前脚180
    TIM_SetCompare4(TIM3, dog_state[0]);  // 通道4（PB1）左边后脚180
    TIM_SetCompare1(TIM3, dog_state[2]);  // 通道1（PA6）右边前脚90
    TIM_SetCompare3(TIM3, dog_state[2]);  // 通道2（PA7）右边后脚90
    // 等待动作完成，可根据实际情况调整延时
    drv_systick_ms(200);

    // 步骤4：后腿向前摆（舵机3、4转动到0°收起），完成一次循环
    TIM_SetCompare2(TIM3, dog_state[0]);  // 通道3（PB0）左边前脚0度
    TIM_SetCompare4(TIM3, dog_state[4]);  // 通道4（PB1）左边后脚0度
    drv_systick_ms(100);

    
}

void dog_backup(void)
{
	TIM_SetCompare1(TIM3, dog_state[2]);  // 通道1（PA6）右边前脚180度
	TIM_SetCompare2(TIM3, dog_state[0]);  // 通道2（PA7）右边后脚90
	TIM_SetCompare3(TIM3, dog_state[2]);  // 通道3（PB0）左边前脚180
	TIM_SetCompare4(TIM3, dog_state[4]);  // 通道4（PB1）左边后脚90
	drv_systick_ms(200);
	
	// 步骤2：前腿向前摆（舵机1、2转动到0°收起），带动整体前移
    TIM_SetCompare2(TIM3, dog_state[4]);  // 通道1（PA6）右边前脚0度
    TIM_SetCompare4(TIM3, dog_state[0]);  // 通道2（PA7）右边后脚0度
    // 等待动作完成，可根据实际情况调整延时
	drv_systick_ms(100);


    // 步骤3：后腿抬起（舵机3、4转动到180°伸展），前腿支撑（舵机1、2保持90°）
    TIM_SetCompare2(TIM3, dog_state[2]);  // 通道3（PB0）左边前脚180
    TIM_SetCompare4(TIM3, dog_state[2]);  // 通道4（PB1）左边后脚180
    TIM_SetCompare1(TIM3, dog_state[0]);  // 通道1（PA6）右边前脚90
    TIM_SetCompare3(TIM3, dog_state[4]);  // 通道2（PA7）右边后脚90
    // 等待动作完成，可根据实际情况调整延时
    drv_systick_ms(200);

    // 步骤4：后腿向前摆（舵机3、4转动到0°收起），完成一次循环
    TIM_SetCompare1(TIM3, dog_state[4]);  // 通道3（PB0）左边前脚0度
    TIM_SetCompare3(TIM3, dog_state[0]);  // 通道4（PB1）左边后脚0度
    drv_systick_ms(100);

}

void dog_sitdown(void)
{
	TIM_SetCompare1(TIM3, dog_state[4]);  // 通道1（PA6）右边前脚180度
	TIM_SetCompare2(TIM3, dog_state[4]);  // 通道2（PA7）右边后脚90
	TIM_SetCompare3(TIM3, dog_state[0]);  // 通道3（PB0）左边前脚180
	TIM_SetCompare4(TIM3, dog_state[0]);  // 通道4（PB1）左边后脚90
	drv_systick_ms(3000);

}
void dog_stand(void)
{
	TIM_SetCompare1(TIM3, dog_state[2]);  // 通道1（PA6）右边前脚180度
	TIM_SetCompare2(TIM3, dog_state[2]);  // 通道2（PA7）右边后脚90
	TIM_SetCompare3(TIM3, dog_state[2]);  // 通道3（PB0）左边前脚180
	TIM_SetCompare4(TIM3, dog_state[2]);  // 通道4（PB1）左边后脚90
	drv_systick_ms(3000);
}
