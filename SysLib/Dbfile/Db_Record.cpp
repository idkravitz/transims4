//********************************************************* 
//	Db_Record.cpp - database record
//*********************************************************

#include "Db_Record.hpp"

#include "Utility.hpp"

#include <stdio.h>
#include <string.h>

char * Db_Record::comma_delimiter = ",";
char * Db_Record::space_delimiter = " \t";
char * Db_Record::tab_delimiter = "\t";
char * Db_Record::csv_delimiter = ", \t";

//-----------------------------------------------------------
//	Db_Record constructor
//-----------------------------------------------------------

Db_Record::Db_Record (Format_Type format) : Db_Status ()
{
	record = nest_rec = delimiters = NULL;
	record_size = max_size = 0;
	nest_rec_size = nest_max_size = 0;
	nested = nest = false;
	
	Record_Format (format);
}

//-----------------------------------------------------------
//	Db_Record destructor
//-----------------------------------------------------------

Db_Record::~Db_Record (void)
{
	Record_Size (0);
	if (Nested ()) {
		Nest (!Nest ());
		Record_Size (0);
	}
}

//-----------------------------------------------------------
//	Record_Format
//-----------------------------------------------------------

void Db_Record::Record_Format (Format_Type format)
{
	if (format == DEFAULT_FORMAT) {
		format = (Format_Type) Default_Format ();
	}
	record_format = format;

	switch (record_format) {
		case DBASE:
			record_format = FIXED_COLUMN;
			break;
		case COMMA_DELIMITED:
			delimiters = comma_delimiter;
			break;
		case SPACE_DELIMITED:
			delimiters = space_delimiter;
			break;
		case VERSION3:
			record_format = TAB_DELIMITED;
		case TAB_DELIMITED:
			delimiters = tab_delimiter;
			break;
		case SQLITE3:
			record_format = BINARY;
			break;
		default:	//---- CSV ----	
			delimiters = csv_delimiter;
			break;
	}
	if (record_format == BINARY || record_format == FIXED_COLUMN) {
		Record_Size (0);
		if (Nested ()) {
			Nest (!Nest ());
			Record_Size (0);
		}
	} else {
		Record_Size (4080);
		if (Nested ()) {
			Nest (!Nest ());
			Record_Size (4080);
		}
	}
}

//-----------------------------------------------------------
//	Record
//-----------------------------------------------------------

bool Db_Record::Record (void *_record, int size)
{
	bool flag = (_record != Record ());

	if (size > 0) {
		if (!Record_Size (size)) return (false);
	} else {
		size = Max_Size ();
	}
	if (record_format == BINARY) {
		if (flag) {
			memcpy (Record (), _record, size);
		}
	} else {
		if (flag) {
			str_cpy (((char *) Record ()), size + 1, ((char *) _record));
		}
		size = (int) strlen (Record ());
	}
	if (nest) {
		nest_rec_size = size;
	} else {
		record_size = size;
	}
	return (true);
}

//-----------------------------------------------------------
//	Record_Size
//-----------------------------------------------------------

