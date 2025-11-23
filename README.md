# 学習メモ

## モーターを題材とした組み込み開発テンプレートを検討
###1. モーター単体の動かし方テンプレートについて(motor.c,motor.h)
 - モーター単体及び複数のモーターでも構造体で管理をしてあげる。その中に基本的な停止や前進などの関数ポインタを記載してあげるとモーター個別の制御ができる。また、その関数ポインタを設定する際に状態列挙体に応じた関数を記載してあげると制御が楽になる。

###2. モーターの動かし方のテンプレートについて(state.c)
 - モーターの動かし方は状態テーブルで管理をしてあげる。次の状態の設定と現在の状態の関数ポインタを設定してあげる。そしてその状態が維持される条件を満たしたときに次の状態へ遷移する。

## C言語でのオブジェクト指向プログラミング
###1. C言語での継承(inheritance.c)
 - ベースとなる関数ポインタや変数の構造体を作成する。次にその機能を継承する構造体にベースとなる構造体を先頭に配置した構造体を作成する。コードでの使用時には、ベース構造体を継承構造体を使用する関数に引数として渡して継承構造体でキャストするとユーザーはベース構造体のみ意識のみで使用できたりする。

## makefileの作成方法
###1. いまのmakefileを流用すればある程度のものはできる

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
