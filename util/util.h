#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#define PRINTF_BUF_SZ 256
#define STDWIDTH 40
#define DONEMSG "[DONE]\n"

#define uabs(v) ((v) > 0 ? (v) : -(v))
int itoa(int n, char s[]);
int lprintf(const char * fmt, ...);
void printw(char* str);

#endif//__UTIL_H__
