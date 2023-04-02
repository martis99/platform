#include "mem.h"

#include "platform.h"
#include "print.h"

#include <memory.h>
#include <stdlib.h>

static m_stats_t *s_stats;

void m_init(m_stats_t *stats)
{
	s_stats = stats;
}

const m_stats_t *m_get_stats()
{
	return s_stats;
}

void m_print(FILE *file)
{
	size_t mem_max = s_stats->mem_max;
	char s	       = '\0';

	if (mem_max > 1024 * 1024) {
		mem_max /= 1024 * 1024;
		s = 'M';
	} else if (mem_max > 1024) {
		mem_max /= 1024;
		s = 'K';
	}

	if (s == '\0') {
		p_fprintf(file, "mem:      %zu max: %zu B\n", s_stats->mem, s_stats->mem_max);
	} else {
		p_fprintf(file, "mem:      %zu max: %zu %cB (%zu B)\n", s_stats->mem, mem_max, s, s_stats->mem_max);
	}
	p_fprintf(file, "allocs:   %d\n", s_stats->allocs);
	p_fprintf(file, "reallocs: %d\n", s_stats->reallocs);
}

#define MAX(a, b) (a) > (b) ? (a) : (b)

void *m_malloc(size_t size)
{
	s_stats->mem += size;
	s_stats->mem_max = MAX(s_stats->mem, s_stats->mem_max);
	s_stats->allocs++;

	return malloc(size);
}

void *m_calloc(size_t count, size_t size)
{
	s_stats->mem += count * size;
	s_stats->mem_max = MAX(s_stats->mem, s_stats->mem_max);
	s_stats->allocs++;

	return calloc(count, size);
}

void *m_realloc(void *memory, size_t new_size, size_t old_size)
{
	s_stats->mem -= old_size;
	s_stats->mem += new_size;
	s_stats->mem_max = MAX(s_stats->mem, s_stats->mem_max);
	s_stats->reallocs++;

	return realloc(memory, new_size);
}

void *m_memset(void *dst, int val, size_t size)
{
	return memset(dst, val, size);
}

void *m_memcpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
#if defined(C_WIN)
	memcpy_s(dst, dst_size, src, src_size);
#else
	memcpy(dst, src, src_size);
#endif
	return dst;
}

int m_memcmp(const void *l, const void *r, size_t size)
{
	return memcmp(l, r, size);
}

void m_free(void *memory, size_t size)
{
	s_stats->mem -= size;
	free(memory);
}
