# CBuilder

This project is highly inspired by [Tsoding nob.h](https://github.com/tsoding/nob.h/) philosophy.
The goal is to compile C code in C without the need of additional tooling such as Makefile or Ninja

## Example

Create a CBuilder.c file

main function signature should be ==int main(int, char **)==
rules function signatures should be ==int fn(char **)==

``` C
#define CBUILDER_IMPLEMENTATION
#include "CBuilder.h"

int somefunction(char **av);

int main(int ac, char **av)
{
    CB_AddRule("ruleName", somefunction, "rule description");
    return 0;
}
```


