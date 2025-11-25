# 学習メモ

## モーターを題材とした組み込み開発テンプレート

### 1. モーター単体の制御テンプレート (motor.c)

複数のモーターを構造体で管理し、関数ポインタで状態ごとの動作を切り替える設計パターン。

#### 設計のポイント
- モーターごとにピン情報と状態を構造体で管理
- 状態（STOP, FORWARD, BACKWARD）と関数を配列で紐づけ
- 関数ポインタにより、状態に応じた処理を動的に呼び出し

#### コード例
```c
// 状態の列挙
typedef enum {
    STOP,
    FORWARD,
    BACKWARD
} MTRState;

// モーター構造体
typedef struct {
    const uint8_t dirPin;
    const uint8_t pwmPin;
    MTRState currentState;

    // 状態と関数を紐づける配列
    void (*stateProcs[3])(MotorS *motor);
} MotorS;

// 初期化時に状態テーブルを設定
void initMTR() {
    motors[i].stateProcs[STOP]     = motorStop;
    motors[i].stateProcs[FORWARD]  = motorForward;
    motors[i].stateProcs[BACKWARD] = motorBackward;
}

// 駆動処理: 状態に応じた関数を呼び出す
bool motorDrive(MTRTYPE motortype, MTRState wantstate) {
    motors[motortype].stateProcs[wantstate](&motors[motortype]);
    motors[motortype].currentState = wantstate;
    return true;
}
```

#### メリット
- モーターの追加が容易（配列に追加するだけ）
- 状態追加時も関数を追加して配列に登録するだけ
- 各モーターを独立して制御可能

---

### 2. 状態遷移テーブルによる動作制御 (state.c)

ロボットの動作シーケンスを状態テーブルで管理するパターン。

#### 設計のポイント
- 各状態に「入口処理(enter)」「実行中処理(update)」「出口処理(exit)」を定義
- 状態の持続時間と次の状態を設定
- メインループで状態マシンを更新

#### コード例
```c
// 状態データ構造体
typedef struct {
    void (*enter)(void);      // 状態に入った時の処理
    void (*update)(void);     // 状態中の繰り返し処理
    void (*exit)(void);       // 状態を出る時の処理
    RobotState nextState;     // 次の状態
    uint32_t duration;        // 持続時間(ms)
} StateData;

// 状態の列挙
typedef enum {
    STATE_INIT,
    STATE_FORWARD,
    STATE_TURN,
    STATE_STOP,
    STATE_MAX
} RobotState;

// 状態テーブル定義
StateData stateTable[STATE_MAX] = {
    //           enter,         update, exit, nextState,      duration
    [STATE_INIT]    = {NULL,         NULL, NULL, STATE_FORWARD, 0},
    [STATE_FORWARD] = {forwardEnter, NULL, NULL, STATE_TURN,    2000},
    [STATE_TURN]    = {turnEnter,    NULL, NULL, STATE_STOP,    1000},
    [STATE_STOP]    = {stopEnter,    NULL, NULL, STATE_STOP,    0},
};

// 状態マシン更新処理
void updateStateMachine(void) {
    StateData *state = &stateTable[currentState];
    uint32_t elapsed = millis() - stateStartTime;

    // 持続時間を超えたら次の状態へ遷移
    if (state->duration > 0 && elapsed >= state->duration) {
        if (state->exit) state->exit();

        currentState = state->nextState;
        stateStartTime = millis();

        if (stateTable[currentState].enter) {
            stateTable[currentState].enter();
        }
    }

    if (state->update) state->update();
}
```

#### メリット
- 動作シーケンスがテーブルで一覧できる
- 状態の追加・変更が容易
- 複雑な動作も見通しよく実装できる

---

## C言語でのオブジェクト指向プログラミング

### 1. インターフェースと継承 (inheritanceC.c)

C言語でポリモーフィズム（多態性）を実現する設計パターン。

#### 設計のポイント
1. **ベースインターフェース**: 関数ポインタを持つ構造体を定義
2. **派生構造体**: ベース構造体を**先頭に配置**した構造体を作成
3. **キャスト**: 派生構造体をベースにキャストして共通処理

