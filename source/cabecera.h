#ifndef HEADER
#define HEADER

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef long int palabra;
typedef int flag;

extern flag debugMode;
 
#define KERNEL_MODE 1000000, 0, 2000
#define SUCCESS 0
#define FAIL 1

#endif