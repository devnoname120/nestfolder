/*
 * Dil: Vita sistem dili Turkce ise Turkce, degilse Ingilizce.
 * Kullanici icin ayar yok. TR(en, tr) secili dildeki metni verir.
 */
#ifndef NEST_LANG_H
#define NEST_LANG_H

extern int g_tr;   /* 1: Turkce */
#define TR(en, tr) (g_tr ? (tr) : (en))

#endif
