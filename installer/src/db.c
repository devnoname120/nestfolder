#include "db.h"
#include "vsqlite.h"
#include "lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Mesajlar UTF-8 ve iki dilli (lang.h); fontu Turkce karakterleri cizer. */

#define FLDR_MAX 100000
#define TITLE_MAX 64
#define MAX_ROWS 512
#define MAX_PAGES 64

static char err_buf[256];
const char *db_error(void) { return err_buf; }

static const char *vfs_name;   /* NULL: SQLite varsayilani */
void db_set_vfs(const char *name) { vfs_name = name; }

static void set_err(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(err_buf, sizeof(err_buf), fmt, ap);
	va_end(ap);
}

static void logf_(db_log_t log, const char *fmt, ...)
{
	if (!log) return;
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	log(buf);
}

/* UTF-8 karakter sinirinda kesen kopya */
static void copy_title(char *dst, const unsigned char *src)
{
	if (!src) { strcpy(dst, TR("(untitled)", "(adsız)")); return; }
	size_t n = strlen((const char *)src);
	if (n > TITLE_MAX - 1) {
		n = TITLE_MAX - 1;
		while (n > 0 && (src[n] & 0xC0) == 0x80) n--;
	}
	memcpy(dst, src, n);
	dst[n] = 0;
	for (size_t i = 0; i < n; i++) if (dst[i] == '\n' || dst[i] == '\r') dst[i] = ' ';
}

static sqlite3 *db_open(const char *path)
{
	sqlite3 *db = NULL;
	err_buf[0] = 0;
	int rc = sqlite3_open_v2(path, &db, SQLITE_OPEN_READWRITE, vfs_name);
	if (rc != SQLITE_OK) {
		set_err(TR("cannot open app.db (%d): %s", "app.db açılamadı (%d): %s"), db ? sqlite3_extended_errcode(db) : rc,
		        db ? sqlite3_errmsg(db) : "?");
		if (db) sqlite3_close(db);
		return NULL;
	}
	/* Gunluk ve gecici veriler bellekte: yan dosya acilmaz. Calisma kopyasi
	 * atilabilir oldugu icin dosya gunlugune gerek yok; ROLLBACK yine calisir. */
	sqlite3_exec(db, "PRAGMA journal_mode=MEMORY", NULL, NULL, NULL);
	sqlite3_exec(db, "PRAGMA temp_store=MEMORY", NULL, NULL, NULL);
	/* Vita app.db'si mi? */
	sqlite3_stmt *st;
	int ok = 0;
	if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND "
	                       "name IN ('tbl_appinfo_icon','tbl_appinfo_page')", -1, &st, NULL) == SQLITE_OK) {
		if (sqlite3_step(st) == SQLITE_ROW) ok = sqlite3_column_int(st, 0) == 2;
		sqlite3_finalize(st);
	}
	if (!ok) {
		set_err(TR("app.db does not contain the expected tables", "app.db beklenen tabloları içermiyor"));
		sqlite3_close(db);
		return NULL;
	}
	return db;
}

static int exec_sql(sqlite3 *db, const char *sql)
{
	if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
		set_err(TR("SQL error (%s, %d): %s", "SQL hatası (%s, %d): %s"), sql, sqlite3_extended_errcode(db), sqlite3_errmsg(db));
		return -1;
	}
	return 0;
}

static int integrity_ok(sqlite3 *db)
{
	sqlite3_stmt *st;
	int ok = 0;
	if (sqlite3_prepare_v2(db, "PRAGMA integrity_check", -1, &st, NULL) != SQLITE_OK) {
		set_err(TR("integrity_check failed to run: %s", "integrity_check çalışmadı: %s"), sqlite3_errmsg(db));
		return 0;
	}
	if (sqlite3_step(st) == SQLITE_ROW) {
		const unsigned char *t = sqlite3_column_text(st, 0);
		ok = t && strcmp((const char *)t, "ok") == 0;
		if (!ok) set_err("integrity_check: %s", t ? (const char *)t : "?");
	}
	sqlite3_finalize(st);
	return ok;
}

