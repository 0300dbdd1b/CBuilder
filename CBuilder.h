#ifndef CBUILDER_H
#define CBUILDER_H

#if		defined(_WIN32)
	#define	CB_PLATFORM_WINDOWS
#elif	defined(__APPLE__)
	#define CB_PLATFORM_MACOS
	#define CB_PLATFORM_UNIX
#else
	#define CB_PLATFORM_LINUX
	#define CB_PLATFORM_UNIX
#endif

#if		defined(__clang__)
	#define	CB_CC	"clang"
#elif	defined(__GNUC__)
	#define CB_CC	"gcc"
#elif	defined(_MSC_VER)
	#define CB_CC	"msvc"
#endif

#ifndef	CB_BUILDER_NAME
	#define CB_BUILDER_NAME "CBuilder"
#endif

#if		defined(CB_PLATFORM_WINDOWS)
	#define CB_EXEC_EXTENSION	".exe"
	#define CB_PATH_SEP			"\\"
	#define CB_NEWLINE			"\r\n"
	#define CB_EXECV			_execv
	#if	defined(CB_USE_COLORS)
		#define CB_COLOR_GRAY		""
		#define CB_COLOR_GREEN		""
		#define CB_COLOR_YELLOW		""
		#define CB_COLOR_ORANGE		""
		#define CB_COLOR_CYAN		""
		#define CB_COLOR_MAGENTA	""
		#define CB_COLOR_RED		""
		#define CB_COLOR_RESET		""
	#endif
#else
	#define CB_EXEC_EXTENSION	""
	#define CB_PATH_SEP			"/"
	#define CB_NEWLINE			"\n"
	#define CB_EXECV			execv
	#if	defined(CB_USE_COLORS)
		#define CB_COLOR_GRAY		"\x1b[90m"
		#define CB_COLOR_GREEN		"\x1b[32m"
		#define CB_COLOR_YELLOW		"\x1b[33m"
		#define CB_COLOR_ORANGE		"\x1b[38;5;208m"
		#define CB_COLOR_CYAN		"\x1b[36m"
		#define CB_COLOR_MAGENTA	"\x1b[35m"
		#define CB_COLOR_RED		"\x1b[31m"
		#define CB_COLOR_RESET		"\x1b[0m"
	#endif
#endif

#if	!defined(CB_USE_COLORS)
	#define CB_COLOR_GRAY		""
	#define CB_COLOR_GREEN		""
	#define CB_COLOR_YELLOW		""
	#define CB_COLOR_ORANGE		""
	#define CB_COLOR_CYAN		""
	#define CB_COLOR_MAGENTA	""
	#define CB_COLOR_RED		""
	#define CB_COLOR_RESET		""
#endif

#define	CB_BUILDER_C_FILE		CB_BUILDER_NAME".c"
#define	CB_BUILDER_H_FILE		CB_BUILDER_NAME".h"
#define CB_BUILDER_EXEC_FILE	CB_BUILDER_NAME CB_EXEC_EXTENSION

#ifdef CB_STRIP_PREFIX
	#define log					cb_log
	#define get_file_time		cb_get_file_time
	#define mkdir_if_not_exists	cb_mkdir_if_not_exists
	#define add_rule			cb_add_rule
#endif	// !CB_STRIP_PREFIX

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>

#define CB_ALLOC	malloc
#define CB_REALLOC	realloc
#define CB_FREE		free
#define CB_STRDUP	strdup
#define CB_STRCMP	strcmp
#define CB_PRINTF	printf


/*************	LOG	*************/
extern unsigned int cb_g_log_level_mask;

#define LOG_LEVELS(X)									\
	X(NONE,		"NONE",		0,	CB_COLOR_RESET)			\
	X(TRACE,	"TRACE",	1,	CB_COLOR_GRAY)			\
	X(DEBUG,	"DEBUG",	2,	CB_COLOR_ORANGE)		\
	X(INFO,		"INFO",		4,	CB_COLOR_CYAN)			\
	X(WARN,		"WARN",		8,	CB_COLOR_YELLOW)		\
	X(ERROR,	"ERROR",	16,	CB_COLOR_RED)			\
	X(FATAL,	"FATAL",	32,	CB_COLOR_MAGENTA)		\
	X(CMD,		"CMD",		64,	CB_COLOR_CYAN)

