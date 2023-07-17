#include "str.h"

#include "cstr.h"
#include "mem.h"

str_t str_null()
{
	return (str_t){ 0 };
}

str_t strz(size_t size)
{
	char *data = m_malloc(size);
	if (size > 0) {
		data[0] = '\0';
	}

	return (str_t){
		.data = data,
		.size = size,
		.len  = 0,
		.ref  = 0,
	};
}

str_t strc(const char *cstr, size_t len)
{
	return (str_t){
		.data = cstr,
		.size = 0,
		.len  = len,
		.ref  = 1,
	};
}

str_t strn(const char *cstr, size_t len, size_t size)
{
	if (cstr == NULL) {
		return (str_t){ 0 };
	}

	if (len + 1 > size) {
		return (str_t){ 0 };
	}

	char *data = m_malloc(size);
	m_memcpy(data, size, cstr, len);
	data[len] = '\0';

	return (str_t){
		.data = data,
		.size = size,
		.len  = len,
		.ref  = 0,
	};
}

str_t strv(const char *fmt, va_list args)
{
	str_t str   = { 0 };
	size_t size = cstrv(NULL, 0, fmt, args);
	if (size == 0) {
		return (str_t){ 0 };
	}

	str.size = size + 1;
	str.data = m_malloc(str.size);
	if (str.data == NULL) {
		return (str_t){ 0 };
	}

	str.len = cstrv((char *)str.data, str.size, fmt, args);
	str.ref = 0;
	return str;
}

str_t strf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	str_t ret = strv(fmt, args);
	va_end(args);
	return ret;
}

str_t strb(const char *buf, size_t size, size_t len)
{
	return (str_t){
		.data = buf,
		.size = size,
		.len  = len,
		.ref  = 1,
	};
}

str_t strr()
{
	return strc(NULL, 0);
}

void str_free(str_t *str)
{
	if (str == NULL) {
		return;
	}

	if (!str->ref) {
		m_free((char *)str->data, str->size);
	}

	str->data = NULL;
	str->size = 0;
	str->len  = 0;
}

int str_resize(str_t *str, size_t size)
{
	if (str->ref) {
		return 1;
	}

	if (size <= str->size) {
		return 0;
	}

	const char *data = m_realloc((char *)str->data, size, str->size);
	if (data == NULL) {
		return 1;
	}

	str->data = data;
	str->size = size;
	return 0;
}

str_t *str_catc(str_t *str, const char *cstr, size_t len)
{
	if (str == NULL || cstr == NULL) {
		return NULL;
	}

	if (str->ref && str->size == 0) {
		return NULL;
	}

	if (!str->ref && str_resize(str, str->len + len)) {
		return NULL;
	}

	str->len = cstr_cat((char *)str->data, str->size, str->len, cstr, len);
	return str;
}

str_t *str_catn(str_t *str, str_t src, size_t len)
{
	return str_catc(str, src.data, len);
}

str_t *str_cat(str_t *str, str_t src)
{
	return str_catn(str, src, src.len);
}

int str_cmpnc(str_t str, const char *cstr, size_t cstr_len, size_t len)
{
	return cstr_cmpn(str.data, str.len, cstr, cstr_len, len);
}

int str_cmpc(str_t str, const char *cstr, size_t cstr_len)
{
	return cstr_cmp(str.data, str.len, cstr, cstr_len);
}

int str_cmpn(str_t str, str_t src, size_t len)
{
	return str_cmpnc(str, src.data, src.len, len);
}

int str_cmp(str_t str, str_t src)
{
	return str_cmpc(str, src.data, src.len);
}

int str_eqc(str_t str, const char *cstr, size_t cstr_len)
{
	return cstr_eq(str.data, str.len, cstr, cstr_len);
}

int str_eq(str_t str, str_t src)
{
	return str_eqc(str, src.data, src.len);
}

str_t str_cpy(str_t src)
{
	str_t copy = strz(src.len + 1);

	cstr_cpy((char *)copy.data, copy.size, src.data, src.len);
	((char *)copy.data)[src.len] = '\0';

	copy.len = src.len;

	return copy;
}

static int append(str_t *str, const char *cstr, size_t len)
{
	if (str == NULL) {
		return 1;
	}

	if (str->ref && str->size == 0) {
		if (str->data) {
			return 1;
		}

		str->data = cstr;
		str->len  = len;
		return 0;
	}

	if (str_catc(str, cstr, len) == NULL) {
		return 1;
	}

	return 0;
}

int str_split(str_t str, char c, str_t *l, str_t *r)
{
	const char *res = cstr_chr(str.data, c);

	if (!res || res >= str.data + str.len) {
		return 1;
	}

	if (l && append(l, str.data, res - str.data)) {
		return 1;
	}

	if (r && append(r, res + 1, str.len - (res + 1 - str.data))) {
		return 1;
	}

	return 0;
}

int str_rsplit(str_t str, char c, str_t *l, str_t *r)
{
	const char *res = cstr_rchr(str.data, c);

	if (!res || res >= str.data + str.len) {
		return 1;
	}

	if (l && append(l, str.data, res - str.data)) {
		return 1;
	}

	if (r && append(r, res + 1, str.len - (res + 1 - str.data))) {
		return 1;
	}

	return 0;
}