static int query_int(sqlite3 *db, const char *sql, int *out)
{
	sqlite3_stmt *st;
	if (sqlite3_prepare_v2(db, sql, -1, &st, NULL) != SQLITE_OK) {
		set_err(TR("SQL error: %s", "SQL hatası: %s"), sqlite3_errmsg(db));
		return -1;
	}
	int rc = sqlite3_step(st);
	*out = rc == SQLITE_ROW ? sqlite3_column_int(st, 0) : 0;
	sqlite3_finalize(st);
	return rc == SQLITE_ROW ? 0 : -1;
}

#define FOLDER_PAGES "(SELECT pageId FROM tbl_appinfo_page WHERE pageNo < 0)"
#define NO_TID "(titleId IS NULL OR titleId = '')"

/* ===================================================================
 *  Ana menu sayfalari ve bos yerler
 * =================================================================== */
typedef struct { sqlite3_int64 pageId; int pageNo; unsigned used; } mpage_t;

static int load_main_pages(sqlite3 *db, mpage_t *pg, int *npg)
{
	sqlite3_stmt *st, *st2;
	*npg = 0;
	if (sqlite3_prepare_v2(db, "SELECT pageId, pageNo FROM tbl_appinfo_page "
	                       "WHERE pageNo >= 0 ORDER BY pageNo", -1, &st, NULL) != SQLITE_OK) {
		set_err(TR("SQL error: %s", "SQL hatası: %s"), sqlite3_errmsg(db));
		return -1;
	}
	while (sqlite3_step(st) == SQLITE_ROW && *npg < MAX_PAGES) {
		pg[*npg].pageId = sqlite3_column_int64(st, 0);
		pg[*npg].pageNo = sqlite3_column_int(st, 1);
		pg[*npg].used = 0;
		(*npg)++;
	}
	sqlite3_finalize(st);
	if (sqlite3_prepare_v2(db, "SELECT pos FROM tbl_appinfo_icon WHERE pageId = ?",
	                       -1, &st2, NULL) != SQLITE_OK) {
		set_err(TR("SQL error: %s", "SQL hatası: %s"), sqlite3_errmsg(db));
		return -1;
	}
	for (int i = 0; i < *npg; i++) {
		sqlite3_reset(st2);
		sqlite3_bind_int64(st2, 1, pg[i].pageId);
		while (sqlite3_step(st2) == SQLITE_ROW) {
			int pos = sqlite3_column_int(st2, 0);
			/* Beklenmeyen pos: o sayfayi dolu say, dokunma */
			if (pos < 0 || pos >= MAIN_PAGE_SLOTS) pg[i].used = (1u << MAIN_PAGE_SLOTS) - 1;
			else pg[i].used |= 1u << pos;
		}
	}
	sqlite3_finalize(st2);
	return 0;
}

static int count_free(const mpage_t *pg, int npg)
{
	int n = 0;
	for (int i = 0; i < npg; i++)
		for (int p = 0; p < MAIN_PAGE_SLOTS; p++)
			if (!(pg[i].used & (1u << p))) n++;
	return n;
}

/* ===================================================================
 *  Durum
 * =================================================================== */
int db_stats(const char *path, db_stats_t *s)
{
	memset(s, 0, sizeof(*s));
	sqlite3 *db = db_open(path);
	if (!db) return -1;
	int r = 0;
	r |= query_int(db, "SELECT COUNT(*) FROM tbl_appinfo_icon WHERE icon0Type = 7", &s->folders);
	r |= query_int(db, "SELECT COUNT(*) FROM tbl_appinfo_icon WHERE icon0Type = 7 AND " NO_TID, &s->no_tid);
	r |= query_int(db, "SELECT COUNT(*) FROM tbl_appinfo_icon WHERE icon0Type = 7 AND pageId IN "
	               FOLDER_PAGES, &s->nested);
	r |= query_int(db, "SELECT COUNT(*) FROM tbl_appinfo_icon WHERE icon0Type = 7 AND " NO_TID
	               " AND pageId IN " FOLDER_PAGES, &s->nested_no_tid);
	r |= query_int(db, "SELECT COUNT(*) FROM tbl_appinfo_icon WHERE icon0Type = 7 AND "
	               "titleId GLOB 'FLDR[0-9][0-9][0-9][0-9][0-9]'", &s->our_tid);
	mpage_t pg[MAX_PAGES]; int npg;
	if (r == 0 && load_main_pages(db, pg, &npg) == 0) s->free_slots = count_free(pg, npg);
	else r = -1;
	sqlite3_close(db);
	return r ? -1 : 0;
}