enum cb_log_level
{
	#define X(sym, name, val, color)	LOG_##sym = val,
		LOG_LEVELS(X)
	#undef X
};

#define CB_LOG_NONE		0u
#define CB_LOG_ALL		(LOG_NONE | LOG_TRACE | LOG_DEBUG | LOG_INFO | LOG_WARN | LOG_ERROR | LOG_FATAL | LOG_CMD)

#define cb_log_set(mask)			\
	do								\
	{								\
		CB_g_log_level_mask = mask;	\
	}	while (0)

#define cb_log_enable(lvl)			\
	do								\
	{								\
		CB_g_log_level_mask |= lvl;	\
	}	while (0)

#define cb_log_disable(lvl)				\
	do									\
	{									\
		CB_g_log_level_mask &= ~(lvl);	\
	}	while (0)

#define cb_log_enabled(lvl) (((lvl) == LOG_NONE) || (cb_g_log_level_mask & (lvl)) != 0u)

static inline const char * cb_log_level_name(int lvl)
{
	switch (lvl)
	{
		#define X(sym, name, val, color)	case LOG_##sym : return name;
			LOG_LEVELS(X)
		#undef X
		default: return "UNKNOWN";
	}
}

static inline const char * cb_log_level_color(int lvl)
{
	switch (lvl)
	{
		#define X(sym, name, val, color)	case LOG_##sym : return color;
			LOG_LEVELS(X)
		#undef X
		default: return "";
	}
}

