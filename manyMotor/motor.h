#include <stdint.h>
#include <stdbool.h>

#define MOTOR_NUM 4
// モーターの状態列挙体
typedef enum COND{
    STOP,
    FORWARD,
    BACKWARF
}MTRState;

// モーターのタイプ
typedef enum MTRV{
    SLIDE,
    REC,
    HIGH,
    ANY
}MTRTYPE;

// モーター構造体
typedef struct{
    const uint8_t dirPin;
    const uint8_t pwmPin;
    const uint8_t drivePin1;
    const uint8_t drivePin2;

    // 現在の状態
    MTRState currntState;

    // 重要なのはここ。状態と関数を列挙体を用いて紐づけてあげる
    void (*stateProcs[3])(struct MotorS *motor);

    // 現在の速度
    uint8_t currntduty;
} MotorS;

// 公開関数
void initMTR();
bool motorDrive(MTRTYPE motortype,MTRState wantstate);