bool Db_Record::Record_Size (int size)
{
	if (size > Max_Size ()) {
		int rec_size = size + 2;

		if (nest) {
			nest_max_size = 0;
			nest_rec = (char *) realloc (nest_rec, rec_size);

			if (nest_rec == NULL) return (Status (MEMORY));
			memset (nest_rec, '\0', rec_size);
			nest_max_size = size;
		} else {
			max_size = 0;
			record = (char *) realloc (record, rec_size);

			if (record == NULL) return (Status (MEMORY));
			memset (record, '\0', rec_size);
			max_size = size;
		}

	} else if (size == 0) {
		if (nest) {
			if (nest_rec != NULL) {
				free (nest_rec);
				nest_rec = NULL;
			}
			nest_max_size = nest_rec_size = 0;
		} else {
			if (record != NULL) {
				free (record);		// was deallocated using "delete", fixed 5/2/2008
				record = NULL;
			}
			max_size = record_size = 0;
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Reset_Record
//-----------------------------------------------------------

char * Db_Record::Reset_Record (void)
{
	if (nest) {
		nest_rec_size = nest_max_size;

		if (nest_rec == NULL)  {
			Status (RECORD_SIZE);
		} else {
			if (nest_rec_size > 0) {
				memset (nest_rec, ((record_format == FIXED_COLUMN) ? ' ' : '\0'), nest_rec_size);
			}
			nest_rec [nest_rec_size] = '\0';
		}
		return (nest_rec);
	} else {
		record_size = max_size;

		if (record == NULL)  {
			Status (RECORD_SIZE);
		} else {
			if (record_size > 0) {
				memset (record, ((record_format == FIXED_COLUMN) ? ' ' : '\0'), record_size);
			}
			record [record_size] = '\0';
		}
		return (record);
	}
}

//-----------------------------------------------------------
//	Clean_Record
//-----------------------------------------------------------

char * Db_Record::Clean_Record (char *string)
{
	char *ptr;
	bool quote, tick;

	if (string == NULL) {
		string = Record ();
	}

	//---- remove comment statements ----

	ptr = string;
	quote = tick = false;

	while (*ptr != '\0') {
		if (!tick && *ptr == '"') {
			quote = !quote;
		} else if (!quote && *ptr == '\'') {
			tick = !tick;
		} else if (!quote && !tick && 
			((*ptr == '#' && (*(ptr+1) == '#' || *(ptr+1) == '-' || *(ptr+1) == '*')) ||
			(*ptr == '/' && (*(ptr+1) == '/' || *(ptr+1) == '-' || *(ptr+1) == '*')) ||
			(*ptr == ';' && (*(ptr+1) == ';' || *(ptr+1) == '-' || *(ptr+1) == '*')))) {
			*ptr = '\0';
			break;
		}
		ptr++;
	}

	//---- remove white space ----

	while (--ptr >= string) {
        if (*ptr != ' ' && *ptr != '\t' && *ptr != '\n' && *ptr != '\r' && *ptr != '\f') break;
		*ptr = '\0';
	}
	return (string);
}

//----------------------------------------------------------
//      Parse_String
//----------------------------------------------------------

char * Db_Record::Parse_String (char *string, char field [], int field_size)
{
	field [0] = '\0';
	if (string == NULL) return (NULL);

	while (*string == ' ') string++;

	if (*string == '\0' || *string == '\n' || *string == '\r')	return (NULL);

	bool quote, tick;
	char *ptr, *delimiter;

	ptr = field;
	field_size--;

	quote = (*string == '"');
	tick = (*string == '\'');
	if (quote || tick) string++;
	
	while (*string != '\0' && *string != '\n' && *string != '\r' && *string != '\f') {
		if (quote && *string == '"') {
			quote = false;
		} else if (tick && *string == '\'') {
			tick = false;
		} else if (quote || tick) {
			if (ptr - field < field_size) *ptr++ = *string;
		} else {
			delimiter = delimiters;

			while (*delimiter != '\0') {
				if (*delimiter == *string) {
					string++;
					while (*string == ' ') {
						string++;
					}
					break;
				}
				delimiter++;
			}
			if (*delimiter == '\0') {
				if (ptr - field < field_size) *ptr++ = *string;
			} else {
				break;
			}
		}
		string++;
	}
	*ptr = '\0';

	return (string);	
}

//----------------------------------------------------------
//	Get_Field_Number
//----------------------------------------------------------

char * Db_Record::Get_Field_Number (int number, char field [], int field_size)
{
	char *ptr = Record ();

	if (ptr == NULL) {
		Status (RECORD_SIZE);
		return (NULL);
	}
	for (int i=0; i < number && ptr != NULL; i++) {
		ptr = Parse_String (ptr, field, field_size);
	}
	return (field);
}

//----------------------------------------------------------
//	Set_Field_Number
//----------------------------------------------------------

bool Db_Record::Set_Field_Number (int number, char *string)
{
	char *rec = Record ();

	if (rec == NULL) return (Status (RECORD_SIZE));

	int len, size, shift;
	char *ptr, *field, buffer [FIELD_BUFFER];

	//---- remove trailing blanks ----

	len = (int) strlen (string);
	ptr = string + len - 1;
	
	while (ptr > string) {
		if (*ptr != ' ') break;
		len--;
		*ptr-- = '\0';
	}

	//---- set the record pointer ----

	size = 0;
	field = ptr = rec;

	//---- locate the field number ----

	for (int i=0; i < number; i++) {
		ptr = Parse_String (ptr, buffer, sizeof (buffer));

		//---- if the field was not found add a dummy field ----

		if (ptr == NULL) {
			size = (int) strlen (rec);
			if (size + 3 >= max_size) return (Status (RECORD_SIZE));

			ptr = rec + size;

			if (*delimiters == ' ') {
				*ptr++ = '\"';
				*ptr++ = '\"';
			}
			*ptr++ = *delimiters;
			*ptr = '\0';
		}

		//---- set the field pointer ----

		if (i == number - 2) {
			field = ptr;
		}
	}

	//---- check the size of the string after the field ----

	shift = (int) strlen (ptr);
	size = (int) (field - rec);
	if (size + 2 >= max_size) return (Status (RECORD_SIZE));

	if (size + len + shift + 3 >= max_size) {
		shift -= (max_size - size - len - 3);
		if (shift < 0) shift = 0;
	}

	if (strstr (string, delimiters)) {
		if (shift) {
			memmove (field + len + 3, ptr, shift+1);
		}
		*field++ = '"';
		memcpy (field, string, len);
		*(field + len) = '"';
		*(field + len + 1) = (shift) ? *delimiters : '\0';
	} else {
		if (shift) {
			memmove (field + len + 1, ptr, shift+1);
		}
		memcpy (field, string, len);
		*(field + len) = (shift) ? *delimiters : '\0';
	}
	rec [max_size + 1] = '\0';
	return (true);
}
