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
		#define CB_COLOR_GRAY		"\033[90m"
		#define CB_COLOR_GREEN		"\033[32m"
		#define CB_COLOR_YELLOW		"\033[33m"
		#define CB_COLOR_ORANGE		"\033[38;5;208m"
		#define CB_COLOR_CYAN		"\033[36m"
		#define CB_COLOR_MAGENTA	"\033[35m"
		#define CB_COLOR_RED		"\033[31m"
		#define CB_COLOR_RESET		"\033[0m"
	#endif
#endif

#if !defined(CB_USE_COLORS)
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
	#define GetFileTime		CB_GetFileTime
	#define AddRule			CB_AddRule
	#define LOG				CB_LOG
	#define SET_LOG_LEVEL	CB_SET_LOG_LEVEL
#endif	// !CB_STRIP_PREFIX

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/stat.h>

#define CB_ALLOC	malloc
#define CB_REALLOC	realloc
#define CB_FREE		free
#define CB_STRDUP	strdup
#define CB_STRCMP	strcmp
#define CB_PRINTF	printf


#define LOG_LEVELS(X)								\
	X(NONE,		"",			0,	"")					\
	X(TRACE,	"TRACE",	1,	CB_COLOR_GRAY)		\
	X(DEBUG,	"DEBUG",	2,	CB_COLOR_ORANGE)	\
	X(INFO,		"INFO",		4,	CB_COLOR_CYAN)		\
	X(WARN,		"WARN",		8,	CB_COLOR_YELLOW)	\
	X(ERROR,	"ERROR",	16,	CB_COLOR_RED)		\
	X(FATAL,	"FATAL",	32,	CB_COLOR_MAGENTA)

enum CB_log_level
{
	#define X(sym, name, val, color)	LOG_##sym = val,
		LOG_LEVELS(X)
	#undef X
};

#define	CB_SET_LOG_LEVEL(lvl)	\
	do							\
	{							\
		CB_g_log_level = lvl;	\
	}	while (0)

static const char * _CB_log_level_name(int lvl)
{
	switch (lvl)
	{
		#define X(sym, name, val, color)	case LOG_##sym : return name;
			LOG_LEVELS(X)
		#undef X
		default: return "UNKNOWN";
	}
}

static const char * _CB_log_level_color(int lvl)
{
	switch (lvl)
	{
		#define X(sym, name, val, color)	case LOG_##sym : return color;
			LOG_LEVELS(X)
		#undef X
		default: return CB_COLOR_RESET;
	}
}	

#define CB_LOG(lvl, fmt, ...)														\
	do																				\
	{																				\
		if ((lvl) >= CB_g_log_level)												\
		{																			\
			CB_PRINTF("%s[%s]: " fmt "\n%s",										\
				_CB_log_level_color(lvl),											\
				_CB_log_level_name(lvl),											\
				##__VA_ARGS__,														\
				CB_COLOR_RESET);													\
		}																			\
	}	while (0)

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

typedef struct CB_Rule
{
	char *	name;
	char *	desc;
	int		(*fn)(char ** av);
}	CB_Rule;

DA_DECL(CB_Rule, CB_Rules);


#endif	// !CBUILDER_H


#ifdef CBUILDER_IMPLEMENTATION

CB_Rules	CB_gRules = {0};
int			CB_g_log_level = LOG_DEBUG;

time_t	CB_GetFileTime(char *path)
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

static int _CB_SaveOldCBuilder(void)
{
	return 1;
}

static int	_CB_RebuildSelf(char **av)
{
	if (CB_GetFileTime(CB_BUILDER_C_FILE)	<= CB_GetFileTime(CB_BUILDER_EXEC_FILE) &&
		CB_GetFileTime(CB_BUILDER_H_FILE)	<= CB_GetFileTime(CB_BUILDER_EXEC_FILE))
	{
		return (0);
	}
	CB_LOG(LOG_INFO, CB_COLOR_RESET "%s %s -o %s", CB_CC, CB_BUILDER_C_FILE, CB_BUILDER_EXEC_FILE);
	int ret = system(CB_CC " " CB_BUILDER_C_FILE " -o " CB_BUILDER_EXEC_FILE);
	return (ret == 0);
}


static int	_CB_ManageRules(int ac, char **av)
{
	if (ac < 2) return 0;

	for (int i = 0; i < CB_gRules.count; i++)
	{
		if (CB_STRCMP(CB_gRules.data[i].name, av[1]) == 0)
		{
			CB_gRules.data[i].fn(av + 1);
		}
	}
	return 0;
}

int	CB_AddRule(char *name, int (*fn)(char ** av), char *desc)
{
	if (!CB_gRules.data && !CB_gRules.capacity && !CB_gRules.count)
	{
		DA_INIT(CB_gRules, 10);
	}
	CB_Rule r;
	r.name = name ? CB_STRDUP(name) : 0;
	r.desc = desc ? CB_STRDUP(desc) : 0;
	r.fn = fn;
	DA_APPEND(CB_gRules, r);
}

#ifndef CB_WRAP_MAIN_ONCE
#define CB_WRAP_MAIN_ONCE

	int	_usr_main_(int, char **);
	
	int main(int ac, char **av)
	{
		_CB_SaveOldCBuilder();
		if (_CB_RebuildSelf(av))
		{
			fflush(NULL);
			CB_EXECV("./"CB_BUILDER_EXEC_FILE, av);
			return (0);
		}
		_usr_main_(ac, av);
		_CB_ManageRules(ac, av);
		return (0);
	}
	#define main _usr_main_

#endif	// !CB_WRAP_MAIN_ONCE

#endif	// !CBUILDER_IMPLEMENTATION
