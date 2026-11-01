#!/usr/bin/env python3
"""
nestfolder yardimci araci - app.db uzerinde, PC'de calisir.
Klasorlere titleId vermek icin simdilik bu arac kullaniliyor (fixtid).

ORIJINAL DOSYAYA ASLA DOKUNMAZ: her degisiklik yeni bir kopyaya yazilir.

Kullanim:
  python3 vita_nest.py inspect app.db
      -> sayfalari, klasorleri, klasor-sayfa baglantisini ve trigger'lari listeler.

  python3 vita_nest.py nest app.db --parent <ID> --child <ID> [-o app_nested.db]
      -> <child> klasorunu <parent> klasorunun icine tasir (kopyada).
         ID'ler 'inspect' ciktisindaki [#rowid] degerleridir.

  python3 vita_nest.py setfield app.db --folder <ID> --field type --value 0 [-o app_field.db]
      -> klasor icindeki bir klasorun 'type' veya 'icon0Type' alanini degistirir (deney icin).

  python3 vita_nest.py settid app.db --folder <ID> [--tid FLDR00001] [-o app_tid.db]
      -> klasor icindeki bir klasore sahte titleId verir (kopyada).
         --clear ile titleId tekrar bos (NULL) yapilir.
"""
import re
import argparse
import shutil
import sqlite3
import sys
from pathlib import Path

FOLDER_TYPE = 7          # tbl_appinfo_icon.icon0Type: 7 = klasor
SLOTS_PER_FOLDER = 10    # firmware siniri: klasor basina 10 ikon


def connect(path):
    p = Path(path)
    if not p.is_file():
        sys.exit(f"Dosya bulunamadi: {p}\nAdi dogru yazdin mi? (ornek: app.db)")
    # mode=ro yoksa sqlite eksik dosyayi bos olarak olusturur; uri ile engelliyoruz
    db = sqlite3.connect(p.resolve().as_uri() + "?mode=rw", uri=True)
    db.row_factory = sqlite3.Row
    names = {r[0] for r in db.execute("SELECT name FROM sqlite_master WHERE type='table'")}
    missing = {"tbl_appinfo_icon", "tbl_appinfo_page"} - names
    if missing:
        sys.exit(f"{p.name} bir Vita app.db dosyasina benzemiyor (eksik tablo: "
                 f"{', '.join(sorted(missing))}).\n"
                 "Dogru dosya: ur0:shell/db/app.db (boyutu genelde 200 KB ustu).")
    return db


def folder_pages(db):
    """pageNo < 0 olan sayfalar = klasor icerik sayfalari."""
    return {r["pageId"]: r["pageNo"] for r in
            db.execute("SELECT pageId, pageNo FROM tbl_appinfo_page WHERE pageNo < 0")}


def folders(db):
    return db.execute(
        "SELECT rowid, * FROM tbl_appinfo_icon WHERE icon0Type = ? ORDER BY pageId, pos",
        (FOLDER_TYPE,)).fetchall()


def link_of(row, fpages):
    """Klasor satirinin icerik sayfasini bulur, (alan, pageId) dondurur.
    Gercek 3.65 app.db'de: klasor satirinin reserved01 degeri, icerik sayfasinin
    pageNo degerine esit (orn. -11913770). Diger alanlar yedek olarak denenir."""
    by_no = {no: pid for pid, no in fpages.items()}
    for col in ("reserved01", "status", "reserved02"):
        try:
            v = int(row[col])
        except (TypeError, ValueError):
            continue
        if v in by_no:
            return col, by_no[v]
        if v in fpages:
            return col, v
    return None, None


def children(db, page_id):
    return db.execute(
        "SELECT rowid, pos, title, titleId, icon0Type FROM tbl_appinfo_icon "
        "WHERE pageId = ? ORDER BY pos", (page_id,)).fetchall()


def fk_list(db):
    return [tuple(r) for r in db.execute("PRAGMA foreign_key_check")]


def show(v, n=40):
    if isinstance(v, bytes):
        return f"<blob {len(v)} bayt>"
    s = repr(v)
    return s if len(s) <= n else s[:n] + "..."


