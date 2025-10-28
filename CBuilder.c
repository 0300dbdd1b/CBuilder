#if 0
	#!/bin/sh
	gcc CBuilder.c -o CBuilder && ./CBuilder
	exit
#endif
#define CBUILDER_IMPLEMENTATION
#define CB_USE_COLORS
#define CB_STRIP_PREFIX
#include "CBuilder.h"
#include <fcntl.h>


int	testfn(char **av)
{
	cb_cmd cmd = build_cmd("echo hello world", NULL);
	for (int i = 0; av[i]; i++)
		add_to_cmd(&cmd, av[i]);
	exec_cmd(&cmd);
	return 1;
}

int	sleepr(char **av)
{
	cb_cmd cmd1 = build_cmd("sleep", "2");
	cb_cmd cmd2 = build_cmd("sleep", "2");

	exec_cmd(&cmd1);
	exec_cmd(&cmd2);
}

int printhello(char **av)
{
	cb_cmd cmd;
	if (has_arg(av, "world"))
		cmd = cb_build_cmd("echo", "hello", "world");
	else
		cmd = cb_build_cmd("echo", "hello");
	cb_exec_cmd(&cmd, .stdout_path = "test.txt");
	return 1;
}

int main(int ac, char **av)
{
	add_rule("hey", testfn, 0);
	add_rule("hello", printhello, 0);
	add_rule("hi", printhello, 0);
	add_rule("sleep", sleepr, 0);
}

