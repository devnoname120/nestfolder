/*
 * taiHEN config.txt metnini duzenleme. Platformdan bagimsiz (PC'de test edilir).
 * Donen metinler malloc ile ayrilir; cagiran free() eder.
 */
#ifndef NEST_CFG_H
#define NEST_CFG_H

#define PLUGIN_LINE "ur0:tai/nestfolder.suprx"

/* Yorum olmayan bir satir nestfolder.suprx'i yukluyor mu? */
int cfg_has_plugin(const char *text);

/* Eklenti satiri *main bolumunde mi? */
int cfg_plugin_in_main(const char *text);

/* nestfolder.suprx satirlarini (yorum olmayan) cikarir, PLUGIN_LINE'i *main
 * bolumunun sonuna ekler. *main yoksa dosyanin sonuna *main bolumu ekler.
 * Satir sonu bicimi (CRLF/LF) korunur; her satir satir sonuyla biter. */
char *cfg_add_plugin(const char *text);

/* Yorum olmayan tum nestfolder.suprx satirlarini cikarir. */
char *cfg_remove_plugin(const char *text);

#endif
