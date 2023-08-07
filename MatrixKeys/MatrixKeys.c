#include "MatrixKeys.h"
#include  "stm32f1xx.h"
#include "system_stm32f1xx.h"

// 定义状态记录,记录所有的
static uint16_t key_state = 0xffff;
/*************第一行*************/
//第一行第一列（0，0），1向右移动0（0*4+0）
//第一行第二列（0，1），1向右移动1位（0*4+1）
//第一行第三行（0，3），1向右移动1位（0*4+1）
//第一行第四行（0，4），1向右移动1位（0*4+1）
/*.......*/
/*************第N行*************/
//第一行第一列（N-1，0），1向右移动0（N-1*4+0）
//第一行第二行（N-1，1），1向右移动1位（N-1*4+1）
//第一行第三行（N-1，3），1向右移动1位（N-1*4+1）
//第一行第四行（N-1，4），1向右移动1位（N-1*4+1）
#define IS_KEY_UP(row, col) 	((key_state >> (row * ROW_NUM + col)) & 0x01) == KEY_UP
#define IS_KEY_DOWN(row, col) ((key_state >> (row * ROW_NUM + col)) & 0x01) == KEY_DOWN
//高电平按下
//#define SET_KEY_UP(row, col)		key_state &= ~(1 << (row * ROW_NUM + col))
//#define SET_KEY_DOWN(row, col)	key_state |= (1 << (row * ROW_NUM + col))


////低电平按下
#define SET_KEY_UP(row, col)		key_state |= (1 << (row * ROW_NUM + col))
#define SET_KEY_DOWN(row, col)	key_state &= ~(1 << (row * ROW_NUM + col))

void KEY_INIT(){
    // ROW 推挽输出
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = ROW1_PIN|ROW2_PIN|ROW3_PIN|ROW4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ROW_GPIO, &GPIO_InitStruct);


	  // COL 列设置下拉输入
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = COL1_PIN|COL2_PIN|COL3_PIN|COL4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull=GPIO_PULLUP;

    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(COL_GPIO, &GPIO_InitStruct);

   // HAL_GPIO_WritePin(COL_GPIO, COL1_PIN|COL2_PIN|COL3_PIN|COL4_PIN, GPIO_PIN_SET);
}


static uint8_t COL_STATE(uint8_t col) {
    if(col == 0) return COL1;
    if(col == 1) return COL2;
    if(col == 2) return COL3;
    if(col == 3) return COL4;
}

static void ROW_OUT(uint8_t row) {
    if(row == 0) {SET_ROW1_LOW; SET_ROW2_HIGH; SET_ROW3_HIGH; SET_ROW4_HIGH;}
    if(row == 1) {SET_ROW1_HIGH; SET_ROW2_LOW; SET_ROW3_HIGH; SET_ROW4_HIGH;}
    if(row == 2) {SET_ROW1_HIGH; SET_ROW2_HIGH; SET_ROW3_LOW; SET_ROW4_HIGH;}
    if(row == 3) {SET_ROW1_HIGH; SET_ROW2_HIGH; SET_ROW3_HIGH; SET_ROW4_LOW;}
}

void MK_init() {
    KEY_INIT();
}
void MK_scan(uint16_t *state) {
    uint8_t i, j;
    for(j = 0; j < ROW_NUM; j++) {
        // j: row
        // i: col
        // 只让row1工作
        ROW_OUT(j);
        for(i = 0; i < COL_NUM; i++) {
            // 列
            // 判断某个列是否抬起或者按下
            // i就是某个列
            // row, col
            if(COL_STATE(i) == KEY_DOWN && IS_KEY_UP(j, i)) {
                // 按键如果是按下， 并且上一次是高电平，电平应该是低
                // 记录状态
                SET_KEY_DOWN(j, i);							
								MK_on_keydown(j, i);
            } else if(COL_STATE(i) == KEY_UP && IS_KEY_DOWN(j, i)) {
                // 按键如果是抬起，并且上一次是低电平, 电平应该是高
                // 记录状态
                SET_KEY_UP(j, i);							
								MK_on_keyup(j, i);
            }
        }
    }
		*state = key_state;
}
