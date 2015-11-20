//********************************************************* 
//	Utility.cpp - utility functions
//*********************************************************

#include "Utility.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//---------------------------------------------------------
//	str_cpy - safe string copy
//---------------------------------------------------------

char * str_cpy (char des [], int size, char *src, size_t len) 
{
#ifdef _MSC_EXTENSIONS
	if (len == 0) len = _TRUNCATE;
	strncpy_s (des, size, src, len);	
#else
	if (len == 0) len = size - 1;
	strncpy (des, src, len);
	des [len] = '\0';
#endif
	return (des);
}

//---------------------------------------------------------
//	str_cat - safe string concatination
//---------------------------------------------------------

char * str_cat (char des [], int size, char *src, size_t len) 
{
#ifdef _MSC_EXTENSIONS
	if (len == 0) len = _TRUNCATE;
	strncat_s (des, size, src, len);	
#else
	if (len == 0) {
		len = size - strlen (des) - 1;
	} else {
		len = MIN (size - strlen (des) - 1, len);
	}
	strncat (des, src, len);
	des [size-1] = '\0';
#endif
	return (des);
}

//---------------------------------------------------------
//	str_fmt - safe string formating
//---------------------------------------------------------

char * str_fmt (char des [], int size, char *format, ...) 
{
	if (des != NULL) {
		*des = '\0';

		if (format != NULL) {
			va_list  args;

			va_start (args, format);
#ifdef _MSC_EXTENSIONS
			vsprintf_s (des, size, format, args);
#else
			vsprintf (des, format, args);
#endif
			va_end (args);
		}
	}
	return (des);
}

//---------------------------------------------------------
//	str_args - safe string formating with arguments
//---------------------------------------------------------

char * str_args (char des [], int size, char *format, va_list args) 
{
	if (des != NULL) {
		*des = '\0';

		if (format != NULL) {
#ifdef _MSC_EXTENSIONS
			vsprintf_s (des, size, format, args);
#else
			vsprintf (des, format, args);
#endif
		}
	}
	return (des);
}

//---------------------------------------------------------
//	str_cmp - string compare
//---------------------------------------------------------

int str_cmp (char *str1, char *str2, size_t len) 
{
#ifdef _WIN32
	if (len == 0) {
		return (_stricmp (str1, str2));
	} else {
		return (_strnicmp (str1, str2, len));
	}
#else
	if (len == 0) {
		return (strcasecmp (str1, str2));
	} else {
		return (strncasecmp (str1, str2, len));
	}
#endif
}

//---------------------------------------------------------
//	f_open - safe file open
//---------------------------------------------------------

FILE *f_open (char *filename, char *control)
{
#ifdef _MSC_EXTENSIONS
	FILE *file;

	if (fopen_s (&file, filename, control) != 0) {
		return (NULL);
	} else {
		return (file);
	}
#else
	return (fopen (filename, control));
#endif
}

//---------------------------------------------------------
//      Get_Integer
//---------------------------------------------------------

char * Get_Integer (char *string, int *number)
{
	char buffer [FIELD_BUFFER];

	string = Get_Token (string, buffer, sizeof (buffer));

	*number = atoi (buffer);

	return (string);
}

//---------------------------------------------------------
//      Get_Double
//---------------------------------------------------------

char * Get_Double (char *string, double *number)
{
	char buffer [FIELD_BUFFER];

	string = Get_Token (string, buffer, sizeof (buffer));

	*number = atof (buffer);

	return (string);
}

//----------------------------------------------------------
//      Get_Range
//----------------------------------------------------------

char * Get_Range (char *string, double *low, double *high)
{
	char range [FIELD_BUFFER], *ptr, *colon;

	*low = *high = 0.0;

	string = Get_Token (string, range);

	//---- search for the range symbol ----

	ptr = range;
	if (*ptr == '\0') return (string);

	while (ptr != NULL) {
		ptr = strchr (ptr, '.');
		if (ptr == NULL || *(ptr+1) == '.') break;
		ptr++;
	}

	if (ptr != NULL) {
		*ptr++ = '\0';
	
		while (*ptr == '.') {
			ptr++;
		}
	}

	//---- get the low value ----

	colon = strchr (range, ':');
	if (colon == NULL) {
		*low = atof (range);
	} else {
		*colon++ = '\0';
		*low = atoi (range) + atof (colon) / 60.0;
	}

	//---- get the high value ----

	if (ptr == NULL) {
		if (*low == 0.0) {
			*high = 0.0001;
		} else {
			*high = *low;
		}
	} else {
		colon = strchr (ptr, ':');
		if (colon == NULL) {
			*high = atof (ptr);
		} else {
			*colon++ = '\0';
			*high = atoi (ptr) + atof (colon) / 60.0;
		}
	}
	return (string);
}

//----------------------------------------------------------
//      Get_Token
//----------------------------------------------------------

char * Get_Token (char *string, char token [], int max_size)
{
	token [0] = '\0';
	if (string == NULL) return (NULL);

	while (*string == ' ' || *string == '\t' || *string == ',') string++;

	bool quote, tick;
	char *ptr = token;

	quote = tick = false;

	if (*string == '\'') {
		tick = true;
		string++;
	}

	while (*string != '\0' && *string != '\n' && *string != '\r') {
		if (quote) {
			if (*string == '"') {
				string++;
				quote = false;
				continue;
			}
		} else if (tick) {
			if (*string == '\'') {
				string++;
				break;
			}
		} else if (*string == '"') {
			quote = true;
			string++;
			continue;
		} else if (*string == ' ' || *string == '\t' || *string == ',' ||
			(*string == '#' && (*(string+1) == '#' || *(string+1) == '-' || *(string+1) == '*')) ||
			(*string == '/' && (*(string+1) == '/' || *(string+1) == '-' || *(string+1) == '*')) ||
			(*string == ';' && (*(string+1) == ';' || *(string+1) == '-' || *(string+1) == '*'))) {
			break;
		}
		if (ptr - token < max_size) *ptr++ = *string;
		string++;
	}
	*ptr = '\0';
	
	if (*string == '\0' || *string == '\n' || *string == '\r') {
		return (NULL);
	}
	return (string);	
}
