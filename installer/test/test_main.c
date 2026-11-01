/* PC testi: db.c ve cfg.c. Kullanim: test_main <komut> [dosya] */
#include "../src/db.h"
#include "../src/cfg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int g_tr = 1;
static void lg(const char *m) { printf("LOG %s\n", m); }
static char *slurp(const char *p) {
	FILE *f = fopen(p, "rb"); if (!f) return NULL;
	fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
	char *b = malloc(n + 1); size_t got = fread(b, 1, n, f); b[got] = 0; fclose(f); return b;
}
int main(int argc, char **argv) {
	(void)argc;
	const char *c = argv[1];
	if (!strcmp(c, "stats")) {
		db_stats_t s; int r = db_stats(argv[2], &s);
		printf("r=%d folders=%d no_tid=%d nested=%d nested_no_tid=%d free=%d our_tid=%d %s\n",
		       r, s.folders, s.no_tid, s.nested, s.nested_no_tid, s.free_slots, s.our_tid, r ? db_error() : "");
	} else if (!strcmp(c, "fix")) {
		int n; int r = db_fix_titleids(argv[2], &n, lg);
		printf("r=%d changed=%d %s\n", r, n, r ? db_error() : "");
	} else if (!strcmp(c, "unnest")) {
		int n; int r = db_unnest_all(argv[2], &n, lg);
		printf("r=%d moved=%d %s\n", r, n, r ? db_error() : "");
	} else if (!strcmp(c, "clear")) {
		int n; int r = db_clear_titleids(argv[2], &n, lg);
		printf("r=%d cleared=%d %s\n", r, n, r ? db_error() : "");
	} else if (!strcmp(c, "cfgadd") || !strcmp(c, "cfgdel")) {
		char *t = slurp(argv[2]);
		printf("has=%d inmain=%d\n", cfg_has_plugin(t), cfg_plugin_in_main(t));
		char *o = c[3] == 'a' ? cfg_add_plugin(t) : cfg_remove_plugin(t);
		fwrite(o, 1, strlen(o), stdout);
		printf("<<END has=%d inmain=%d\n", cfg_has_plugin(o), cfg_plugin_in_main(o));
		free(t); free(o);
	}
	return 0;
}
