// 配列操作の練習
#include <stdint.h>
#include <stdio.h>

#define ROW 10
#define COLUM 10

int main(){
    // 格納方法例(C99のみ対応)
    const uint8_t array[ROW][COLUM] = {
        [0][1] = 1,
    };

    // 出力
    for(int i = 0; i < COLUM;i++){
        for(int j = 0; j < ROW;j++){
            printf("%d\n",array[i][j]);
        }
    }
}