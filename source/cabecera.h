#ifndef HEADER
#define HEADER

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//logger configuracion
#define SHOW_THREAD_ID 0

//tamanos
#define TAM_MENSAJE 255
#define MAX_TAM_PROC 85
#define MAX_PROCESOS 20
#define MAX_TAM_PROG 30
#define MAX_TAM_STACK 55

//tipos
typedef long int palabra;
typedef int flag;
typedef char string[TAM_MENSAJE];
// #define SUCCESS 0
// #define FAIL 1
typedef enum{
    SUCCESS,
    FAIL
} FlagValues;

extern flag debugMode;


#define KERNEL_MODE 1000000, 0, 2000


#endif