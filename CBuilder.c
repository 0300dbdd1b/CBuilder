#define CBUILDER_IMPLEMENTATION
#define CB_USE_COLORS
#define CB_STRIP_PREFIX
#include "CBuilder.h"


int	testfn(char **av)
{
	for (int i = 0; av[i]; i++)
		log(LOG_CMD, "%s", av[i]);
	return 1;
}

int printhello(char **av)
{
	log(LOG_INFO, "hello");
	return 1;
}

int main(int ac, char **av)
{
	add_rule("hey", testfn, 0);
	add_rule("hello", printhello, 0);
	add_rule("hi", printhello, 0);
}

