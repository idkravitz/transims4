//*********************************************************
//	Shapename.cpp - construct the shape file name
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Shapename
//---------------------------------------------------------

char * ArcNet::Shapename (char *arcdir, char *filename)
{
	char *str_ptr, buffer [FILE_NAME_SIZE];
	int len;

	str_cpy (buffer, sizeof (buffer), filename);

	str_ptr = strrchr (buffer, '\\');

	if (str_ptr == NULL) {
		str_ptr = strrchr (buffer, '/');
	}
	if (str_ptr == NULL) {
		str_ptr = buffer;
	} else {
		str_ptr++;
	}
	len = (int) strlen (str_ptr);

	if (len > 4) {
		char *ptr = strrchr (str_ptr + len - 4, '.');

		if (ptr != NULL) {
			*ptr = '\0';
		}
	}
	str_cpy (shapename, sizeof (shapename), arcdir);
	str_cat (shapename, sizeof (shapename), str_ptr);
	str_cat (shapename, sizeof (shapename), ".shp");

	return (shapename);
}