def cmd_inspect(args):
    db = connect(args.db)
    print("== Tablolar / trigger'lar ==")
    for r in db.execute("SELECT type, name, tbl_name FROM sqlite_master "
                        "WHERE type IN ('table','trigger') ORDER BY type, name"):
        print(f"  {r['type']:8} {r['name']}  ({r['tbl_name']})")

    print("\n== Ana sayfalar ==")
    for r in db.execute("SELECT pageId, pageNo FROM tbl_appinfo_page "
                        "WHERE pageNo >= 0 ORDER BY pageNo"):
        n = db.execute("SELECT COUNT(*) FROM tbl_appinfo_icon WHERE pageId=?",
                       (r["pageId"],)).fetchone()[0]
        print(f"  pageNo={r['pageNo']:2}  pageId={r['pageId']}  ikon={n}")

    fpages = folder_pages(db)
    cols = [d[1] for d in db.execute("PRAGMA table_info(tbl_appinfo_icon)")]
    print(f"\n== Klasorler ({len(fpages)} klasor sayfasi) ==")
    for f in folders(db):
        col, target = link_of(f, fpages)
        where = "ANA SAYFA" if f["pageId"] not in fpages else "KLASOR ICINDE"
        print(f"\n[#{f['rowid']}] {show(f['title'])}  ({where}: pageId={f['pageId']} pos={f['pos']})")
        print("   alanlar: " + ", ".join(f"{c}={show(f[c], 24)}" for c in cols
                                         if c not in ("pageId", "pos")))
        if col:
            kids = children(db, target)
            print(f"   baglanti: {col} -> pageId {target}  ({len(kids)}/{SLOTS_PER_FOLDER} dolu)")
            for k in kids:
                tag = " [KLASOR]" if k["icon0Type"] == FOLDER_TYPE else ""
                print(f"     pos={k['pos']} {k['titleId'] or ''} {show(k['title'])}{tag}")
        else:
            print("   baglanti: BULUNAMADI (bu satiri bana gonder, alanlari birlikte inceleyelim)")

    linked = {link_of(f, fpages)[1] for f in folders(db)}
    orphans = [pid for pid in fpages if pid not in linked]
    if orphans:
        print("\n== Hicbir klasore bagli olmayan klasor sayfalari ==")
        for pid in orphans:
            print(f"  pageId={pid} pageNo={fpages[pid]} ikon={len(children(db, pid))}")


def cmd_nest(args):
    src = Path(args.db)
    out = Path(args.output or src.with_name(src.stem + "_nested.db"))
    if out.resolve() == src.resolve():
        sys.exit("Cikti dosyasi orijinalle ayni olamaz.")
    shutil.copy2(src, out)
    db = connect(out)
    fpages = folder_pages(db)
    by_id = {f["rowid"]: f for f in folders(db)}

    parent, child = by_id.get(args.parent), by_id.get(args.child)
    if not parent or not child:
        sys.exit("Gecersiz ID. Once 'inspect' calistirip [#...] degerlerini kullan.")
    if parent["rowid"] == child["rowid"]:
        sys.exit("Bir klasor kendi icine konamaz.")
    if parent["pageId"] in fpages:
        sys.exit("Ust klasor zaten bir klasorun icinde; 2 seviye sinirini asiyoruz.")
    if any(k["icon0Type"] == FOLDER_TYPE for k in children(db, link_of(child, fpages)[1] or 0)):
        sys.exit("Alt klasorun icinde zaten klasor var; 2 seviye sinirini asiyoruz.")

    if child["titleId"] is None or str(child["titleId"]).strip() == "":
        sys.exit("Alt klasorun titleId'si bos; klasor icinde GORUNMEZ olur.\n"
                 "Once 'fixtid' calistir "
                 "sonra tekrar dene.")

    col, target = link_of(parent, fpages)
    if not col:
        sys.exit("Ust klasorun icerik sayfasi bulunamadi.")
    used = {k["pos"] for k in children(db, target)}
    free = next((p for p in range(SLOTS_PER_FOLDER) if p not in used), None)
    if free is None:
        sys.exit("Ust klasor dolu (10/10).")

    fk_before = fk_list(db)
    db.execute("UPDATE tbl_appinfo_icon SET pageId=?, pos=? WHERE rowid=?",
               (target, free, child["rowid"]))
    db.commit()

    ok = db.execute("PRAGMA integrity_check").fetchone()[0]
    fk_after = fk_list(db)
    db.close()
    new = [x for x in fk_after if x not in fk_before]
    print(f"Tasindi: [#{child['rowid']}] {show(child['title'])} -> "
          f"[#{parent['rowid']}] {show(parent['title'])} (pageId {target}, pos {free})")
    print(f"integrity_check: {ok}")
    print(f"foreign_key hatasi: once {len(fk_before)}, sonra {len(fk_after)}, "
          f"bu islemin YENI urettigi: {len(new)}")
    for t, n in sorted({(x[0], x[2]) for x in fk_before}):
        cnt = sum(1 for x in fk_before if x[0] == t and x[2] == n)
        print(f"   (onceden var) {t} -> {n}: {cnt} satir")
    for x in new:
        print(f"   YENI HATA: {x}")
    print("SONUC: " + ("Guvenli, Vita'ya kopyalanabilir." if ok == "ok" and not new
                       else "KOPYALAMA! Ciktiyi bana gonder."))
    print(f"Yeni dosya: {out}\nOrijinal dosya degismedi: {src}")


