#include "log.h"

#include "c_time.h"

static log_t *s_log;

static const char *level_strings[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

static const char *level_colors[] = { "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m" };

static void stdout_callback(log_event_t *ev)
{
	fprintf(ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", ev->time, level_colors[ev->level], level_strings[ev->level], ev->file, ev->line);
	vfprintf(ev->udata, ev->fmt, ev->ap);
	fprintf(ev->udata, "\n");
	fflush(ev->udata);
}

static void file_callback(log_event_t *ev)
{
	fprintf(ev->udata, "%s %-5s %s:%d: ", ev->time, level_strings[ev->level], ev->file, ev->line);
	vfprintf(ev->udata, ev->fmt, ev->ap);
	fprintf(ev->udata, "\n");
	fflush(ev->udata);
}

void log_init(log_t *log)
{
	s_log = log;
}

const char *log_level_string(int level)
{
	return level_strings[level];
}

void log_set_level(int level)
{
	s_log->level = level;
}

void log_set_quiet(int enable)
{
	s_log->quiet = enable;
}

int log_add_callback(log_LogFn fn, void *udata, int level)
{
	for (int i = 0; i < LOG_MAX_CALLBACKS; i++) {
		if (!s_log->callbacks[i].fn) {
			s_log->callbacks[i] = (callback_t){ fn, udata, level };
			return 0;
		}
	}
	return -1;
}

int log_add_fp(FILE *fp, int level)
{
	return log_add_callback(file_callback, fp, level);
}

static void init_event(log_event_t *ev, void *udata)
{
	if (!ev->time[0]) {
		c_time_str(ev->time);
	}
	ev->udata = udata;
}

void log_log(int level, const char *file, int line, const char *fmt, ...)
{
	if (s_log == NULL) {
		return;
	}

	log_event_t ev = {
		.fmt   = fmt,
		.file  = file,
		.line  = line,
		.level = level,
	};

	if (!s_log->quiet && level >= s_log->level) {
		init_event(&ev, stderr);
		va_start(ev.ap, fmt);
		stdout_callback(&ev);
		va_end(ev.ap);
	}

	for (int i = 0; i < LOG_MAX_CALLBACKS && s_log->callbacks[i].fn; i++) {
		callback_t *cb = &s_log->callbacks[i];
		if (level >= cb->level) {
			init_event(&ev, cb->udata);
			va_start(ev.ap, fmt);
			cb->fn(&ev);
			va_end(ev.ap);
		}
	}
}
