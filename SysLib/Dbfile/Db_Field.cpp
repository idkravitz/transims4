//********************************************************* 
//	Db_Field.cpp - database field
//*********************************************************

#include "Db_Field.hpp"

#include "Utility.hpp"

#include <string.h>

//-----------------------------------------------------------
//	Db_Field constructor
//-----------------------------------------------------------

Db_Field::Db_Field (char *_name, Field_Type type, int offset, int size, int decimal, bool flag, bool nest) :
	Db_Code ()
{
	name = buffer = NULL;
	resized = false;

	Set_Field (_name, type, offset, size, decimal, flag, nest);
}

Db_Field::Db_Field (void) :	Db_Code ()
{
	name = buffer = NULL;
	offset = size = decimal = 0;
	type = INTEGER;
	nested = resized = false;
}

//-----------------------------------------------------------
//	Db_Field destructor
//-----------------------------------------------------------

Db_Field::~Db_Field (void) 
{
	Clear ();
}

//-----------------------------------------------------------
//	Set_Field
//-----------------------------------------------------------

bool Db_Field::Set_Field (char *name, Field_Type type, int offset, int size, int decimal, bool flag, bool nest) 
{
	if (!Name (name)) return (false);
	Type (type);
	Offset (offset);
	Size (size);
	Decimal (decimal);
	Nested (nest);
	if (!Buffer (flag)) return (false);
	return (true);
}

//-----------------------------------------------------------
//	Name
//-----------------------------------------------------------

bool Db_Field::Name (char *_name) 
{
	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
	if (_name != NULL) {
		int len = (int) strlen (_name) + 1;
		name = new char [len];
		if (name == NULL) return (false);
		str_cpy (name, len, _name);
	}
	return (true);
}

//-----------------------------------------------------------
//	Buffer
//-----------------------------------------------------------

bool Db_Field::Buffer (bool flag) 
{
	if (flag) {
		buffer = (char *) realloc (buffer, size + 1);
		if (buffer == NULL) return (false);
		memset (buffer, '\0', size + 1);

		if (type == STRING || type == CHAR) {
			buffer [0] = ' ';
		} else {
			buffer [0] = '0';
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Buffer
//-----------------------------------------------------------

bool Db_Field::Buffer (char *value) 
{
	int len = (int) strlen (value);
	if (buffer == NULL || len > size) {
		if (len > size) resized = true;
		size = MAX (len, size);
		if (!Buffer (true)) return (false);
	}
	str_cpy (buffer, size + 1, value);
	return (true);
}

//-----------------------------------------------------------
//	Clear
//-----------------------------------------------------------

void Db_Field::Clear (void) 
{
	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
	if (buffer != NULL) {
		free (buffer);
		buffer = NULL;
	}
}