/* ===================================================================
 *  titleId verme
 * =================================================================== */
static void mark_fldr(unsigned char *used, const unsigned char *t)
{
	if (!t || strncmp((const char *)t, "FLDR", 4) != 0 || strlen((const char *)t) != 9) return;
	int v = 0;
	for (int k = 4; k < 9; k++) {
		if (t[k] < '0' || t[k] > '9') return;
		v = v * 10 + (t[k] - '0');
	}
	if (v > 0 && v < FLDR_MAX) used[v / 8] |= (unsigned char)(1u << (v % 8));
}

static void mark_from(sqlite3 *db, unsigned char *used, const char *sql)
{
	sqlite3_stmt *st;
	if (sqlite3_prepare_v2(db, sql, -1, &st, NULL) != SQLITE_OK) return;  /* tablo yoksa atla */
	while (sqlite3_step(st) == SQLITE_ROW) mark_fldr(used, sqlite3_column_text(st, 0));
	sqlite3_finalize(st);
}

typedef struct { sqlite3_int64 rowid, pageId; int nested; char title[TITLE_MAX]; } row_t;

int db_fix_titleids(const char *path, int *changed, db_log_t log)
{
	*changed = 0;
	sqlite3 *db = db_open(path);
	if (!db) return -1;
	unsigned char *used = calloc(FLDR_MAX / 8 + 1, 1);
	row_t *rows = calloc(MAX_ROWS, sizeof(row_t));
	int n = 0, ret = -1;
	sqlite3_stmt *st = NULL;
	if (!used || !rows) { set_err(TR("out of memory", "bellek yetersiz")); goto out; }

	mark_from(db, used, "SELECT titleId FROM tbl_appinfo_icon WHERE titleId LIKE 'FLDR%'");
	mark_from(db, used, "SELECT titleId FROM tbl_appinfo WHERE titleId LIKE 'FLDR%'");

	if (sqlite3_prepare_v2(db, "SELECT rowid, title, pageId, pageId IN " FOLDER_PAGES
	                       " FROM tbl_appinfo_icon WHERE icon0Type = 7 AND " NO_TID
	                       " ORDER BY pageId, pos", -1, &st, NULL) != SQLITE_OK) {
		set_err(TR("SQL error: %s", "SQL hatası: %s"), sqlite3_errmsg(db));
		goto out;
	}
	while (sqlite3_step(st) == SQLITE_ROW && n < MAX_ROWS) {
		rows[n].rowid = sqlite3_column_int64(st, 0);
		copy_title(rows[n].title, sqlite3_column_text(st, 1));
		rows[n].pageId = sqlite3_column_int64(st, 2);
		rows[n].nested = sqlite3_column_int(st, 3);
		n++;
	}
	sqlite3_finalize(st); st = NULL;
	if (n == 0) { ret = 0; goto out; }

	if (exec_sql(db, "BEGIN") < 0) goto out;
	if (sqlite3_prepare_v2(db, "UPDATE tbl_appinfo_icon SET titleId = ? WHERE rowid = ?",
	                       -1, &st, NULL) != SQLITE_OK) {
		set_err(TR("SQL error: %s", "SQL hatası: %s"), sqlite3_errmsg(db));
		exec_sql(db, "ROLLBACK");
		goto out;
	}
	int next = 1;
	for (int i = 0; i < n; i++) {
		while (next < FLDR_MAX && (used[next / 8] & (1u << (next % 8)))) next++;
		if (next >= FLDR_MAX) { set_err(TR("no free FLDR number left", "boş FLDR numarası kalmadı")); break; }
		char tid[16];
		snprintf(tid, sizeof(tid), "FLDR%05d", next % FLDR_MAX);
		used[next / 8] |= (unsigned char)(1u << (next % 8));
		sqlite3_reset(st);
		sqlite3_bind_text(st, 1, tid, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int64(st, 2, rows[i].rowid);
		if (sqlite3_step(st) != SQLITE_DONE || sqlite3_changes(db) != 1) {
			set_err(TR("cannot write titleId (%d): %s", "titleId yazılamadı (%d): %s"), sqlite3_extended_errcode(db), sqlite3_errmsg(db));
			break;
		}
		logf_(log, "  %s → %s%s", rows[i].title, tid,
		      rows[i].nested ? TR("  (inside a folder, will be visible again)", "  (klasör içinde, yeniden görünecek)") : "");
		(*changed)++;
	}
	sqlite3_finalize(st); st = NULL;
	if (*changed != n) { exec_sql(db, "ROLLBACK"); *changed = 0; goto out; }
	if (exec_sql(db, "COMMIT") < 0) { exec_sql(db, "ROLLBACK"); *changed = 0; goto out; }
	ret = integrity_ok(db) ? 0 : -1;
out:
	if (st) sqlite3_finalize(st);
	free(used);
	free(rows);
	sqlite3_close(db);
	return ret;
}

/* ===================================================================
 *  Ic klasorleri ana menuye cikarma
 * =================================================================== */

/* Klasor sayfasinin (pageId) sahibi olan klasor satirinin pageId'si.
 * Klasor satirinin reserved01 degeri icerik sayfasinin pageNo'sudur. */
static int owner_page(sqlite3 *db, sqlite3_int64 page, sqlite3_int64 *owner)
{
	sqlite3_stmt *st;
	int ok = 0;
	if (sqlite3_prepare_v2(db, "SELECT f.pageId FROM tbl_appinfo_icon f, tbl_appinfo_page p "
	                       "WHERE p.pageId = ? AND f.icon0Type = 7 AND "
	                       "CAST(f.reserved01 AS INTEGER) = p.pageNo LIMIT 1",
	                       -1, &st, NULL) != SQLITE_OK) return 0;
	sqlite3_bind_int64(st, 1, page);
	if (sqlite3_step(st) == SQLITE_ROW) { *owner = sqlite3_column_int64(st, 0); ok = 1; }
	sqlite3_finalize(st);
	return ok;
}

int db_unnest_all(const char *path, int *moved, db_log_t log)
{
	*moved = 0;
	sqlite3 *db = db_open(path);
	if (!db) return -1;
	row_t *rows = calloc(MAX_ROWS, sizeof(row_t));
	mpage_t pg[MAX_PAGES]; int npg = 0;
	sqlite3_stmt *st = NULL;
	int n = 0, ret = -1;
	if (!rows) { set_err(TR("out of memory", "bellek yetersiz")); goto out; }

	if (sqlite3_prepare_v2(db, "SELECT rowid, title, pageId FROM tbl_appinfo_icon "
	                       "WHERE icon0Type = 7 AND pageId IN " FOLDER_PAGES
	                       " ORDER BY pageId, pos", -1, &st, NULL) != SQLITE_OK) {
		set_err(TR("SQL error: %s", "SQL hatası: %s"), sqlite3_errmsg(db));
		goto out;
	}
	while (sqlite3_step(st) == SQLITE_ROW && n < MAX_ROWS) {
		rows[n].rowid = sqlite3_column_int64(st, 0);
		copy_title(rows[n].title, sqlite3_column_text(st, 1));
		rows[n].pageId = sqlite3_column_int64(st, 2);
		n++;
	}
	sqlite3_finalize(st); st = NULL;
	if (n == 0) { ret = 0; goto out; }

	if (load_main_pages(db, pg, &npg) < 0) goto out;
	if (npg == 0) { set_err(TR("no home screen page found", "ana menü sayfası bulunamadı")); goto out; }
	int nfree = count_free(pg, npg);
	if (nfree < n) {
		set_err(TR("%d free home screen slots needed, %d available. Put a few "
		           "icons into a folder first.",
		           "Ana menüde %d boş yer gerekli, %d var. Önce ana menüden "
		           "birkaç ikonu bir klasöre koy."), n, nfree);
		ret = -2;
		goto out;
	}

	if (exec_sql(db, "BEGIN") < 0) goto out;
	if (sqlite3_prepare_v2(db, "UPDATE tbl_appinfo_icon SET pageId = ?, pos = ? WHERE rowid = ?",
	                       -1, &st, NULL) != SQLITE_OK) {
		set_err(TR("SQL error: %s", "SQL hatası: %s"), sqlite3_errmsg(db));
		exec_sql(db, "ROLLBACK");
		goto out;
	}
	for (int i = 0; i < n; i++) {
		/* Ust klasorun bulundugu ana sayfadan basla (en fazla 4 seviye yukari) */
		int start = 0;
		sqlite3_int64 page = rows[i].pageId, up;
		for (int d = 0; d < 4 && owner_page(db, page, &up); d++) {
			page = up;
			int found = -1;
			for (int k = 0; k < npg; k++) if (pg[k].pageId == page) { found = k; break; }
			if (found >= 0) { start = found; break; }
		}
		int tp = -1, tpos = -1;
		for (int k = 0; k < npg && tp < 0; k++) {
			int pi = (start + k) % npg;
			for (int p = 0; p < MAIN_PAGE_SLOTS; p++)
				if (!(pg[pi].used & (1u << p))) { tp = pi; tpos = p; break; }
		}
		if (tp < 0) { set_err(TR("no free slot found", "boş yer bulunamadı")); break; }
		sqlite3_reset(st);
		sqlite3_bind_int64(st, 1, pg[tp].pageId);
		sqlite3_bind_int(st, 2, tpos);
		sqlite3_bind_int64(st, 3, rows[i].rowid);
		if (sqlite3_step(st) != SQLITE_DONE || sqlite3_changes(db) != 1) {
			set_err(TR("cannot move folder (%d): %s", "klasör taşınamadı (%d): %s"), sqlite3_extended_errcode(db), sqlite3_errmsg(db));
			break;
		}
		pg[tp].used |= 1u << tpos;
		logf_(log, TR("  %s → home screen, page %d", "  %s → ana menü, sayfa %d"), rows[i].title, pg[tp].pageNo + 1);
		(*moved)++;
	}
	sqlite3_finalize(st); st = NULL;
	if (*moved != n) { exec_sql(db, "ROLLBACK"); *moved = 0; goto out; }
	if (exec_sql(db, "COMMIT") < 0) { exec_sql(db, "ROLLBACK"); *moved = 0; goto out; }
	ret = integrity_ok(db) ? 0 : -1;
out:
	if (st) sqlite3_finalize(st);
	free(rows);
	sqlite3_close(db);
	return ret;
}

/* ===================================================================
 *  Kaldirirken titleId'leri temizleme
 * =================================================================== */
#define OUR_TID "(icon0Type = 7 AND titleId GLOB 'FLDR[0-9][0-9][0-9][0-9][0-9]')"

int db_clear_titleids(const char *path, int *cleared, db_log_t log)
{
	*cleared = 0;
	sqlite3 *db = db_open(path);
	if (!db) return -1;
	int ret = -1, n = 0, nested = 0;
	if (query_int(db, "SELECT COUNT(*) FROM tbl_appinfo_icon WHERE " OUR_TID, &n) < 0) goto out;
	if (n == 0) { ret = 0; goto out; }
	/* Guvenlik: hala ic ice klasor varsa titleId'siz kalip gorunmez olurlar */
	if (query_int(db, "SELECT COUNT(*) FROM tbl_appinfo_icon WHERE icon0Type = 7 AND pageId IN "
	              FOLDER_PAGES, &nested) < 0) goto out;
	if (nested) {
		set_err(TR("%d nested folder(s) still inside a folder; titleIds not cleared",
		           "%d klasör hâlâ bir klasörün içinde; titleId'ler temizlenmedi"), nested);
		goto out;
	}
	if (exec_sql(db, "BEGIN") < 0) goto out;
	if (exec_sql(db, "UPDATE tbl_appinfo_icon SET titleId = NULL WHERE " OUR_TID) < 0) {
		exec_sql(db, "ROLLBACK");
		goto out;
	}
	*cleared = sqlite3_changes(db);
	if (*cleared != n) {
		set_err(TR("expected %d titleIds, cleared %d", "%d titleId bekleniyordu, %d temizlendi"), n, *cleared);
		exec_sql(db, "ROLLBACK");
		*cleared = 0;
		goto out;
	}
	if (exec_sql(db, "COMMIT") < 0) { exec_sql(db, "ROLLBACK"); *cleared = 0; goto out; }
	(void)log;
	ret = integrity_ok(db) ? 0 : -1;
out:
	sqlite3_close(db);
	return ret;
}
