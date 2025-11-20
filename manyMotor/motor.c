#include "motor.h"

// 4つ分のモーターを定義
MotorS motors[MOTOR_NUM] = {
    { .dirPin = 1,.pwmPin = 5,.drivePin1 = 9,.drivePin2 = 13,.currntState = STOP,.currntduty = 0},
    { .dirPin = 2,.pwmPin = 6,.drivePin1 = 10,.drivePin2 = 14,.currntState = STOP,.currntduty = 0},
    { .dirPin = 3,.pwmPin = 7,.drivePin1 = 11,.drivePin2 = 15,.currntState = STOP,.currntduty = 0},
    { .dirPin = 4,.pwmPin = 8,.drivePin1 = 12,.drivePin2 = 16,.currntState = STOP,.currntduty = 0},
};

//　モーターの初期化
void initMTR(){
    for(int i = 0;i < MOTOR_NUM;i++){
        pinMode(motors[i].dirPin, OUTPUT);
        pinMode(motors[i].pwmPin, OUTPUT);
        pinMode(motors[i].drivePin1, OUTPUT);
        pinMode(motors[i].drivePin2, OUTPUT);

        // 状態テーブル
        motors[i].stateProcs[STOP]     = motorStop;
        motors[i].stateProcs[FORWARD]  = motorForward;
        motors[i].stateProcs[BACKWARD] = motorBackward;
    }
}

// モーター駆動処理
bool motorDrive(MTRTYPE motortype,MTRState wantstate){
    motors[motortype].stateProcs[wantstate](&motors[motortype]);

    // 状態を更新
    motors[motortype].currntState = wantstate;
    
    // 戻り値を返す
    return true;

}