#### コード例
```c
// ベースインターフェース（抽象クラス相当）
typedef struct {
    int (*init)(void *self);
    int (*send)(void *self, const uint8_t *buf, int len);
    int (*recv)(void *self, uint8_t *buf, int len);
} ComIF;

// UARTドライバ（派生クラス相当）
typedef struct {
    ComIF base;      // ★先頭にベースを配置
    int uart_ch;     // UART固有のメンバ
} UART_Com;

// SPIドライバ（派生クラス相当）
typedef struct {
    ComIF base;      // ★先頭にベースを配置
    int spi_ch;      // SPI固有のメンバ
    int spi_msg;
} SPI_Com;

// UART用の実装
int UART_send(void *self, const uint8_t *buf, int len) {
    UART_Com *obj = (UART_Com*)self;  // キャストして固有メンバにアクセス
    printf("UART ch%d send\n", obj->uart_ch);
    return 0;
}

// 初期化関数で関数ポインタを設定
void UART_Com_init(UART_Com *obj, int ch) {
    obj->uart_ch = ch;
    obj->base.init = UART_init;
    obj->base.send = UART_send;
    obj->base.recv = UART_recv;
}

// 使用例: ベースインターフェースで統一的に扱う
int main() {
    ComIF *com;          // ベースのポインタ
    UART_Com uart;
    SPI_Com spi;

    // SPIを使う場合
    SPI_Com_init(&spi, 1, 8);
    com = (ComIF*)&spi;  // ベースにキャスト

    // 共通インターフェースで呼び出し
    com->init(com);
    com->send(com, data, len);
    com->recv(com, rbuf, sizeof(rbuf));

    // UARTに切り替えても同じコードで動作
    UART_Com_init(&uart, 2);
    com = (ComIF*)&uart;
    com->send(com, data, len);  // UART_sendが呼ばれる
}
```

#### なぜ先頭に配置するのか？
```
メモリレイアウト:
UART_Com構造体
+0: base.init  ─┐
+4: base.send   │ ComIF部分（先頭に配置）
+8: base.recv  ─┘
+12: uart_ch    ← UART固有部分

→ UART_Com* と ComIF* は同じアドレスを指すため、キャスト可能
```

#### メリット
- ドライバの差し替えが容易（UART↔SPI↔I2C等）
- 上位層はインターフェースのみ意識すればよい
- テスト用モックの差し込みが可能

---

## Makefileの作成方法

### 1. 基本構造 (practiceMakefile/Makefile)

#### ディレクトリ構成
```
practiceMakefile/
├── Makefile
├── include/      # ヘッダーファイル
├── src/          # ソースファイル
├── obj/          # オブジェクトファイル（自動生成）
└── bin/          # 実行ファイル（自動生成）
```

#### Makefile全体
```makefile
# コンパイラとフラグ設定
CC = gcc
CFLAGS = -Wall -g -Iinclude -MMD -MP

# ディレクトリ定義
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# ターゲット（実行ファイル名）
TARGET = $(BIN_DIR)/myapp

# 全ての .c ファイルを src ディレクトリから探す
SRCS = $(wildcard $(SRC_DIR)/*.c)

# .c を .o に変換（src/main.c → obj/main.o）
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# 依存関係ファイル（ヘッダー変更検知用）
DEPS = $(OBJS:.o=.d)

# デフォルトターゲット
all: $(TARGET)

# リンク（実行ファイル生成）
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^

# コンパイル（.c → .o）
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# クリーン
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# 依存関係ファイルを読み込む
-include $(DEPS)

.PHONY: all clean
```

### 2. 重要な変数・関数

| 記法 | 意味 | 例 |
|------|------|-----|
| `$@` | ターゲット名 | `$(TARGET)` |
| `$<` | 最初の依存ファイル | `src/main.c` |
| `$^` | 全ての依存ファイル | `obj/main.o obj/hello.o` |
| `$(wildcard *.c)` | パターンにマッチするファイル一覧 | |
| `$(patsubst A,B,TEXT)` | TEXTのAをBに置換 | |

