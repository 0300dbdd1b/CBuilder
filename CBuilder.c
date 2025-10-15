#define CBUILDER_IMPLEMENTATION
#define CB_USE_COLORS
#define CB_STRIP_PREFIX
#include "CBuilder.h"


int	testfn(char **av)
{
	system("echo yes");
	return 1;
}

int printhello(char **av)
{
	printf("HELLO \n");
	return 1;
}

int main(int ac, char **av)
{
	AddRule("hey", testfn, 0);
	AddRule("hello", printhello, 0);
	AddRule("hi", printhello, 0);
	LOG(LOG_DEBUG, "Caca");
}

