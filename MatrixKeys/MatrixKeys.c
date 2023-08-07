#include "MatrixKeys.h"
#include  "stm32f1xx.h"
#include "system_stm32f1xx.h"

// ����״̬��¼,��¼���е�
static uint16_t key_state = 0xffff;
/*************��һ��*************/
//��һ�е�һ�У�0��0����1�����ƶ�0��0*4+0��
//��һ�еڶ��У�0��1����1�����ƶ�1λ��0*4+1��
//��һ�е����У�0��3����1�����ƶ�1λ��0*4+1��
//��һ�е����У�0��4����1�����ƶ�1λ��0*4+1��
/*.......*/
/*************��N��*************/
//��һ�е�һ�У�N-1��0����1�����ƶ�0��N-1*4+0��
//��һ�еڶ��У�N-1��1����1�����ƶ�1λ��N-1*4+1��
//��һ�е����У�N-1��3����1�����ƶ�1λ��N-1*4+1��
//��һ�е����У�N-1��4����1�����ƶ�1λ��N-1*4+1��
#define IS_KEY_UP(row, col) 	((key_state >> (row * ROW_NUM + col)) & 0x01) == KEY_UP
#define IS_KEY_DOWN(row, col) ((key_state >> (row * ROW_NUM + col)) & 0x01) == KEY_DOWN
//�ߵ�ƽ����
//#define SET_KEY_UP(row, col)		key_state &= ~(1 << (row * ROW_NUM + col))
//#define SET_KEY_DOWN(row, col)	key_state |= (1 << (row * ROW_NUM + col))


////�͵�ƽ����
#define SET_KEY_UP(row, col)		key_state |= (1 << (row * ROW_NUM + col))
#define SET_KEY_DOWN(row, col)	key_state &= ~(1 << (row * ROW_NUM + col))

void KEY_INIT(){
    // ROW �������
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = ROW1_PIN|ROW2_PIN|ROW3_PIN|ROW4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ROW_GPIO, &GPIO_InitStruct);


	  // COL ��������������
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
        // ֻ��row1����
        ROW_OUT(j);
        for(i = 0; i < COL_NUM; i++) {
            // ��
            // �ж�ĳ�����Ƿ�̧����߰���
            // i����ĳ����
            // row, col
            if(COL_STATE(i) == KEY_DOWN && IS_KEY_UP(j, i)) {
                // ��������ǰ��£� ������һ���Ǹߵ�ƽ����ƽӦ���ǵ�
                // ��¼״̬
                SET_KEY_DOWN(j, i);							
								MK_on_keydown(j, i);
            } else if(COL_STATE(i) == KEY_UP && IS_KEY_DOWN(j, i)) {
                // ���������̧�𣬲�����һ���ǵ͵�ƽ, ��ƽӦ���Ǹ�
                // ��¼״̬
                SET_KEY_UP(j, i);							
								MK_on_keyup(j, i);
            }
        }
    }
		*state = key_state;
}
