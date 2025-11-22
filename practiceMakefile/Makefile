# コンパイラとフラグ設定
CC = gcc
# -Iinclude で「ヘッダーファイルは include フォルダにあるよ」と教える
# -MMD -MP はヘッダーファイルの変更を検知するための魔法（後述）
CFLAGS = -Wall -g -Iinclude -MMD -MP

# ディレクトリ定義
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# ターゲット（実行ファイル名）
TARGET = $(BIN_DIR)/myapp

# 全ての .c ファイルを src ディレクトリから探す
SRCS = $(wildcard $(SRC_DIR)/*.c)

# SRCS のリストから、.c を .o に変えて、ディレクトリも src -> obj に変える
# 例: src/main.c -> obj/main.o
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# ヘッダー依存関係ファイルのリスト（.d ファイル）
DEPS = $(OBJS:.o=.d)

# デフォルトターゲット
all: $(TARGET)

# リンク（実行ファイル生成）
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^

# コンパイル（.c -> .o）
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# make clean で生成物を削除
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# 依存関係ファイルを読み込む（ヘッダー変更検知用）
-include $(DEPS)

.PHONY: all clean