#define cb_log(lvl, fmt, ...)											\
	do																	\
	{																	\
		if (cb_log_enabled(lvl))										\
		{																\
			if ((lvl) == LOG_NONE)										\
			{															\
				CB_PRINTF(fmt CB_COLOR_RESET "\n",						\
					##__VA_ARGS__);										\
			}															\
			else if ((lvl) == LOG_CMD)									\
			{															\
				CB_PRINTF("%s[%s]: %s" fmt CB_COLOR_RESET "\n",			\
			  			cb_log_level_color(lvl),						\
						cb_log_level_name(lvl),							\
						CB_COLOR_RESET,									\
						##__VA_ARGS__);									\
			}															\
			else														\
			{															\
				CB_PRINTF("%s[%s]: " fmt CB_COLOR_RESET "\n",			\
					cb_log_level_color(lvl),							\
					cb_log_level_name(lvl),								\
					##__VA_ARGS__);										\
			}															\
		}																\
	}	while (0)


/*************	DA	*************/
#define DA_DECL(T, name)	\
	typedef struct			\
	{						\
		T *		data;		\
		int		count;		\
		int		capacity;	\
	}	name

#define DA_INIT(a, cap)												\
	do																\
	{																\
		(a).count = 0;												\
		(a).capacity = cap;											\
		(a).data = (cap) ? CB_ALLOC(sizeof(*(a).data) * cap) : 0;	\
	}	while (0)

#define DA_APPEND(a, value)														\
	do																			\
	{ 																			\
		if ((a).count >= (a).capacity)											\
		{																		\
			(a).capacity = (a).capacity ? (a).capacity * 2 : 1;					\
			void *tmp = CB_REALLOC((a).data, sizeof(*(a).data) * (a).capacity);	\
			(a).data = tmp;														\
		}																		\
		(a).data[(a).count++] = (value);										\
} while (0)

#define	DA_CLEAR(a)		\
	do					\
	{					\
		(a).count = 0;	\
	}	while (0)

#define	DA_FREE(a)			\
	do						\
	{						\
		CB_FREE((a).data);	\
		(a).data = 0;		\
		(a).count = 0;		\
		(a).capacity = 0;	\
	}	while (0)			\

typedef struct cb_rule
{
	char *	name;
	char *	desc;
	int		(*fn)(char ** av);
}	cb_rule;

DA_DECL(cb_rule, cb_rules);


#endif	// !CBUILDER_H


#ifdef CBUILDER_IMPLEMENTATION

cb_rules		cb_g_rules = {0};
unsigned int	cb_g_log_level_mask = CB_LOG_ALL;

time_t	cb_get_file_time(char *path)
{
	#if		defined(CB_PLATFORM_WINDOWS)
		struct _stat info;
		if (_stat(path, &info) == 0)
		{
			return info.st_mtime;
		}
		return (time_t)(-1);
	#elif	defined(CB_PLATFORM_UNIX)
		struct stat info;
		if (stat(path, &info) == 0)
		{
			return info.st_mtime;
		}
		return (time_t)(-1);
	#else
		return (time_t)(-1);
	#endif
}

static int _cb_save_old_builder(void)
{
	return 1;
}

static int	_cb_rebuild_self(char **av)
{
	if (cb_get_file_time(CB_BUILDER_C_FILE)	<= cb_get_file_time(CB_BUILDER_EXEC_FILE) &&
		cb_get_file_time(CB_BUILDER_H_FILE)	<= cb_get_file_time(CB_BUILDER_EXEC_FILE))
	{
		return (0);
	}
	cb_log(LOG_CMD, "%s %s -o %s", CB_CC, CB_BUILDER_C_FILE, CB_BUILDER_EXEC_FILE);
	int ret = system(CB_CC " " CB_BUILDER_C_FILE " -o " CB_BUILDER_EXEC_FILE);
	return (ret == 0);
}

static int _cb_find_rule_index(const char *name)
{
	if (!name) return -1;
	for (int k = 0; k < cb_g_rules.count; ++k)
	{
		const char *r = cb_g_rules.data[k].name;
		if (r && CB_STRCMP(r, name) == 0)
			return k;
	}
	return -1;
}

static int _cb_manage_rules(int ac, char **av)
{
	if (ac < 2 || cb_g_rules.count == 0) return 0;

	int status = 0;
	int i = 1;

	while (i < ac)
	{
		int ri = _cb_find_rule_index(av[i]);
		if (ri < 0)
		{
			i++;
			continue;
		}
		int start = i;
		int j = i + 1;
		while (j < ac && _cb_find_rule_index(av[j]) < 0)
			j++;
		char *saved = NULL;
		if (j < ac)
		{
			saved = av[j];
			av[j] = NULL;
		}
		char **rule_argv = &av[start + 1];
		int rc = cb_g_rules.data[ri].fn(rule_argv);
		if (rc != 0) status = rc;
		if (j < ac) av[j] = saved;
		i = j;
	}
	return status;
}

int	_cb_help_rule(char **av)
{
	for (int i = 0; i < cb_g_rules.count; i++)
	{
		cb_rule r = cb_g_rules.data[i];
		CB_PRINTF("%s\t:\t%s\n", r.name, r.desc);
	}
}

int	cb_add_rule(char *name, int (*fn)(char ** av), char *desc)
{
	if (!cb_g_rules.data && !cb_g_rules.capacity && !cb_g_rules.count)
	{
		DA_INIT(cb_g_rules, 10);
	}
	cb_rule r;
	r.name = name ? CB_STRDUP(name) : 0;
	r.desc = desc ? CB_STRDUP(desc) : 0;
	r.fn = fn;
	DA_APPEND(cb_g_rules, r);
}

#ifndef CB_WRAP_MAIN_ONCE
#define CB_WRAP_MAIN_ONCE

	#if !defined(CB_SRC)
		int	_usr_main_(int, char **);
		
		int main(int ac, char **av)
		{
			cb_add_rule("help", _cb_help_rule, "display this help message");
			_cb_save_old_builder();
			_usr_main_(ac, av);
			if (_cb_rebuild_self(av))
			{
				fflush(NULL);
				CB_EXECV("./"CB_BUILDER_EXEC_FILE, av);
				return (0);
			}
			_cb_manage_rules(ac, av);
			return (0);
		}

		#define main _usr_main_
	#endif
#endif	// !CB_WRAP_MAIN_ONCE

int cb_mkdir_if_not_exists(const char *path)
{
	#ifdef CB_PLATFORM_WINDOWS
		int result = _mkdir(path);
	#else
		int result = mkdir(path, 0755);
	#endif
	if (result < 0)
	{
		if (errno == EEXIST)
		{
			cb_log(LOG_INFO, "directory `%s` already exists", path);
			return 1;
		}
		cb_log(LOG_ERROR, "could not create directory `%s`: %s", path, strerror(errno));
		return 0;
	}
	cb_log(LOG_INFO, "created directory `%s`", path);
	return 1;
}

#endif	// !CBUILDER_IMPLEMENTATION
