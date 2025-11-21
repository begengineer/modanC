typedef struct {
    void (*enter)(void);      // ① その状態に入った時に1回だけ実行
    void (*update)(void);     // ② その状態の間、毎回実行（オプション）
    void (*exit)(void);       // ③ その状態から出る時に1回だけ実行（オプション）
    RobotState nextState;     // ④ 次に遷移する状態
    uint32_t duration;        // ⑤ この状態を何ミリ秒続けるか
} StateData;

// 状態の列挙
typedef enum {
    STATE_INIT,
    STATE_FORWARD,
    STATE_TURN,
    STATE_STOP,
    STATE_MAX
} RobotState;

// 各状態で実行する関数を定義
void forwardEnter(void) {
    Serial.println("前進開始");
    robotForward(100);
}

void turnEnter(void) {
    Serial.println("旋回開始");
    robotTurnRight(80);
}

void stopEnter(void) {
    Serial.println("停止");
    robotStop();
}

// ★状態テーブル：すべての状態の情報をまとめたもの
StateData stateTable[STATE_MAX] = {
    // enter関数, update関数, exit関数, 次の状態, 持続時間
    [STATE_INIT]    = {NULL,         NULL, NULL, STATE_FORWARD, 0},      // すぐ次へ
    [STATE_FORWARD] = {forwardEnter, NULL, NULL, STATE_TURN,    2000},   // 2秒後にSTATE_TURNへ
    [STATE_TURN]    = {turnEnter,    NULL, NULL, STATE_STOP,    1000},   // 1秒後にSTATE_STOPへ
    [STATE_STOP]    = {stopEnter,    NULL, NULL, STATE_STOP,    0},      // 終了（自分自身に遷移）
};

RobotState currentState = STATE_INIT;  // 現在の状態
uint32_t stateStartTime = 0;           // 状態が始まった時刻

void updateStateMachine(void) {
    // ① 現在の状態データを取得
    StateData *state = &stateTable[currentState];
    
    // ② この状態に入ってからの経過時間を計算
    uint32_t elapsed = millis() - stateStartTime;
    
    // ③ 指定時間が経過したか？
    if(state->duration > 0 && elapsed >= state->duration) {
        
        // --- 状態遷移が発生 ---
        
        // ④ 現在の状態から出る処理（あれば）
        if(state->exit != NULL) {
            state->exit();
        }
        
        // ⑤ 次の状態へ切り替え
        currentState = state->nextState;
        stateStartTime = millis();  // 新しい状態の開始時刻を記録
        
        // ⑥ 新しい状態に入る処理（あれば）
        if(stateTable[currentState].enter != NULL) {
            stateTable[currentState].enter();
        }
    }
    
    // ⑦ 現在の状態の実行中処理（あれば）
    if(state->update != NULL) {
        state->update();
    }
}

void setup() {
    Serial.begin(9600);
    initMTR();
    
    // 初期状態の開始
    stateStartTime = millis();
    
    // 初期状態のenter処理を実行
    if(stateTable[STATE_INIT].enter != NULL) {
        stateTable[STATE_INIT].enter();
    }
}

void loop() {
    updateStateMachine();
}