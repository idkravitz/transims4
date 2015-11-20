//*********************************************************
//	Ext_Header.cpp - Partitioned Db_Header Class
//*********************************************************

#include "Ext_Header.hpp"

#include "Utility.hpp"

char Ext_Header::buffer [] = "";

//---------------------------------------------------------
//	Ext_Header constructors
//---------------------------------------------------------

Ext_Header::Ext_Header (Access_Type access, Format_Type format) : Db_Header (access, format)
{
	pathname = NULL;
	extend = 0;
}

Ext_Header::Ext_Header (char *rootname, Access_Type access, Format_Type format) : Db_Header (access, format)
{
	pathname = NULL;
	extend = 0;

	Filename (rootname);
}

//---------------------------------------------------------
//	Ext_Header destructor
//---------------------------------------------------------

Ext_Header::~Ext_Header (void)
{
	if (pathname != NULL) {
		delete [] pathname;
	}
}

//---------------------------------------------------------
//	Filename
//---------------------------------------------------------

bool Ext_Header::Filename (char *rootname, int ext_number)
{
	extend = 0;

	if (pathname != NULL) {
		delete [] pathname;
		pathname = NULL;
	}
	extension [0] = '\0';

	Db_File::Filename (NULL);

	if (rootname == NULL) return (false);

	int len = (int) strlen (rootname) + 1;
	pathname = new char [len];
	if (pathname == NULL) return (Status (MEMORY));
	str_cpy (pathname, len, rootname);

	char *str_ptr = strstr (pathname, ".*");

	if (str_ptr != NULL) {
		extend = 1;
		*str_ptr = '\0';
	} else {
		str_ptr = strstr (pathname, ".t*");

		if (str_ptr != NULL) {
			extend = 2;
			*str_ptr = '\0';
		} else {
			extend = 0;
		}
	}
	if (ext_number >= 0 && extend) {
		Extension (ext_number);
		extend = 0;

		len = (int) (strlen (pathname) + strlen (buffer) + 2);
		str_ptr = new char [len];
		if (str_ptr == NULL) return (Status (MEMORY));

		str_fmt (str_ptr, len, "%s.%s", pathname, buffer);

		delete [] pathname;
		pathname = str_ptr;
	}
	return (Filename (0));
}

bool Ext_Header::Filename (int ext_number)
{
	Db_File::Filename (NULL);
	extension [0] = '\0';

	if (extend) {
		if (ext_number < 0) {
			return (Status (EXTENSION));
		} else {
			char name [FILE_NAME_SIZE];

			str_fmt (extension, sizeof (extension), Extension (ext_number));

			str_fmt (name, sizeof (name), "%s.%s", pathname, extension);

			return (Db_File::Filename (name));
		}
	} else if (ext_number != 0) {
		return (Status (EXTENSION));
	} else {
		return (Db_File::Filename (pathname));
	}
}

//---------------------------------------------------------
//	Extension
//---------------------------------------------------------

char * Ext_Header::Extension (int ext_number)
{
	buffer [0] = '\0';

	if (extend && ext_number >= 0) {
		if (extend == 1) {
			str_fmt (buffer, sizeof (buffer), "%c%c", ('A' + (ext_number / 26)), ('A' + (ext_number % 26)));
		} else {
			str_fmt (buffer, sizeof (buffer), "t%c%c", ('A' + (ext_number / 26)), ('A' + (ext_number % 26)));
		}
	}
	return (buffer);
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Ext_Header::Open (int ext_number)
{
	bool stat, send;

	send = Send_Messages ();

	Send_Messages (false);

	stat = Filename (ext_number);

	if (stat) {
		stat = Open ();
	}
	Send_Messages (send);

	return (stat);
}

//---------------------------------------------------------
//	Groupname
//---------------------------------------------------------

char * Ext_Header::Groupname (void)
{
	if (extend == 1) {
		return (str_fmt (buffer, sizeof (buffer), "%s.*", pathname));
	} else if (extend == 2) {
		return (str_fmt (buffer, sizeof (buffer), "%s.t*", pathname));
	} else {
		return (pathname);
	}
}
