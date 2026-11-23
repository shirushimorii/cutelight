#!/usr/bin/env bash
# CuteLight - Cutefish OS 専用インストーラー
# 使い方: curl -fsSL https://raw.githubusercontent.com/shirushimorii/cutelight/main/install.sh | bash

set -euo pipefail

APP_NAME="CuteLight"
REPO_URL="https://github.com/shirushimorii/cutelight.git"
TMP_DIR="$(mktemp -d)"
BRANCH="${CUTELIGHT_BRANCH:-main}"

cleanup() {
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

echo "==> $APP_NAME インストーラー"
echo "    対象: Cutefish OS (Debian 13 / Trixie) 専用"
echo

# Cutefish 環境チェック
if ! command -v cutefish-session >/dev/null 2>&1 && \
   ! dpkg -l 2>/dev/null | grep -q '^ii\s\+cutefish-toolkit'; then
  echo "エラー: このアプリは Cutefish OS 専用です。Cutefish 環境で実行してください。" >&2
  exit 1
fi

# 依存関係チェック & 不足時は自動インストール
missing_pkgs=()
command -v git >/dev/null 2>&1 || missing_pkgs+=(git)
command -v cmake >/dev/null 2>&1 || missing_pkgs+=(cmake)
command -v g++ >/dev/null 2>&1 || missing_pkgs+=(g++)
command -v pkg-config >/dev/null 2>&1 || missing_pkgs+=(pkg-config)
if ! command -v ninja >/dev/null 2>&1 && ! command -v make >/dev/null 2>&1; then
  missing_pkgs+=(ninja-build)
fi

# Cutefish Toolkit（ヘッダー/CMake設定を含む）
if ! find /usr/lib -name 'CutefishToolkitConfig.cmake' 2>/dev/null | grep -q . && \
   ! dpkg -l 2>/dev/null | grep -q '^ii\s\+cutefish-toolkit\s'; then
  missing_pkgs+=(cutefish-toolkit)
fi

if [ ${#missing_pkgs[@]} -gt 0 ]; then
  # 重複を除去
  mapfile -t missing_pkgs < <(printf '%s\n' "${missing_pkgs[@]}" | awk '!seen[$0]++')
  echo "==> 不足している依存関係を検出しました: ${missing_pkgs[*]}"
  if [ "$(id -u)" -eq 0 ]; then
    apt-get update
    apt-get install -y "${missing_pkgs[@]}"
  else
    if ! command -v sudo >/dev/null 2>&1; then
      echo "エラー: sudo が見つかりません。root で実行するか、手動でインストールしてください: ${missing_pkgs[*]}" >&2
      exit 1
    fi
    echo "    依存関係のインストールには sudo パスワードが必要です..."
    sudo -v < /dev/tty
    sudo apt-get update
    sudo apt-get install -y "${missing_pkgs[@]}"
  fi
  echo "==> 依存関係のインストールが完了しました。"
  echo
fi

# 再チェック
for cmd in git cmake g++ pkg-config; do
  if ! command -v "$cmd" >/dev/null 2>&1; then
    echo "エラー: 必要なコマンド '$cmd' が見つかりません。手動でインストールしてください。" >&2
    exit 1
  fi
done
if ! find /usr/lib -name 'CutefishToolkitConfig.cmake' 2>/dev/null | grep -q . && \
   ! dpkg -l 2>/dev/null | grep -q '^ii\s\+cutefish-toolkit\s'; then
  echo "エラー: Cutefish Toolkit が見つかりません。cutefish-toolkit パッケージを確認してください。" >&2
  exit 1
fi

echo "==> ソースコードを取得しています..."
git clone --depth 1 --branch "$BRANCH" "$REPO_URL" "$TMP_DIR/cutelight"

echo "==> ビルドしています..."
cd "$TMP_DIR/cutelight"
chmod +x build.sh
./build.sh Release

# 最新のビルドディレクトリを見つける
BUILD_DIR="$(ls -1d builds/build* 2>/dev/null | sort -V | tail -1)"
if [ -z "$BUILD_DIR" ] || [ ! -x "$BUILD_DIR/install.sh" ]; then
  echo "エラー: ビルド成果物が見つかりません。" >&2
  exit 1
fi

echo "==> インストールしています..."
cd "$BUILD_DIR"
./install.sh

echo
echo "==> 完了しました。CuteLight をお楽しみください。"
