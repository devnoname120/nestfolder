#include "cfg.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Satir: [s, e) ; satir sonu karakterleri haric */
typedef struct { const char *s; int len; } line_t;

static int next_line(const char **p, line_t *ln)
{
	const char *s = *p;
	if (!*s) return 0;
	const char *e = s;
	while (*e && *e != '\n') e++;
	int len = (int)(e - s);
	if (len > 0 && s[len - 1] == '\r') len--;
	ln->s = s; ln->len = len;
	*p = *e ? e + 1 : e;
	return 1;
}

static void trim(const char **s, int *len)
{
	while (*len > 0 && isspace((unsigned char)**s)) { (*s)++; (*len)--; }
	while (*len > 0 && isspace((unsigned char)(*s)[*len - 1])) (*len)--;
}

/* Yorum olmayan ve "nestfolder.suprx" ile biten satir mi (buyuk/kucuk harf duyarsiz) */
static int is_plugin_line(line_t ln)
{
	const char *s = ln.s; int len = ln.len;
	trim(&s, &len);
	if (len == 0 || s[0] == '#' || s[0] == '*') return 0;
	static const char suf[] = "nestfolder.suprx";
	int n = (int)sizeof(suf) - 1;
	if (len < n) return 0;
	for (int i = 0; i < n; i++)
		if (tolower((unsigned char)s[len - n + i]) != suf[i]) return 0;
	/* "xnestfolder.suprx" gibi bir ad degil: oncesi ayirici olmali */
	if (len > n) {
		char c = s[len - n - 1];
		if (c != '/' && c != ':' && c != '\\') return 0;
	}
	return 1;
}

static int is_section(line_t ln, const char **name, int *nlen)
{
	const char *s = ln.s; int len = ln.len;
	trim(&s, &len);
	if (len < 1 || s[0] != '*') return 0;
	s++; len--;
	trim(&s, &len);
	*name = s; *nlen = len;
	return 1;
}

static int is_main_section(line_t ln)
{
	const char *n; int nl;
	return is_section(ln, &n, &nl) && nl == 4 && strncmp(n, "main", 4) == 0;
}

static const char *eol_of(const char *text)
{
	const char *nl = strchr(text, '\n');
	return (nl && nl > text && nl[-1] == '\r') ? "\r\n" : "\n";
}

int cfg_has_plugin(const char *text)
{
	line_t ln; const char *p = text;
	while (next_line(&p, &ln)) if (is_plugin_line(ln)) return 1;
	return 0;
}

int cfg_plugin_in_main(const char *text)
{
	line_t ln; const char *p = text; int in_main = 0;
	while (next_line(&p, &ln)) {
		const char *n; int nl;
		if (is_section(ln, &n, &nl)) { in_main = is_main_section(ln); continue; }
		if (in_main && is_plugin_line(ln)) return 1;
	}
	return 0;
}

/* Basit buyuyen tampon */
typedef struct { char *b; size_t len, cap; } sbuf_t;
static int sb_put(sbuf_t *sb, const char *s, size_t n)
{
	if (sb->len + n + 1 > sb->cap) {
		size_t cap = sb->cap ? sb->cap * 2 : 1024;
		while (cap < sb->len + n + 1) cap *= 2;
		char *nb = realloc(sb->b, cap);
		if (!nb) return -1;
		sb->b = nb; sb->cap = cap;
	}
	memcpy(sb->b + sb->len, s, n);
	sb->len += n;
	sb->b[sb->len] = 0;
	return 0;
}

static char *rebuild(const char *text, int add)
{
	const char *eol = eol_of(text);
	size_t el = strlen(eol);
	sbuf_t sb = {0, 0, 0};
	if (sb_put(&sb, "", 0) < 0) return NULL;

	/* Eklenecek yer: *main bolumundeki son bos olmayan satirin ardi.
	 * Ilk geciste o satirin indeksini bul, ikinci geciste metni kur. */
	line_t ln; const char *p = text;
	int in_main = 0, added = 0;
	int idx = 0, insert_after = -1;
	while (next_line(&p, &ln)) {
		const char *n; int nl;
		if (is_section(ln, &n, &nl)) {
			in_main = is_main_section(ln);
			if (in_main) insert_after = idx;
		} else if (in_main && !is_plugin_line(ln)) {
			const char *s = ln.s; int len = ln.len;
			trim(&s, &len);
			if (len > 0) insert_after = idx;
		}
		idx++;
	}

	p = text; idx = 0;
	while (next_line(&p, &ln)) {
		if (!is_plugin_line(ln)) {
			if (sb_put(&sb, ln.s, ln.len) < 0 || sb_put(&sb, eol, el) < 0) goto fail;
		}
		if (add && idx == insert_after && !added) {
			if (sb_put(&sb, PLUGIN_LINE, strlen(PLUGIN_LINE)) < 0 || sb_put(&sb, eol, el) < 0) goto fail;
			added = 1;
		}
		idx++;
	}
	if (add && !added) {
		if (sb.len > 0 && sb.b[sb.len - 1] != '\n' && sb_put(&sb, eol, el) < 0) goto fail;
		if (sb_put(&sb, "*main", 5) < 0 || sb_put(&sb, eol, el) < 0 ||
		    sb_put(&sb, PLUGIN_LINE, strlen(PLUGIN_LINE)) < 0 || sb_put(&sb, eol, el) < 0) goto fail;
	}
	return sb.b;
fail:
	free(sb.b);
	return NULL;
}

char *cfg_add_plugin(const char *text)    { return rebuild(text, 1); }
char *cfg_remove_plugin(const char *text) { return rebuild(text, 0); }
