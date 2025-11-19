#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ベースのインターフェース
typedef struct 
{
    int (*init)(void *self);
    int (*send)(void *self,const uint8_t *buf,int len);
    int (*recv)(void *self,uint8_t *buf,int len);
}ComIF;

// UARTドライバ
typedef struct{
    ComIF base;
    int uart_ch;
}UART_Com;

// SPIドライバ
typedef struct{
    ComIF base;
    int spi_ch;
    int spi_msg;
}SPI_Com;

// メゾット実装
// 初期化
int UART_init(void *self){
    UART_Com *obj = (UART_Com*)self;
    printf("Uart initial\n");
    return 0;
}

// 送信
int UART_send(void *self,const uint8_t *buf,int len){
    UART_Com *obj = (UART_Com*)self;
    printf("Uart send\n");
    return 0;
}
// 受信
int UART_recv(void *self,uint8_t *buf,int len){
    UART_Com *obj = (UART_Com*)self;
    printf("Uart recv\n");
    return 0;
}

// メゾット実装
// 初期化
int SPI_init(void *self){
    SPI_Com *obj = (SPI_Com*)self;
    printf("SPI initial\n");
    return 0;
}

// 送信
int SPI_send(void *self,const uint8_t *buf,int len){
    SPI_Com *obj = (SPI_Com*)self;
    printf("SPI send\n");
    return 0;
}
// 受信
int SPI_recv(void *self,uint8_t *buf,int len){
    SPI_Com *obj = (SPI_Com*)self;
    printf("SPI recv\n");
    return 0;
}

// 初期化
void UART_Com_init(UART_Com *obj,int ch){
    obj->uart_ch = ch;

    // 関数設置
    obj->base.init = UART_init;
    obj->base.send = UART_send;
    obj->base.recv = UART_recv;
}

// 初期化(ここで関数ごとの処理をいれる)
void SPI_Com_init(SPI_Com *obj,int ch,int msg){
    obj->spi_ch = ch;
    obj->spi_msg = msg;

    // 関数設置
    obj->base.init = SPI_init;
    obj->base.send = SPI_send;
    obj->base.recv = SPI_recv;
}

int main(){
    // 宣言
    ComIF *com;
    UART_Com uart;
    SPI_Com spi;

    // 初期化
    SPI_Com_init(&spi,1,8);
    com = (ComIF*)&spi;
    
    // テスト用変数
    const uint8_t * data = "Hello";
    int len = 5;
    int msg = 9;
    uint8_t rbuf[10] = {0};

    // 実際の処理
    com->init(com);
    com->send(com,data,len);
    com->recv(com,rbuf,sizeof(rbuf));
}