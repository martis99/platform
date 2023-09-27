#include "mem.h"

#include "platform.h"
#include "print.h"

#include <memory.h>
#include <stdlib.h>

static mem_stats_t *s_stats;
static int s_oom;

void mem_init(mem_stats_t *stats)
{
	s_stats = stats;
	s_oom	= 0;
}

const mem_stats_t *mem_get_stats()
{
	return s_stats;
}

void mem_print(FILE *file)
{
	size_t mem_max = s_stats->mem_max;
	char s	       = '\0';

	if (mem_max > (size_t)1024 * 1024) {
		mem_max /= (size_t)1024 * 1024;
		s = 'M';
	} else if (mem_max > 1024) {
		mem_max /= 1024;
		s = 'K';
	}

	if (s == '\0') {
		c_fprintf(file, "mem:      %zu max: %zu B\n", s_stats->mem, s_stats->mem_max);
	} else {
		c_fprintf(file, "mem:      %zu max: %zu %cB (%zu B)\n", s_stats->mem, mem_max, s, s_stats->mem_max);
	}
	c_fprintf(file, "allocs:   %d\n", s_stats->allocs);
	c_fprintf(file, "reallocs: %d\n", s_stats->reallocs);
}

#define MAX(a, b) (a) > (b) ? (a) : (b)

void *mem_alloc(size_t size)
{
	if (size > 0 && s_oom) {
		return NULL;
	}

	s_stats->mem += size;
	s_stats->mem_max = MAX(s_stats->mem, s_stats->mem_max);
	s_stats->allocs++;

	return malloc(size);
}

void *mem_calloc(size_t count, size_t size)
{
	if (count * size > 0 && s_oom) {
		return NULL;
	}

	s_stats->mem += count * size;
	s_stats->mem_max = MAX(s_stats->mem, s_stats->mem_max);
	s_stats->allocs++;

	return calloc(count, size);
}

void *mem_realloc(void *memory, size_t new_size, size_t old_size)
{
	if (new_size > old_size && s_oom) {
		return NULL;
	}

	s_stats->mem -= old_size;
	s_stats->mem += new_size;
	s_stats->mem_max = MAX(s_stats->mem, s_stats->mem_max);
	s_stats->reallocs++;

	return realloc(memory, new_size);
}

void *mem_set(void *dst, int val, size_t size)
{
	if (dst == NULL) {
		return NULL;
	}

	return memset(dst, val, size);
}

void *mem_cpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
#if defined(C_WIN)
	memcpy_s(dst, dst_size, src, src_size);
#else
	memcpy(dst, src, src_size);
#endif
	return dst;
}

int mem_cmp(const void *l, const void *r, size_t size)
{
	return memcmp(l, r, size);
}

int mem_swap(void *ptr1, void *ptr2, size_t size)
{
	if (ptr1 == NULL || ptr2 == NULL) {
		return 1;
	}

	unsigned char *byte_ptr1 = (unsigned char *)ptr1;
	unsigned char *byte_ptr2 = (unsigned char *)ptr2;

	for (size_t i = 0; i < size; i++) {
		unsigned char temp = byte_ptr1[i];
		byte_ptr1[i]	   = byte_ptr2[i];
		byte_ptr2[i]	   = temp;
	}

	return 0;
}

void mem_free(void *memory, size_t size)
{
	if (memory == NULL) {
		return;
	}

	s_stats->mem -= size;
	free(memory);
}

void mem_oom(int oom)
{
	s_oom = oom;
}
