/*
 * app.db islemleri. Hepsi bir CALISMA KOPYASI uzerinde yapilir; canli
 * ur0:shell/db/app.db dosyasina yazma main.c'de, yeniden baslatmadan hemen
 * once yapilir. Platformdan bagimsiz (PC'de test edilir).
 */
#ifndef NEST_DB_H
#define NEST_DB_H

#define MAIN_PAGE_SLOTS 10   /* ana menu sayfasi basina ikon */

typedef void (*db_log_t)(const char *msg);

typedef struct {
	int folders;        /* tum klasorler */
	int no_tid;         /* titleId'si bos klasorler */
	int nested;         /* bir klasorun icindeki klasorler */
	int nested_no_tid;  /* bunlardan titleId'si bos olanlar (gorunmez) */
	int free_slots;     /* ana menudeki bos yerler */
	int our_tid;        /* eklentinin verdigi titleId'li klasorler (FLDRnnnnn) */
} db_stats_t;

/* Kullanilacak SQLite VFS adi (NULL: varsayilan). Vita'da sceIo VFS'i. */
void db_set_vfs(const char *name);

/* Hata mesaji (son basarisiz cagri) */
const char *db_error(void);

int db_stats(const char *path, db_stats_t *st);

/* titleId'si bos tum klasorlere FLDRnnnnn verir. *changed: verilen sayi.
 * Donus 0: basarili (integrity_check ok). */
int db_fix_titleids(const char *path, int *changed, db_log_t log);

/* Bir klasorun icindeki tum klasorleri ana menudeki bos yerlere tasir.
 * Yer yetmezse hicbir sey degistirmez ve -2 doner. */
int db_unnest_all(const char *path, int *moved, db_log_t log);

/* Eklentinin verdigi titleId'leri (klasor satirlarinda FLDR + 5 rakam)
 * temizler (NULL). Klasorler ic ice degilken cagrilmali (db_unnest_all'dan
 * sonra). *cleared: temizlenen sayi. */
int db_clear_titleids(const char *path, int *cleared, db_log_t log);

#endif
