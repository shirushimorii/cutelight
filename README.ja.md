# cutefish-search

Cutefish デスクトップ向けの Spotlight 風デスクトップ検索アプリです。Cutefish Toolkit を直接使用してビルドしています。

> **状態:** アップストリーム提案用ブランチです。このリポジトリは元々 `cutelight` でしたが、CutefishOS の慣例に合わせて再編しています。

[English README](README.md)

## 特徴

- Cutefish Toolkit でビルドしたネイティブ Wayland 検索ポップアップ
- インストール済みアプリの検索と起動
- 計算式と簡単な通貨換算
- ローカル結果がない場合の Web 検索フォールバック
- 自動起動、常駐、位置、外観、ティント、プレースホルダー、ショートカットの設定
- D-Bus 制御（`Show` / `Hide` / `Toggle` / `ShowSettings`）
- コマンドライン制御（`--toggle` / `--show` / `--hide` / `--settings`）

## 動作環境

- Cutefish OS / Cutefish デスクトップ環境
- `cutefish-toolkit` 開発ファイル / CMake パッケージ
- CMake 3.25+
- Ninja または Make
- C++23 コンパイラ
- `pkg-config`

## ビルド

```bash
./build.sh Release
```

`./build.sh` は毎回新しい `builds/buildN` ディレクトリを作成し、以前のビルドを上書きしません。

## インストール

```bash
cd builds/buildN
./install.sh
```

インストーラーは `~/.local/opt/cutefish-search/` に導入し、デスクトップエントリと自動起動を登録します。依存関係やショートカットヘルパーの導入に `sudo` を求める場合があります。

## ワンコマンドインストール

```bash
curl -fsSL https://raw.githubusercontent.com/shirushimorii/cutelight/upstream-proposal/install.sh | bash
```

## 使い方

- 入力するとインストール済みアプリを検索します。
- `Enter` で選択した結果を実行します。
- `2+2` のような計算式や簡単な通貨換算を使えます。
- `Escape` またはウィンドウ外クリックで非表示にします。
- `Ctrl+,` で設定を開きます。

## グローバルショートカット

- 既定は **Super+Space** です。設定で有効にし、ショートカットボタンをクリックして新しい組み合わせを登録できます（まず修飾キー、続けてキーを押します。例: `Ctrl` → `Space`）。`Super` 単体も登録可能で、`Escape` でキャンセルできます。
- ショートカットから開いた場合も検索入力欄にフォーカスが当たります。
- 修飾キーのビット値は Cutefish Toolkit に合わせています（`Shift=1`、`Control=2`、`Alt=4`、`Super=8`）。設定画面とヘルパーの解釈が一致します。

## アップストリームについて

Cutefish は現状、サードパーティアプリ向けの任意グローバルショートカット登録 API を公開していません。現在の実装にはインストーラーが導入するヘルパーサービスが含まれていますが、アップストリーム採用時はコンポジター公式のショートカット機構に置き換える検討が必要です。

## ライセンス

BSD-3-Clause。[LICENSE](LICENSE) を参照してください。
