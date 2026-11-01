/*
 * Vita'da sistemin SceSqlite modulu kullanilir (SceSqlite_stub); VitaSDK'da
 * sqlite3.h olmadigi icin gereken bildirimler burada. PC testlerinde
 * sistemin sqlite3.h dosyasi kullanilir.
 */
#ifndef NEST_VSQLITE_H
#define NEST_VSQLITE_H

#ifndef __vita__
#include <sqlite3.h>
#else
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef long long sqlite3_int64;

#define SQLITE_OK        0
#define SQLITE_ROW       100
#define SQLITE_DONE      101
#define SQLITE_INTEGER   1
#define SQLITE_TEXT      3
#define SQLITE_NULL      5
#define SQLITE_OPEN_READONLY  0x00000001
#define SQLITE_OPEN_READWRITE 0x00000002
#define SQLITE_TRANSIENT ((void (*)(void *))-1)

int sqlite3_initialize(void);
int sqlite3_open_v2(const char *filename, sqlite3 **db, int flags, const char *vfs);
int sqlite3_close(sqlite3 *db);
int sqlite3_exec(sqlite3 *db, const char *sql,
                 int (*cb)(void *, int, char **, char **), void *arg, char **errmsg);
int sqlite3_prepare_v2(sqlite3 *db, const char *sql, int n, sqlite3_stmt **st, const char **tail);
int sqlite3_step(sqlite3_stmt *st);
int sqlite3_reset(sqlite3_stmt *st);
int sqlite3_finalize(sqlite3_stmt *st);
int sqlite3_bind_int(sqlite3_stmt *st, int i, int v);
int sqlite3_bind_int64(sqlite3_stmt *st, int i, sqlite3_int64 v);
int sqlite3_bind_text(sqlite3_stmt *st, int i, const char *s, int n, void (*d)(void *));
int sqlite3_column_int(sqlite3_stmt *st, int i);
sqlite3_int64 sqlite3_column_int64(sqlite3_stmt *st, int i);
const unsigned char *sqlite3_column_text(sqlite3_stmt *st, int i);
int sqlite3_column_type(sqlite3_stmt *st, int i);
const char *sqlite3_errmsg(sqlite3 *db);
int sqlite3_extended_errcode(sqlite3 *db);
int sqlite3_changes(sqlite3 *db);
const char *sqlite3_libversion(void);

/* VFS (surum 1 yapilari) */
#define SQLITE_ERROR            1
#define SQLITE_IOERR            10
#define SQLITE_CANTOPEN         14
#define SQLITE_NOTFOUND         12
#define SQLITE_FULL             13
#define SQLITE_IOERR_SHORT_READ (SQLITE_IOERR | (2 << 8))
#define SQLITE_IOERR_READ       (SQLITE_IOERR | (1 << 8))
#define SQLITE_IOERR_WRITE      (SQLITE_IOERR | (3 << 8))
#define SQLITE_IOERR_FSYNC      (SQLITE_IOERR | (4 << 8))
#define SQLITE_IOERR_TRUNCATE   (SQLITE_IOERR | (6 << 8))
#define SQLITE_IOERR_FSTAT      (SQLITE_IOERR | (7 << 8))
#define SQLITE_IOERR_DELETE     (SQLITE_IOERR | (10 << 8))
#define SQLITE_OPEN_CREATE         0x00000004
#define SQLITE_OPEN_DELETEONCLOSE  0x00000008
#define SQLITE_OPEN_EXCLUSIVE      0x00000010
#define SQLITE_ACCESS_EXISTS    0

typedef struct sqlite3_file sqlite3_file;
typedef struct sqlite3_io_methods sqlite3_io_methods;
typedef struct sqlite3_vfs sqlite3_vfs;
struct sqlite3_file { const sqlite3_io_methods *pMethods; };
struct sqlite3_io_methods {
	int iVersion;
	int (*xClose)(sqlite3_file *);
	int (*xRead)(sqlite3_file *, void *, int, sqlite3_int64);
	int (*xWrite)(sqlite3_file *, const void *, int, sqlite3_int64);
	int (*xTruncate)(sqlite3_file *, sqlite3_int64);
	int (*xSync)(sqlite3_file *, int);
	int (*xFileSize)(sqlite3_file *, sqlite3_int64 *);
	int (*xLock)(sqlite3_file *, int);
	int (*xUnlock)(sqlite3_file *, int);
	int (*xCheckReservedLock)(sqlite3_file *, int *);
	int (*xFileControl)(sqlite3_file *, int, void *);
	int (*xSectorSize)(sqlite3_file *);
	int (*xDeviceCharacteristics)(sqlite3_file *);
};
struct sqlite3_vfs {
	int iVersion;
	int szOsFile;
	int mxPathname;
	sqlite3_vfs *pNext;
	const char *zName;
	void *pAppData;
	int (*xOpen)(sqlite3_vfs *, const char *, sqlite3_file *, int, int *);
	int (*xDelete)(sqlite3_vfs *, const char *, int);
	int (*xAccess)(sqlite3_vfs *, const char *, int, int *);
	int (*xFullPathname)(sqlite3_vfs *, const char *, int, char *);
	void *(*xDlOpen)(sqlite3_vfs *, const char *);
	void (*xDlError)(sqlite3_vfs *, int, char *);
	void (*(*xDlSym)(sqlite3_vfs *, void *, const char *))(void);
	void (*xDlClose)(sqlite3_vfs *, void *);
	int (*xRandomness)(sqlite3_vfs *, int, char *);
	int (*xSleep)(sqlite3_vfs *, int);
	int (*xCurrentTime)(sqlite3_vfs *, double *);
	int (*xGetLastError)(sqlite3_vfs *, int, char *);
};
int sqlite3_vfs_register(sqlite3_vfs *vfs, int makeDflt);
#endif

#endif
