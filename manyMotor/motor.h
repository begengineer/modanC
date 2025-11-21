#include <stdint.h>
#include <stdbool.h>

#define MOTOR_NUM 4
// モーターの状態列挙体
typedef enum COND{
    STOP,
    FORWARD,
    BACKWARD
}MTRState;

// モーターのタイプ
typedef enum MTRV{
    A,
    B,
    C,
    D
}MTRTYPE;

// 公開関数
void initMTR();
bool motorDrive(MTRTYPE motortype,MTRState wantstate);