//*********************************************************
//	Scanf.cpp - tab and column scan function
//*********************************************************

#include "Utility.hpp"

#include "stdlib.h"
#include "stdarg.h"
#include "string.h"

#define SCAN_SUPPRESS	0x01
#define SCAN_SHORTVAL	0x02
#define SCAN_LONGVAL	0x04
#define SCAN_LONGDBL	0x08

#define SCAN_CHAR		1
#define SCAN_INT		2
#define SCAN_FLOAT		3
#define SCAN_STRING		4
#define SCAN_UNSIGN		5

//---------------------------------------------------------
//	tscanf -- tab scanf
//---------------------------------------------------------

int tscanf (const char *string, const char *format, ...)
{
	va_list args;

	int c, width, flags, type, quote, nread;
	int nassigned;	// number of fields assigned

	char *str, *fmt, *buf, buffer [256];

	nassigned = 0;

	str = (char *) string;
	fmt = (char *) format;
	va_start(args, format);

	for (;;) {
		c = *fmt++;

		//--- check for end of format string ----

		if (c == '\0') {
			goto finish;
		}

		//--- check for a special character ----

		if (c != '%') {
			if (*str++ != c) {
				goto finish;
			}
			continue;
		}

		//---- process a field command ----

		width = 0;
		flags = 0;
		type = 0;

next:
		c = *fmt++;

		switch (c) {
			case '%':
				if (*str++ != c) {
					goto finish;
				}
				continue;

			case '+':
			case '-':
				goto next;
			case '*':
				flags |= SCAN_SUPPRESS;
				goto next;
			case 'l':
				flags |= SCAN_LONGVAL;
				goto next;
			case 'L':
				flags |= SCAN_LONGDBL;
				goto next;
			case 'h':
				flags |= SCAN_SHORTVAL;
				goto next;

			case '.':
				c = *fmt++;

				if (c == '*') goto next;

				if (c >= '0' && c <= '0') {
					while (c >= '0' && c <= '9') {
						c = *fmt++;
					}
					fmt--;
				}
				goto next;

			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				width = width * 10 + c - '0';
				goto next;

			case 'c':
				type = SCAN_CHAR;
				break;

			case 'D':
			case 'I':
				flags |= SCAN_LONGDBL;
			case 'd':
			case 'i':
				type = SCAN_INT;
				break;

			case 'f':
				type = SCAN_FLOAT;
				break;

			case 's':
				type = SCAN_STRING;
				break;

			case 'U':
				flags |= SCAN_LONGDBL;
			case 'u':
				type = SCAN_UNSIGN;
				break;

			default:
				goto finish;
		}

		if (*str == '\0') goto finish;

		//---- read the field into a string buffer ----

		buf = buffer;

		if (width == 0) width = sizeof (buffer);
		nread = 0;
		quote = 0;

		while ((*str != '\t' || quote) && *str != '\n' && *str != '\0' && nread < width) {
			if (*str == '"') {
				quote = !quote;
				str++;
			} else {
				*buf++ = *str++;
				nread++;
			}
		}
		*buf = '\0';
		nassigned++;

		if (flags & SCAN_SUPPRESS || type == 0) continue;

		switch (type) {
			case SCAN_CHAR:
				*va_arg (args, char *) = buffer [0];
				break;

			case SCAN_INT:
				if (flags & SCAN_SHORTVAL) {
					*va_arg(args, short *) = (short) atoi (buffer);
				} else if (flags & SCAN_LONGVAL) {
					*va_arg(args, long *) = (long) atol (buffer);
				} else {
					*va_arg(args, int *) = atoi (buffer);
				}
				break;

			case SCAN_FLOAT:
				if (flags & SCAN_SHORTVAL) {
					*va_arg(args, float *) = (float) atof (buffer); 
				} else {
					*va_arg(args, double *) = (double) atof (buffer);
				}
				break;

			case SCAN_STRING:
				str_cpy (va_arg(args, char *), nread+1, buffer);
				break;

			case SCAN_UNSIGN:
				if (flags & SCAN_SHORTVAL) {
					*va_arg(args, unsigned short *) = (unsigned short) strtoul (buffer, &buf, 10);
				} else if (flags & SCAN_LONGVAL) {
					*va_arg(args, unsigned long *) = (unsigned long) strtoul (buffer, &buf, 10);
				} else {
					*va_arg(args, unsigned int *) = (unsigned int) strtoul (buffer, &buf, 10);
				}
				break;

			default:
				break;
		}
	}

finish:
	va_end(args);
	return (nassigned);
}