def cmd_settid(args):
    src = Path(args.db)
    out = Path(args.output or src.with_name(src.stem + "_tid.db"))
    if out.resolve() == src.resolve():
        sys.exit("Cikti dosyasi orijinalle ayni olamaz.")
    tid = None if args.clear else args.tid
    if tid is not None and not re.fullmatch(r"[A-Z0-9]{9}", tid):
        sys.exit("titleId tam 9 karakter olmali, sadece A-Z ve 0-9 (ornek: FLDR00001).")
    shutil.copy2(src, out)
    db = connect(out)
    fpages = folder_pages(db)
    allf = folders(db)
    if str(args.folder).isdigit():
        f = {r["rowid"]: r for r in allf}.get(int(args.folder))
    else:
        m = [r for r in allf if str(r["title"] or "").strip().lower() == str(args.folder).strip().lower()]
        if len(m) > 1:
            sys.exit("Bu isimde birden fazla klasor var; 'inspect' ciktisindaki [#numara] ile sec: "
                     + ", ".join(f"#{r['rowid']}" for r in m))
        f = m[0] if m else None
    if not f:
        sys.exit("Klasor bulunamadi. Once 'inspect' calistir; numara ([#...]) ya da tam adi ver.")
    if f["pageId"] not in fpages and not args.ana_sayfa:
        sys.exit("Bu klasor ana sayfada. Ana sayfadaki bir klasore titleId vermek icin --ana-sayfa ekle.")
    if tid is not None:
        used = db.execute("SELECT (SELECT COUNT(*) FROM tbl_appinfo_icon WHERE titleId=?) + "
                          "(SELECT COUNT(*) FROM tbl_appinfo WHERE titleId=?)", (tid, tid)).fetchone()[0]
        if used:
            sys.exit(f"{tid} zaten kullaniliyor; baska bir deger sec.")
    fk_before = fk_list(db)
    db.execute("UPDATE tbl_appinfo_icon SET titleId=? WHERE rowid=?", (tid, f["rowid"]))
    db.commit()
    ok = db.execute("PRAGMA integrity_check").fetchone()[0]
    new = [x for x in fk_list(db) if x not in fk_before]
    db.close()
    print(f"[#{f['rowid']}] {show(f['title'])}: titleId {show(f['titleId'])} -> {show(tid)}")
    print(f"integrity_check: {ok} | yeni foreign_key hatasi: {len(new)}")
    print("SONUC: " + ("Guvenli, Vita'ya kopyalanabilir." if ok == "ok" and not new
                       else "KOPYALAMA! Ciktiyi bana gonder."))
    print(f"Yeni dosya: {out}\nOrijinal dosya degismedi: {src}")


