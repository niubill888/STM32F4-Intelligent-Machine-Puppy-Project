#include "drv_systick.h"

// 静态变量：用于存储SysTick定时器的毫秒/微秒计数基准
static uint16_t fs_ms = 0;    // 1ms对应的SysTick加载值
static uint16_t fs_us = 0;    // 1us对应的SysTick加载值
volatile uint32_t tick_cnt = 0;  // 系统滴答计数（毫秒级，volatile确保多任务可见）


/**
 * @brief 初始化SysTick定时器
 * @note  配置SysTick为8分频，计算1us和1ms对应的加载值
 *        用于后续的us级和ms级延时函数
 * @param systick：系统时钟频率（单位：MHz，如168表示168MHz）
 */
void drv_systick_init(uint32_t systick)
{
    SysTick->CTRL &= ~(0x05);  // 关闭SysTick定时器（清除第0位使能，第2位选择外部时钟）
    fs_us = systick / 8;       // 8分频后，1us对应的计数次数（systick MHz / 8 = 计数/MHz → 1us计数=systick/8）
    fs_ms = fs_us * 1000;      // 1ms = 1000us，计算1ms对应的计数次数
}


/**
 * @brief 毫秒级延时函数
 * @note  由于SysTick是24位定时器，最大单次延时约800ms（取决于fs_ms）
 *        超过800ms时自动分段延时，避免计数溢出
 * @param ms：需要延时的毫秒数（无上限，内部自动分段）
 */
void drv_systick_ms(uint16_t ms)
{
    uint32_t temp = 0;         // 临时变量，用于读取SysTick状态
    uint8_t systick_flg = 0;   // 分段延时标志（0：继续分段，1：最后一段）

    while (systick_flg == 0)
    {
        if (ms > 798)  // 单次最大延时798ms（预留余量，避免24位计数器溢出）
        {
            SysTick->LOAD = 798 * fs_ms;  // 加载798ms对应的计数值
            ms -= 798;                    // 剩余延时时间减少798ms
        }
        else
        {
            SysTick->LOAD = ms * fs_ms;   // 加载剩余延时对应的计数值
            systick_flg = 1;              // 标记为最后一段，退出循环
        }

        SysTick->VAL = 0;                 // 清空当前计数寄存器（避免残留值影响延时精度）
        SysTick->CTRL |= 0x01;            // 开启SysTick定时器

        // 等待定时完成（检查CTRL寄存器的第16位：计数到0标志）
        do
        {
            temp = SysTick->CTRL;
        } while (!(temp & (0x01 << 16)));

        SysTick->CTRL &= ~0x01;  // 关闭SysTick定时器（避免影响后续操作）
    }
}


/**
 * @brief 微秒级延时函数
 * @note  适用于短延时场景，单次最大延时取决于fs_us（24位计数器上限）
 * @param us：需要延时的微秒数（建议不超过800000us，即800ms）
 */
void drv_systick_us(uint16_t us)
{
    uint32_t temp = 0;         // 临时变量，用于读取SysTick状态

    SysTick->LOAD = us * fs_us;  // 加载us对应的计数值
    SysTick->VAL = 0;            // 清空当前计数寄存器
    SysTick->CTRL |= 0x01;       // 开启SysTick定时器

    // 等待定时完成（检查CTRL寄存器的第16位：计数到0标志）
    do
    {
        temp = SysTick->CTRL;
    } while (!(temp & (0x01 << 16)));

    SysTick->CTRL &= ~(0x01);  // 关闭SysTick定时器
}


/**
 * @brief 获取系统运行的毫秒数
 * @note  需配合SysTick中断使用（中断服务程序中递增tick_cnt）
 * @return 当前系统运行的总毫秒数（uint32_t类型，溢出后从0重新计数）
 */
uint32_t GetTick(void) {
    return tick_cnt;
}