### 3. フラグの意味

| フラグ | 意味 |
|--------|------|
| `-Wall` | 全ての警告を表示 |
| `-g` | デバッグ情報を付与 |
| `-Iinclude` | ヘッダーファイルの検索パス |
| `-MMD -MP` | ヘッダー依存関係を自動生成 |

### 4. 使い方

```bash
make          # ビルド
make clean    # 生成物を削除
make all      # 明示的にallターゲットを実行
```

---

## GitHub Actionsの導入方法

### 1. GitHub Actionsとは
GitHub Actionsは、リポジトリで発生するイベント（push、pull request等）をトリガーに、自動でタスクを実行するCI/CDツール。

**主な用途:**
- コードをpushしたときに自動でビルド・テスト
- 静的解析によるコード品質チェック
- 自動デプロイ

### 2. 設定ファイルの配置場所
```
.github/workflows/main.yml
```
このパスにYAMLファイルを配置する。

### 3. ワークフローファイルの基本構造
```yaml
name: ワークフロー名              # GitHubのActionsタブに表示される

on:                              # トリガー条件
  push:
    branches: [main]             # mainブランチへのpush時
  pull_request:
    branches: [main]             # mainへのPR時
  workflow_dispatch:             # 手動実行を許可

jobs:                            # 実行するジョブ
  build:                         # ジョブ名
    runs-on: ubuntu-latest       # 実行環境

    steps:                       # 実行するステップ
      - uses: actions/checkout@v4    # リポジトリをチェックアウト
      - name: ステップ名
        run: コマンド                 # シェルコマンドを実行
```

### 4. 導入手順
1. `.github/workflows/main.yml` を作成
2. 作業ブランチで変更をコミット・プッシュ
3. GitHubでPull Requestを作成（base: main ← compare: 作業ブランチ）
4. CIチェックが完了するのを待つ
5. 「Merge pull request」でマージ
6. Actionsタブで実行結果を確認

### 5. 組み込み開発への応用

#### 5.1 クロスコンパイル環境の自動化
組み込み開発では、ターゲットボード用のクロスコンパイラを使用する。GitHub Actionsでクロスコンパイル環境を構築できる。

```yaml
jobs:
  build-arm:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install ARM toolchain
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc-arm-none-eabi
      - name: Build for ARM
        run: |
          arm-none-eabi-gcc -mcpu=cortex-m4 -c src/*.c
```

#### 5.2 静的解析の自動化
組み込みソフトウェアでは品質が重要。以下のツールをCIに組み込める：

| ツール | 用途 |
|--------|------|
| cppcheck | C/C++の静的解析（メモリリーク、未初期化変数等） |
| clang-tidy | より高度な静的解析 |
| MISRA-Cチェッカー | 車載・医療機器向けコーディング規約 |

```yaml
- name: Run cppcheck
  run: |
    cppcheck --enable=warning,style --error-exitcode=1 src/
```

#### 5.3 ユニットテストの自動実行
ハードウェア非依存のロジック部分はPC上でテスト可能：

```yaml
- name: Run unit tests
  run: |
    gcc -o test_runner tests/*.c src/logic.c -I include
    ./test_runner
```

#### 5.4 ファームウェアサイズのチェック
ROMサイズ制限がある組み込み開発では、ビルド後のサイズ監視が重要：

```yaml
- name: Check firmware size
  run: |
    arm-none-eabi-size bin/firmware.elf
    # サイズが制限を超えたらエラーにする
    SIZE=$(arm-none-eabi-size bin/firmware.elf | tail -1 | awk '{print $1}')
    if [ $SIZE -gt 65536 ]; then
      echo "Firmware size exceeds 64KB limit!"
      exit 1
    fi
```

#### 5.5 組み込み開発でのCI/CDの利点
- **早期バグ発見**: pushのたびに自動でビルド・静的解析
- **リグレッション防止**: テストが自動で実行される
- **チーム開発の効率化**: PRごとにCIが通るか確認できる
- **ドキュメント自動生成**: Doxygenでコードからドキュメント生成
