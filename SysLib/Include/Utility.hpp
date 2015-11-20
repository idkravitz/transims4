//*********************************************************
//	Utility.hpp - utility functions
//*********************************************************

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "System_Defines.hpp"

#include <stdio.h>
#include <stdarg.h>

char * str_cpy (char des [], int size, char *src, size_t len = 0);
char * str_cat (char des [], int size, char *src, size_t len = 0);

char * str_fmt (char des [], int size, char *format, ...);
char * str_args (char des [], int size, char *format, va_list args);

int str_cmp (char *str1, char *str2, size_t len = 0);

FILE *f_open (char *filename, char *control);

char * Get_Integer (char *string, int *number);
char * Get_Double (char *string, double *number);
char * Get_Range (char *string, double *low, double *high);
char * Get_Token (char *string, char token [], int max_size = FIELD_BUFFER);

#endif
