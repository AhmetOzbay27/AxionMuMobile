#!/bin/bash
# install_hooks.sh - .githooks dizinini bu klonda etkinlestirir (core.hooksPath).
# Kullanim: bash BuildLog/denetim/install_hooks.sh   (klon basina bir kez)
set -u
cd "$(git rev-parse --show-toplevel)" || exit 1
git config --local core.hooksPath .githooks || exit 1
chmod +x .githooks/* 2>/dev/null || true
echo "core.hooksPath=$(git config --local core.hooksPath)"
echo "kurulu kancalar: $(ls .githooks | tr '\n' ' ')"
echo "pre-push cekirdegi: BuildLog/denetim/pre_push_check.sh (build_all + 603 kapisi)"