def cmd_fixtid(args):
    """titleId'si bos olan TUM klasorlere benzersiz FLDRnnnnn verir.
    Klasor icine tasinmis ama titleId'si bos oldugu icin gorunmeyen klasorleri de geri getirir."""
    src = Path(args.db)
    out = Path(args.output or src.with_name(src.stem + "_fix.db"))
    if out.resolve() == src.resolve():
        sys.exit("Cikti dosyasi orijinalle ayni olamaz.")
    shutil.copy2(src, out)
    db = connect(out)
    fpages = folder_pages(db)
    used = {r[0] for r in db.execute("SELECT titleId FROM tbl_appinfo_icon WHERE titleId IS NOT NULL "
                                     "UNION SELECT titleId FROM tbl_appinfo WHERE titleId IS NOT NULL")}
    todo = [f for f in folders(db) if f["titleId"] is None or str(f["titleId"]).strip() == ""]
    if not todo:
        db.close(); out.unlink()
        print("titleId'si bos klasor yok; degisiklik gerekmedi.")
        return
    fk_before = fk_list(db)
    n = 1
    for f in todo:
        while f"FLDR{n:05d}" in used:
            n += 1
        tid = f"FLDR{n:05d}"; used.add(tid)
        db.execute("UPDATE tbl_appinfo_icon SET titleId=? WHERE rowid=?", (tid, f["rowid"]))
        where = "klasor icinde (GORUNMUYORDU, geri gelecek)" if f["pageId"] in fpages else "ana sayfada"
        print(f"[#{f['rowid']}] {show(f['title'])}: titleId -> {tid}   ({where})")
    db.commit()
    ok = db.execute("PRAGMA integrity_check").fetchone()[0]
    new = [x for x in fk_list(db) if x not in fk_before]
    db.close()
    print(f"\n{len(todo)} klasor guncellendi | integrity_check: {ok} | yeni foreign_key hatasi: {len(new)}")
    print("SONUC: " + ("Guvenli, Vita'ya kopyalanabilir." if ok == "ok" and not new
                       else "KOPYALAMA! Ciktiyi bana gonder."))
    print(f"Yeni dosya: {out}\nOrijinal dosya degismedi: {src}")


def cmd_setfield(args):
    src = Path(args.db)
    out = Path(args.output or src.with_name(src.stem + "_field.db"))
    if out.resolve() == src.resolve():
        sys.exit("Cikti dosyasi orijinalle ayni olamaz.")
    if args.field not in ("type", "icon0Type"):
        sys.exit("Sadece 'type' veya 'icon0Type' degistirilebilir.")
    shutil.copy2(src, out)
    db = connect(out)
    fpages = folder_pages(db)
    row = db.execute("SELECT rowid, * FROM tbl_appinfo_icon WHERE rowid=?", (args.folder,)).fetchone()
    if not row:
        sys.exit("Gecersiz ID. Once 'inspect' calistir.")
    if row["pageId"] not in fpages:
        sys.exit("Bu satir bir klasorun icinde degil; sadece ic ice klasorde denenebilir.")
    if row["icon0Type"] != FOLDER_TYPE and args.field == "type":
        sys.exit("Bu satir bir klasor degil.")
    fk_before = fk_list(db)
    db.execute(f"UPDATE tbl_appinfo_icon SET {args.field}=? WHERE rowid=?", (args.value, row["rowid"]))
    db.commit()
    ok = db.execute("PRAGMA integrity_check").fetchone()[0]
    new = [x for x in fk_list(db) if x not in fk_before]
    db.close()
    print(f"[#{row['rowid']}] {show(row['title'])}: {args.field} {row[args.field]} -> {args.value}")
    print(f"integrity_check: {ok} | yeni foreign_key hatasi: {len(new)}")
    print("SONUC: " + ("Guvenli, Vita'ya kopyalanabilir." if ok == "ok" and not new
                       else "KOPYALAMA! Ciktiyi bana gonder."))
    print(f"Yeni dosya: {out}\nOrijinal dosya degismedi: {src}")


def main():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd", required=True)
    i = sub.add_parser("inspect"); i.add_argument("db")
    n = sub.add_parser("nest"); n.add_argument("db")
    n.add_argument("--parent", type=int, required=True)
    n.add_argument("--child", type=int, required=True)
    n.add_argument("-o", "--output")
    t = sub.add_parser("settid"); t.add_argument("db")
    t.add_argument("--folder", required=True, help="klasor numarasi ([#...]) ya da adi")
    t.add_argument("--ana-sayfa", dest="ana_sayfa", action="store_true",
                   help="ana sayfadaki bir klasore de titleId ver")
    t.add_argument("--tid", default="FLDR00001")
    t.add_argument("--clear", action="store_true")
    t.add_argument("-o", "--output")
    x = sub.add_parser("fixtid"); x.add_argument("db")
    x.add_argument("-o", "--output")
    f = sub.add_parser("setfield"); f.add_argument("db")
    f.add_argument("--folder", type=int, required=True)
    f.add_argument("--field", required=True)
    f.add_argument("--value", type=int, required=True)
    f.add_argument("-o", "--output")
    a = p.parse_args()
    {"inspect": cmd_inspect, "nest": cmd_nest, "settid": cmd_settid, "setfield": cmd_setfield, "fixtid": cmd_fixtid}[a.cmd](a)


if __name__ == "__main__":
    main()