//---------------------------------------------------------
//	cscanf - comma scanf
//---------------------------------------------------------

int cscanf (const char *string, const char *format, ...)
{
	va_list args;

	int c, width, flags, type, nread;
	int nassigned;	// number of fields assigned

	char *str, *fmt, *buf, buffer [256];

	nassigned = 0;

	str = (char *) string;
	fmt = (char *) format;
	va_start(args, format);

	for (;;) {
		c = *fmt++;

		//--- check for end of format string ----

		if (c == '\0') {
			goto finish;
		}

		//--- check for a special character ----

		if (c != '%') {
			if (*str++ != c) {
				goto finish;
			}
			continue;
		}

		//---- process a field command ----

		width = 0;
		flags = 0;
		type = 0;

next:
		c = *fmt++;

		switch (c) {
			case '%':
				if (*str++ != c) {
					goto finish;
				}
				continue;

			case '+':
			case '-':
				goto next;
			case '*':
				flags |= SCAN_SUPPRESS;
				goto next;
			case 'l':
				flags |= SCAN_LONGVAL;
				goto next;
			case 'L':
				flags |= SCAN_LONGDBL;
				goto next;
			case 'h':
				flags |= SCAN_SHORTVAL;
				goto next;

			case '.':
				c = *fmt++;

				if (c == '*') goto next;

				if (c >= '0' && c <= '0') {
					while (c >= '0' && c <= '9') {
						c = *fmt++;
					}
					fmt--;
				}
				goto next;

			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				width = width * 10 + c - '0';
				goto next;

			case 'c':
				type = SCAN_CHAR;
				break;

			case 'D':
			case 'I':
				flags |= SCAN_LONGDBL;
			case 'd':
			case 'i':
				type = SCAN_INT;
				break;

			case 'f':
				type = SCAN_FLOAT;
				break;

			case 's':
				type = SCAN_STRING;
				break;

			case 'U':
				flags |= SCAN_LONGDBL;
			case 'u':
				type = SCAN_UNSIGN;
				break;

			default:
				goto finish;
		}

		if (*str == '\0') goto finish;

		//---- read the field into a string buffer ----

		buf = buffer;

		if (width == 0) width = 1;
		nread = 0;

		while (*str != '\n' && *str != '\0' && nread < width) {
			*buf++ = *str++;
			nread++;
		}
		*buf = '\0';
		nassigned++;

		if (flags & SCAN_SUPPRESS || type == 0) continue;

		switch (type) {
			case SCAN_CHAR:
				*va_arg (args, char *) = buffer [0];
				break;

			case SCAN_INT:
				if (flags & SCAN_SHORTVAL) {
					*va_arg(args, short *) = (short) atoi (buffer);
				} else if (flags & SCAN_LONGVAL) {
					*va_arg(args, long *) = (long) atol (buffer);
				} else {
					*va_arg(args, int *) = atoi (buffer);
				}
				break;

			case SCAN_FLOAT:
				if (flags & SCAN_SHORTVAL) {
					*va_arg(args, float *) = (float) atof (buffer); 
				} else {
					*va_arg(args, double *) = (double) atof (buffer);
				}
				break;

			case SCAN_STRING:
				str_cpy (va_arg(args, char *), nread+1, buffer);
				break;

			case SCAN_UNSIGN:
				if (flags & SCAN_SHORTVAL) {
					*va_arg(args, unsigned short *) = (unsigned short) strtoul (buffer, &buf, 10);
				} else if (flags & SCAN_LONGVAL) {
					*va_arg(args, unsigned long *) = (unsigned long) strtoul (buffer, &buf, 10);
				} else {
					*va_arg(args, unsigned int *) = (unsigned int) strtoul (buffer, &buf, 10);
				}
				break;

			default:
				break;
		}
	}

finish:
	va_end(args);
	return (nassigned);
}

