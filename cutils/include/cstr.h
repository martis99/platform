#ifndef CSTR_H
#define CSTR_H

#include <stdarg.h>

#include "type.h"

size_t cstrv(char *cstr, size_t size, const char *fmt, va_list args);
size_t cstrf(char *cstr, size_t size, const char *fmt, ...);

size_t cstr_len(const char *cstr);

size_t cstr_catn(char *cstr, size_t size, size_t cstr_len, const char *src, size_t src_len, size_t len);
size_t cstr_cat(char *cstr, size_t size, size_t cstr_len, const char *src, size_t src_len);

int cstr_cmpn(const char *cstr, size_t cstr_len, const char *src, size_t src_len, size_t len);
int cstr_cmp(const char *cstr, size_t cstr_len, const char *src, size_t src_len);

int cstr_eq(const char *cstr, size_t cstr_len, const char *src, size_t src_len);

void *cstr_cpy(char *cstr, size_t size, const char *src, size_t len);

char *cstr_chr(const char *cstr, char c);
char *cstr_rchr(const char *cstr, char c);
char *cstr_cstr(const char *cstr, const char *src);

size_t cstr_replace(const char *src, size_t src_len, char *dst, size_t dst_len, const char *from, size_t from_len, const char *to, size_t to_len);
size_t cstr_replaces(const char *src, size_t src_len, char *dst, size_t dst_len, const char *const *from, const char *const *to, size_t len);
size_t cstr_inplace(char *str, size_t str_size, size_t str_len, const char *old, size_t old_len, const char *new, size_t new_len);
size_t cstr_inplaces(char *str, size_t str_size, size_t str_len, const char *const *old, const char *const *new, size_t cnt);

void wcstrn_cat(wchar_t *dst, size_t size, const wchar_t *src, size_t cnt);

#define CSTR(_str) _str, sizeof(_str) - 1

#endif
