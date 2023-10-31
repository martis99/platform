#include "print.h"

#include "file.h"
#include "log.h"
#include "platform.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <wchar.h>

#if defined(C_WIN)
	#include <io.h>
#else
	#include <locale.h>
#endif

int c_printv(const char *fmt, va_list args)
{
	if (fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	errno	= 0;
	int ret = vprintf(fmt, copy);
	if (ret < 0 && errno == 0) {
		file_reopen(NULL, "w", stdout);
		ret = vprintf(fmt, copy);
	}
	va_end(copy);
	return ret;
}

int c_printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_printv(fmt, args);
	va_end(args);
	return ret;
}

int c_fprintv(FILE *file, const char *fmt, va_list args)
{
	if (file == NULL || fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	int ret;
	errno = 0;
#if defined(C_WIN)
	ret = vfprintf_s(file, fmt, copy);
#else
	ret = vfprintf(file, fmt, copy);
#endif
	if (ret < 0) {
		int errnum = errno;
		log_error("cutils", "print", NULL, "failed to write to file: %s (%d)", log_strerror(errnum), errnum);
		ret = 0;
	}
	va_end(copy);
	return ret;
}

int c_fprintf(FILE *file, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_fprintv(file, fmt, args);
	va_end(args);
	return ret;
}

int c_sprintv(char *buf, size_t size, const char *fmt, va_list args)
{
	if ((buf == NULL && size > 0) || fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	int ret;
#if defined(C_WIN)
	ret = vsnprintf(buf, size / sizeof(char), fmt, copy);
#else
	ret = vsnprintf(buf, size / sizeof(char), fmt, copy);
#endif
	va_end(copy);

	if (size > 0 && (size_t)ret > size) {
		return 0;
	}

	return ret;
}

int c_sprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_sprintv(buf, size, fmt, args);
	va_end(args);
	return ret;
}

int c_wprintv(const wchar *fmt, va_list args)
{
	if (fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	errno	= 0;
	int ret = vwprintf(fmt, copy);
	if (ret < 0 && errno == 0) {
		file_reopen(NULL, "w", stdout);
		ret = vwprintf(fmt, copy);
	}

	va_end(copy);
	return ret;
}

int c_wprintf(const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_wprintv(fmt, args);
	va_end(args);
	return ret;
}

int c_swprintv(wchar *buf, size_t size, const wchar *fmt, va_list args)
{
	if ((buf == NULL && size > 0) || fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	int ret;
#if defined(C_WIN)
	ret = vswprintf_s(buf, size / sizeof(wchar_t), fmt, copy);
#else
	ret = vswprintf(buf, size / sizeof(wchar_t), fmt, copy);
#endif
	va_end(copy);
	return ret;
}

int c_swprintf(wchar *buf, size_t size, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_swprintv(buf, size, fmt, args);
	va_end(args);
	return ret;
}

int c_fflush(FILE *file)
{
	if (file == NULL) {
		return 1;
	}

	return fflush(file);
}

int c_set_u16(FILE *file)
{
	if (file == NULL) {
		return 0;
	}

#if defined(C_WIN)
	fflush(file);
	return _setmode(_fileno(file), _O_U16TEXT);
#else
	return 0;
#endif
}

int c_unset_u16(FILE *file, int mode)
{
	if (file == NULL) {
		return 0;
	}

#if defined(C_WIN)
	fflush(file);
	return _setmode(_fileno(file), mode);
#else
	(void)mode;
	return 0;
#endif
}

int c_ur(FILE *file)
{
	if (file == NULL) {
		return 0;
	}

	int ret;

#if defined(C_WIN)
	if (file == stdout || file == stderr) {
		int mode = c_set_u16(file);
		ret	 = fwprintf_s(file, L"\u2514\u2500");
		c_unset_u16(file, mode);
	} else {
		ret = fprintf_s(file, "└─");
	}
#else
	ret = fprintf(file, "└─");
#endif
	return ret;
}

int c_v(FILE *file)
{
	if (file == NULL) {
		return 0;
	}

	int ret;

#if defined(C_WIN)
	if (file == stdout || file == stderr) {
		int mode = c_set_u16(file);
		ret	 = fwprintf_s(file, L"\u2502 ");
		c_unset_u16(file, mode);
	} else {
		ret = fprintf_s(file, "│ ");
	}
#else
	ret = fprintf(file, "│ ");
#endif
	return ret;
}

int c_vr(FILE *file)
{
	if (file == NULL) {
		return 0;
	}

	int ret;

#if defined(C_WIN)
	if (file == stdout || file == stderr) {
		int mode = c_set_u16(file);
		ret	 = fwprintf_s(file, L"\u251C\u2500");
		c_unset_u16(file, mode);
	} else {
		ret = fprintf_s(file, "├─");
	}
#else
	ret = fprintf(file, "├─");
#endif
	return ret;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int c_sprintv_cb(void *buf, size_t size, int off, const char *fmt, va_list args)
{
	off = MIN((int)size, off);
	return c_sprintv((char *)buf + off, size - off, fmt, args);
}

int c_fprintv_cb(void *priv, size_t size, int off, const char *fmt, va_list args)
{
	(void)size;
	(void)off;
	return c_fprintv(priv, fmt, args);
}

int c_sprintf_cb(void *buf, size_t size, int off, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int len = c_sprintv_cb(buf, size, off, fmt, args);
	va_end(args);
	return len;
}

int c_fprintf_cb(void *priv, size_t size, int off, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int len = c_fprintv_cb(priv, size, off, fmt, args);
	va_end(args);
	return len;
}
