# CuteLight（Cutefish OS 専用）

CuteLight は、Cutefish OS 向けのネイティブな Spotlight 風デスクトップ検索アプリです。  
**Cutefish Toolkit** を直接使用して作られており、通常の Wayland クライアントとして動作します。コンポジター、ランチャー、ドック、その他のシステムコンポーネントは変更しません。

> **重要:** このプロジェクトは **Cutefish OS 専用**です。他の Linux デスクトップ環境や他の OS では動作対象外です。

[English README](README.md)

## 特徴

- macOS Sequoia / Tahoe 風のガラス調検索ウィンドウ
- フレームレスのフローティング Wayland ウィンドウ
- 折りたたみ時 `760×76`、展開時 `760×430`（論理ピクセル）、画面サイズに応じたクランプ
- 作業領域上端から 37% の位置に配置、検索行は固定
- コンポジターのガラス/ブラー効果、軽いティント、角丸 20px
- 検索行: 検索アイコン / ネイティブテキスト入力 / 入力時のみ表示されるクリアボタン
- ネイティブ Unicode 入力（キャレット、選択、貼り付け、IME 対応）
- 静かな開閉・展開アニメーション
- ライト/ダークテーマ追従
- `Escape` またはウィンドウ外クリックで非表示（バックグラウンド常駐設定時）
- インストール済みアプリ検索
- 計算式、簡単な通貨換算、該当なし時の Google 検索フォールバック
- 設定画面（自動起動、バックグラウンド常駐、ショートカット、位置、外観、ティント、プレースホルダー）
- 設定は「保存して閉じる」で反映、「閉じる」で破棄
- システムトレイとメニューバー統合
- D-Bus 制御: `Show` / `Hide` / `Toggle` / `ShowSettings`

## 動作環境

- **Cutefish OS（Debian 13 / Trixie ベース）専用**
- `cutefish-toolkit` 開発ヘッダー / CMake パッケージ
- CMake 3.25+
- Ninja（推奨）または Make
- C++23 対応コンパイラ
- `git`, `pkg-config`

## ワンコマンドインストール

以下のコマンドで、ソース取得・ビルド・インストールを一括で行います。

```bash
curl -fsSL https://raw.githubusercontent.com/shirushimorii/cutelight/main/install.sh | bash
```

インストール中に、グローバルショートカット用ヘルパーサービスの設定のため `sudo` パスワードを求められる場合があります。

### インストールされる内容

- 実行ファイル: `~/.local/opt/cutelight/cutelight`
- デスクトップエントリ: `~/.local/share/applications/com.mori.cutelight.desktop`
- 自動起動: `~/.config/autostart/com.mori.cutelight.desktop`
- 設定ファイル: `~/.config/cutelight/cutelight.conf`
- グローバルショートカットヘルパー: `/usr/local/lib/cutelight/shortcut-helper.py`
- systemd サービス: `cutelight-shortcut.service`

インストーラーは旧 Spotlight プロトタイプの残骸を削除し、新しいビルドをインストールします。既存の CuteLight ユーザー設定は可能な限り保持します。

## 手動ビルド / 手動インストール

```bash
git clone https://github.com/shirushimorii/cutelight.git
cd cutelight
./build.sh Release
cd builds/buildN   # N は最新のビルド番号
./install.sh
```

`./build.sh [Debug|Release]` は毎回新しい `builds/buildN` ディレクトリを作成し、既存のビルドには上書きしません。

## 使い方

### 基本操作

- 検索バーに文字を入力すると、ウィンドウが下方向に展開されます。
- 結果は 1 行のコンパクト表示です。マウス、または矢印キーと `Enter` で実行できます。
- インストール済みアプリはそのまま起動します。
- 計算結果や通貨換算結果はクリップボードにコピーできます。
- 該当結果がない場合は、既定のブラウザで Google 検索を開きます。
- 入力をクリアすると、ウィンドウは折りたたみ状態に戻ります。
- `Escape` または検索ウィンドウ外クリックで非表示にできます。
- 「バックグラウンドで動作し続ける」が有効な場合、非表示後も常駐します。

### 設定を開く

- ウィンドウ内で `Ctrl+,`
- デスクトップエントリの「設定」アクション
- トレイアイコンの「設定」
- コマンド: `cutelight --settings`
- D-Bus: `ShowSettings`

設定ウィンドウはタイトルバーをドラッグして移動できます。  
**保存して閉じる** で反映、**閉じる** で破棄します。

### ショートカット

- 既定のショートカットは **Super+Space** です。
- 設定画面で「ショートカットで開く」を有効にし、ショートカットボタンをクリックして新しいキーを登録できます。まず修飾キー（例: `Ctrl`）を押し、続けてキー（例: `Space`）を押してください。`Super` のような修飾キー単体も登録できます。`Escape` でキャンセルできます。
- ショートカットから開いた場合も、検索入力欄にフォーカスが当たります。
- Cutefish はサードパーティアプリに任意のグローバルショートカット登録 API を公開していないため、CuteLight は小さなヘルパーサービス `cutelight-shortcut.service` を使ってショートカットを検知します。
- ヘルパーは設定ファイル `~/.config/cutelight/cutelight.conf` を読み、ショートカット押下時に `cutelight --toggle` を実行します。

### コマンドライン

```bash
cutelight --toggle     # 表示/非表示を切り替え（実行中インスタンスを再利用）
cutelight --show       # 表示
cutelight --hide       # 非表示
cutelight --settings   # 設定を表示
```

## プロジェクト構成

```text
src/application/  アプリケーション寿命と全体調整
src/window/       Wayland ウィンドウ、配置、アニメーション、ショートカット
src/ui/           デザイントークン、検索行、結果、設定 UI
src/search/       アプリ検索、計算、通貨換算、Web フォールバック
src/integration/  D-Bus、メニューバー、システムトレイ
src/settings/     設定永続化、自動起動
packaging/        desktop entry とインストーラーテンプレート
docs/             設計・統合ドキュメント
builds/           番号付きビルド試行（ローカル保持、Git には通常含めない）
```

## ライセンス

[LICENSE](LICENSE) を参照してください。
