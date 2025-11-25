#include <stdio.h>
#include <stdlib.h>

#define MAX_OBSERVERS 10

typedef struct {
    void (*update)(float temperature);
} TempObserver;

typedef struct {
    TempObserver *observers[MAX_OBSERVERS];
    int observer_count;
    float current_temp;
} TempSensor;

// Observer 1: 警報システム
void alarm_system(float temp) {
    if (temp > 80.0f) {
        printf("[ALARM] 高温警報! 温度: %.1f℃\n", temp);
    }
}

// Observer 2: ファン制御
void fan_controller(float temp) {
    if (temp > 60.0f) {
        printf("[FAN] ファン速度UP: %.1f℃\n", temp);
    } else if (temp < 40.0f) {
        printf("[FAN] ファン停止\n");
    }
}

// Observer 3: ログ記録
void data_logger(float temp) {
    printf("[LOG] 温度記録: %.1f℃\n", temp);
}

void attach_observer(TempSensor *sensor, TempObserver *observer) {
    if (sensor->observer_count < MAX_OBSERVERS) {
        sensor->observers[sensor->observer_count++] = observer;
    }
}

void notify_observers(TempSensor *sensor) {
    for (int i = 0; i < sensor->observer_count; i++) {
        sensor->observers[i]->update(sensor->current_temp);
    }
}

// センサー値更新（メインループや割り込みから呼ばれる）
void update_temperature(TempSensor *sensor, float new_temp) {
    sensor->current_temp = new_temp;
    notify_observers(sensor);
}

int main() {
    TempSensor sensor = {.observer_count = 0};
    
    TempObserver alarm = {.update = alarm_system};
    TempObserver fan = {.update = fan_controller};
    TempObserver logger = {.update = data_logger};
    
    attach_observer(&sensor, &alarm);
    attach_observer(&sensor, &fan);
    attach_observer(&sensor, &logger);
    
    // センサー値のシミュレーション
    printf("=== 温度: 30℃ ===\n");
    update_temperature(&sensor, 30.0f);
    
    printf("\n=== 温度: 65℃ ===\n");
    update_temperature(&sensor, 65.0f);
    
    printf("\n=== 温度: 85℃ ===\n");
    update_temperature(&sensor, 85.0f);
    
    return 0;
}