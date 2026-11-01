#!/usr/bin/env bash
# Kurulum uygulamasinin app.db ve config.txt mantigini PC'de dener.
# GitHub Actions'ta da calisir. Gerekenler: gcc, libsqlite3-dev, python3.
# Kullanim (depo kokunden): bash installer/test/ci_test.sh
set -euo pipefail
cd "$(dirname "$0")"
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT

gcc -O1 -Wall -o "$T/test_main" test_main.c ../src/db.c ../src/cfg.c -lsqlite3
fail=0
check() {   # check <ad> <beklenen metin> <komut...>
	local name=$1 want=$2; shift 2
	local out; out=$("$@" 2>&1 || true)
	if grep -qF -- "$want" <<<"$out"; then echo "ok    $name"
	else echo "FAIL  $name"; echo "      beklenen: $want"; sed 's/^/      | /' <<<"$out"; fail=1; fi
}

# --- titleId verme
python3 make_db.py "$T/a.db"
check "stats"            "folders=4 no_tid=2 nested=2 nested_no_tid=1 free=16" "$T/test_main" stats "$T/a.db"
check "titleId ver"      "r=0 changed=2"  "$T/test_main" fix "$T/a.db"
check "titleId tekrar"   "r=0 changed=0"  "$T/test_main" fix "$T/a.db"
check "titleId sonrasi"  "no_tid=0"       "$T/test_main" stats "$T/a.db"

# --- ic klasorleri cikarma + titleId temizleme
check "unnest"           "r=0 moved=2"    "$T/test_main" unnest "$T/a.db"
check "unnest sonrasi"   "nested=0"       "$T/test_main" stats "$T/a.db"
check "titleId temizle"  "r=0 cleared=4"  "$T/test_main" clear "$T/a.db"
check "temizlik sonrasi" "our_tid=0"      "$T/test_main" stats "$T/a.db"

# --- ic klasor varken temizleme reddedilmeli
python3 make_db.py "$T/b.db"
"$T/test_main" fix "$T/b.db" >/dev/null
check "temizle (ic klasor var)" "r=-1 cleared=0" "$T/test_main" clear "$T/b.db"

# --- ana menu dolu: unnest hicbir sey degistirmemeli
python3 make_db.py "$T/c.db" full
cp "$T/c.db" "$T/c0.db"
check "unnest (dolu menu)" "r=-2 moved=0" "$T/test_main" unnest "$T/c.db"
if cmp -s "$T/c.db" "$T/c0.db"; then echo "ok    dolu menu: dosya degismedi"; else echo "FAIL  dolu menu: dosya degisti"; fail=1; fi

# --- gecersiz dosya
echo "x" > "$T/bad.db"
check "gecersiz app.db"  "r=-1"           "$T/test_main" stats "$T/bad.db"

# --- config.txt
check "config ekle"      "<<END has=1 inmain=1" "$T/test_main" cfgadd c1.txt
check "config CRLF"      "<<END has=1 inmain=1" "$T/test_main" cfgadd c2.txt
check "config *main yok" "<<END has=1 inmain=1" "$T/test_main" cfgadd c3.txt
check "config benzer ad" "<<END has=1 inmain=1" "$T/test_main" cfgadd c4.txt
check "config sil"       "<<END has=0 inmain=0" "$T/test_main" cfgdel c2.txt
"$T/test_main" cfgadd c1.txt | sed -n '2,/<<END/p' | sed '$d' > "$T/c1b.txt"
"$T/test_main" cfgadd "$T/c1b.txt" | sed -n '2,/<<END/p' | sed '$d' > "$T/c1c.txt"
if cmp -s "$T/c1b.txt" "$T/c1c.txt"; then echo "ok    config tekrar ekleme degistirmiyor"; else echo "FAIL  config tekrar ekleme"; fail=1; fi

exit $fail
