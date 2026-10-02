#!/usr/bin/env bash
# 2e.3 — test DB restore: ServerTools/MuServer_S6_2020/DB/DB_SQL_12.bak → yerel SQL Express
#
# İdempotent ve güvenli:
#   - Yalnız YEREL SQL örneğine bağlanır (varsayılan .\SQLEXPRESS); canlı/uzak sunucuya dokunmaz.
#   - Hedef DB zaten varsa HİÇBİR ŞEY yapmaz (üzerine yazmaz).
#   - Yedekteki mantıksal dosya adları: MuOnlineS6 / MuOnlineS6_log.
#
# Kullanım:
#   bash BuildLog/2e3/restore_test_db.sh
# Ortam değişkenleri (isteğe bağlı):
#   SQLINSTANCE  (varsayılan .\SQLEXPRESS)   TARGETDB (varsayılan MuOnlineS6)
#   BAKPATH      (varsayılan ServerTools/MuServer_S6_2020/DB/DB_SQL_12.bak)
#   SQLCMD       (sqlcmd.exe tam yolu)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
INSTANCE="${SQLINSTANCE:-.\\SQLEXPRESS}"
TARGET="${TARGETDB:-MuOnlineS6}"
BAK_WIN="$(cygpath -w "${BAKPATH:-$ROOT/ServerTools/MuServer_S6_2020/DB/DB_SQL_12.bak}")"

if [ -z "${SQLCMD:-}" ]; then
  SQLCMD="$(command -v sqlcmd || true)"
fi
if [ -z "${SQLCMD:-}" ]; then
  for c in "/c/Program Files/Microsoft SQL Server/Client SDK/ODBC"/*/Tools/Binn/sqlcmd.exe; do
    [ -f "$c" ] && SQLCMD="$c" && break
  done
fi
[ -n "${SQLCMD:-}" ] || { echo "HATA: sqlcmd bulunamadi (SQLCMD ortam degiskeni verin)"; exit 1; }

echo "sqlcmd     : $SQLCMD"
echo "instance   : $INSTANCE"
echo "yedek       : $BAK_WIN"
echo "hedef DB   : $TARGET"

echo "== 1) hedef DB var mi? =="
EXISTS="$("$SQLCMD" -S "$INSTANCE" -E -h-1 -W -Q "SET NOCOUNT ON; SELECT ISNULL(CONVERT(varchar(8), DB_ID(N'$TARGET')), 'YOK')" | tr -d '\r' | head -1)"
if [ "$EXISTS" != "YOK" ]; then
  echo "   DB zaten var: $TARGET (restore ATLANDI — uzerine yazilmaz)"
else
  echo "   yok → restore ediliyor"
  DATADIR="$("$SQLCMD" -S "$INSTANCE" -E -h-1 -W -Q "SET NOCOUNT ON; SELECT CAST(SERVERPROPERTY('InstanceDefaultDataPath') AS varchar(300))" | tr -d '\r' | head -1)"
  [ -n "$DATADIR" ] || { echo "HATA: InstanceDefaultDataPath alinamadi"; exit 1; }
  echo "   veri dizini: $DATADIR"
  "$SQLCMD" -S "$INSTANCE" -E -b -Q "RESTORE DATABASE [$TARGET] FROM DISK = N'$BAK_WIN' WITH MOVE N'MuOnlineS6' TO N'$DATADIR$TARGET.mdf', MOVE N'MuOnlineS6_log' TO N'$DATADIR${TARGET}_log.ldf', RECOVERY, STATS = 10"
fi

echo "== 2) dogrulama =="
STATE="$("$SQLCMD" -S "$INSTANCE" -E -h-1 -W -Q "SET NOCOUNT ON; SELECT state_desc FROM sys.databases WHERE name = N'$TARGET'" | tr -d '\r' | head -1)"
TABLES="$("$SQLCMD" -S "$INSTANCE" -E -h-1 -W -Q "SET NOCOUNT ON; SELECT COUNT(*) FROM [$TARGET].sys.tables" | tr -d '\r' | head -1)"
echo "   $TARGET durum: $STATE, tablo sayisi: $TABLES"

echo "== 3) DSN kontrolu (DataServer=MuOnline, JoinServer=MuOnlineJoin) =="
for dsn in MuOnline MuOnlineJoin; do
  KEY="HKLM\\SOFTWARE\\WOW6432Node\\ODBC\\ODBC.INI\\$dsn"
  if reg query "$KEY" >/dev/null 2>&1; then echo "   DSN $dsn: VAR"; else echo "   DSN $dsn: YOK (olusturulmali!)"; fi
done
echo "Tamam."
