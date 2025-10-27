#if 0
	#!/bin/sh
	gcc CBuilder.c -o CBuilder && ./CBuilder
	exit
#endif
#define CBUILDER_IMPLEMENTATION
#define CB_USE_COLORS
#define CB_STRIP_PREFIX
#include "CBuilder.h"


int	testfn(char **av)
{
	cb_cmd cmd;
	DA_INIT(cmd, 10);
	for (int i = 0; av[i]; i++)
		DA_APPEND(cmd, av[i]);
	exec_cmd(&cmd);
	return 1;
}

int printhello(char **av)
{
	cb_cmd cmd;
	if (has_arg(av, "world"))
		cmd = cb_build_cmd("echo", "hello", "world");
	else
		cmd = cb_build_cmd("echo", "hello");
	cb_exec_cmd(&cmd);
	return 1;
}

int main(int ac, char **av)
{
	add_rule("hey", testfn, 0);
	add_rule("hello", printhello, 0);
	add_rule("hi", printhello, 0);
}

