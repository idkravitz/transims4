//********************************************************* 
//	Db_Base.cpp - database record
//*********************************************************

#include "Db_Base.hpp"

#include "Utility.hpp"
#include "Date_Time.hpp"

#include <stdio.h>
#include <string.h>
#include <math.h>

char Db_Base::buffer [] = " ";
char Db_Base::svalue [] = " ";

//-----------------------------------------------------------
//	Db_Base constructor
//-----------------------------------------------------------

Db_Base::Db_Base (Access_Type access, Format_Type format) : 
	Db_File (access, format)
{
	Dbase_Format (format);

	if (!field.Initialize (sizeof (Db_Field), 0)) Status (FIELD_LIST);
}

//-----------------------------------------------------------
//	Db_Base constructor
//-----------------------------------------------------------

Db_Base::Db_Base (char *filename, Access_Type access, Format_Type format) : 
	Db_File (access, format)
{
	Dbase_Format (format);

	if (!field.Initialize (sizeof (Db_Field), 0)) Status (FIELD_LIST);

	Open (filename);
}

//-----------------------------------------------------------
//	Db_Base destructor
//-----------------------------------------------------------

Db_Base::~Db_Base (void)
{
	Clear_Fields ();
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Db_Base::Initialize (Access_Type access, Format_Type format) 
{
	Dbase_Format (format);

	Db_File::Initialize (access, format);
}

//---------------------------------------------------------
//	Dbase_Format
//---------------------------------------------------------

void Db_Base::Dbase_Format (Format_Type type) 
{
	if (type == DEFAULT_FORMAT) {
		type = (Format_Type) Default_Format ();
	}
	format = type;
	File_Format (type);
}

//-----------------------------------------------------------
//	Add_Field
//-----------------------------------------------------------

bool Db_Base::Add_Field (char *name, Field_Type type, int size, int decimal, int offset, bool binary, bool nest) 
{
	if (field.Max_Records () == 0) {
		field.Max_Records (10);
	}
	int length, num;
	bool buffer_flag;
	Db_Field *fld;

	switch (Dbase_Format ()) {
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
		case VERSION3:
			num = 1;
			length = 0;
			buffer_flag = true;

			for (fld = (Db_Field *) field.First (); fld != NULL; fld = (Db_Field *) field.Next ()) {
				if (fld->Nested () == nest) {
					length += fld->Size () + 1;
					num++;
				}
			}
			if (binary) {
				Binary_Text_Size (type, &size, &decimal);
			}
			length += size;
			if (offset <= 0) offset = num;
			break;
		case UNFORMATED:
		case BINARY:
		case FIXED_COLUMN:
		case DBASE:
		case SQLITE3:
		default:
			buffer_flag = false;

			if (offset < 0) {
				offset = (Dbase_Format () == DBASE) ? 1 : 0;

				for (fld = (Db_Field *) field.First (); fld != NULL; fld = (Db_Field *) field.Next ()) {
					if (fld->Nested () == nest) {
						length = fld->Offset () + fld->Size ();
						if (offset < length) offset = length;
					}
				}
			}
			if (binary) {
				if (Record_Format () != BINARY) {
					Binary_Text_Size (type, &size, &decimal);
				}
			} else if (Record_Format () == BINARY) {
				Text_Binary_Size (type, &size);
			}
			length = offset + size;
			break;
	}
	fld = (Db_Field *) field.New_Record (true);

	if (fld == NULL) return (Status (FIELD_LIST));

	if (!fld->Set_Field (name, type, offset, size, decimal, buffer_flag, nest)) {
		return (Status (MEMORY));
	}
	if (!field.Add ()) return (Status (FIELD_LIST));

	Nest (nest);

	return (Record_Size (length));
}

//---------------------------------------------------------
//	Required_Field
//---------------------------------------------------------

int Db_Base::Required_Field (char *name1, char *name2, char *name3, char *name4, char *name5)
{
	int num = Optional_Field (name1, name2, name3, name4, name5);

	if (!num && Send_Messages ()) {
		if (name5) {
			exe->Error ("%s does not include field:\n\t\t\"%s\", \"%s\", \"%s\", \"%s\" or \"%s\"", 
						File_Type (), name1, name2, name3, name4, name5);
		} else if (name4) {
			exe->Error ("%s does not include field:\n\t\t\"%s\", \"%s\", \"%s\" or \"%s\"", 
						File_Type (), name1, name2, name3, name4);
		} else if (name3) {
			exe->Error ("%s does not include field:\n\t\t\"%s\", \"%s\" or \"%s\"", 
						File_Type (), name1, name2, name3);
		} else if (name2) {
			exe->Error ("%s does not include \"%s\" or \"%s\" field", File_Type (), name1, name2);
		} else if (name1) {
			exe->Error ("%s does not include \"%s\" field", File_Type (), name1);
		} else {
			exe->Error ("%s Required Field Name is NULL");
		}
	}
	return (num);
}

//---------------------------------------------------------
//	Optional_Field
//---------------------------------------------------------

int Db_Base::Optional_Field (char *name1, char *name2, char *name3, char *name4, char *name5)
{
	if (!name1) {
		Status (NULL_NAME);
		return (0);
	}
	int num = Field_Number (name1);

	if (!num && name2) {
		num = Field_Number (name2);

		if (!num && name3) {
			num = Field_Number (name3);

			if (!num && name4) {
				num = Field_Number (name4);

				if (!num && name5) {
					num = Field_Number (name5);
				}
			}
		}
	}
	return (num);
}

//-----------------------------------------------------------
//	Field - by name
//-----------------------------------------------------------

Db_Field * Db_Base::Field (char *name) 
{
	if (name) {
		for (void *fld = field.First (); fld != NULL; fld = field.Next ()) {
			if (str_cmp (((Db_Field *) fld)->Name (), name) == 0) {
				return ((Db_Field *) fld);
			}
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Field_Number
//---------------------------------------------------------

int Db_Base::Field_Number (char *name)
{
	if (name) {
		for (void *fld = field.First (); fld != NULL; fld = field.Next ()) {
			if (str_cmp (((Db_Field *) fld)->Name (), name) == 0) {
				return (field.Record_Index ());
			}
		}
	}
	return (0);
}

//----------------------------------------------------------
//	Read_Field
//----------------------------------------------------------

bool Db_Base::Read_Field (Db_Field *fld, void *data, Field_Type type)
{
	if (data == NULL) return (Status (NULL_POINTER));

	//---- initialize the data field ----

	switch (type) {
		case INTEGER:
			*((int *) data) = 0;
			break;
		case DOUBLE:
			*((double *) data) = 0.0;
			break;
		case STRING:
			*((char *) data) = '\0';
			break;
		case DATE_TIME:
			*((time_t *) data) = 0;
			break;
		default:
			return (Status (ERROR));
	}
	if (fld == NULL) return (Status (NO_FIELD, false));

	Nest (fld->Nested ());

	char *field = Record ();

	if (field == NULL) return (Status (RECORD_SIZE));

	Field_Type fld_type;
	bool asc_flag, justify;
	int len, lvalue = 0;
	double dvalue = 0.0;
	time_t tvalue = 0;
	
	int size = fld->Size ();
	int offset = fld->Offset ();

	switch (Record_Format ()) {
		case BINARY:
			field += offset;
			asc_flag = false;
			justify = (fld->Type () == STRING);
			break;
		case FIXED_COLUMN:
			field += offset;
			asc_flag = justify = true;
			break;
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
			field = fld->Buffer ();
			if (field != NULL) {
				size = (int) strlen (field);
				asc_flag = true;
				justify = false;
				break;
			}
		default:
			field = Get_Field_Number (offset, buffer, sizeof (buffer));
			size = (int) strlen (field);
			asc_flag = true;
			justify = false;
			break;
	}

	if (justify) {
		while (*field == ' ' && size > 0) {
			field++;
			size--;
		}
		if (size >= (int) sizeof (svalue)) size = (int) sizeof (svalue) - 1;

		str_cpy (svalue, size+1, field);
		field = svalue + strlen (svalue) - 1;

		while (field > svalue && 
			(*field == ' ' || *field == '\t' || *field == '\n' || *field == '\r')) {
			*field-- = '\0';
		}
	} else if (asc_flag) {
		str_cpy (svalue, sizeof (svalue), field);
	}

	//---- read the field from the current record ----

	switch (fld->Type ()) {
		default:
			return (Status (ERROR));

		case INTEGER:
			if (asc_flag) {
				lvalue = atol (svalue);
			} else if (size == sizeof (int)) {
				lvalue = *((int *) field);
			} else if (size == sizeof (short)) {
				lvalue = *((short *) field);
			} else if (size == sizeof (char)) {
				lvalue = *((char *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = INTEGER;
			break;
			
		case UNSIGNED:
			if (asc_flag) {
				lvalue = atol (svalue);
			} else if (size == sizeof (int)) {
				lvalue = *((unsigned int *) field);
			} else if (size == sizeof (short)) {
				lvalue = *((unsigned short *) field);
			} else if (size == sizeof (char)) {
				lvalue = *((unsigned char *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = INTEGER;
			break;
			
		case DOUBLE:
			if (asc_flag) {
				dvalue = atof (svalue);
			} else if (size == sizeof (double)) {
				dvalue = *((double *) field);
			} else if (size == sizeof (float)) {
				dvalue = *((float *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DOUBLE;
			break;
			
		case FIXED:
			if (asc_flag) {
				dvalue = atof (svalue);
				if (strchr (svalue, '.') == NULL) {
					dvalue = dvalue / pow (10.0, fld->Decimal ());
				}
			} else {
				if (size == sizeof (int)) {
					lvalue = *((int *) field);
				} else if (size == sizeof (short)) {
					lvalue = *((short *) field);
				} else if (size == sizeof (char)) {
					lvalue = *((char *) field);
				} else {
					return (Status (FIELD_BYTES));
				}
				dvalue = (double) lvalue / pow (10.0, fld->Decimal ());
			}
			fld_type = DOUBLE;
			break;
			
		case STRING:
			fld_type = STRING;
			break;

		case CHAR:
			if (asc_flag) {
				if (svalue [0] == '\0') {
					svalue [0] = ' ';
					svalue [1] = '\0';
				}
			} else {
				svalue [0] = *((char *) field);
				if (svalue [0] == '\0') {
					svalue [0] = ' ';
				}
				svalue [1] = '\0';
			}
			fld_type = STRING;
			break;
			
		case DATE:
		case TIME:
		case DATE_TIME:
			if (asc_flag) {
				tvalue = (time_t) Convert_Time_String (svalue);
			} else if (size == sizeof (time_t)) {
				tvalue = *((time_t *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DATE_TIME;
			break;

		case DAY_TIME:
			if (asc_flag) {	
				dvalue = Convert_Day_Time_String (svalue);
			} else if (size == sizeof (time_t)) {
				dvalue = (double) *((int *) field) / pow (10.0, fld->Decimal ());
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DOUBLE;
			break;
	}

	//---- convert to the output data type ----

	switch (type) {
		case INTEGER:
			switch (fld_type) {
				case INTEGER:
					*((int *) data) = lvalue;
					break;
				case DOUBLE:
					if (dvalue < 0.0) {
						*((int *) data) = (int) (dvalue - 0.5);
					} else {
						*((int *) data) = (int) (dvalue + 0.5);
					}
					break;
				case STRING:
					if (*svalue < '0' || *svalue > '9' || strchr (svalue, ':') != NULL || strchr (svalue, '/') != NULL) {
						*((int *) data) = (int) Convert_Time_String (svalue);
					} else {
						*((int *) data) = atol (svalue);
					}
					break;
				case DATE_TIME:
					*((int *) data) = (int) tvalue;
					break;
				default:
					return (Status (ERROR));
			}
			break;
		case DOUBLE:
			switch (fld_type) {
				case INTEGER:
					*((double *) data) = lvalue;
					break;
				case DOUBLE:
					*((double *) data) = dvalue;
					break;
				case STRING:
					if ((*svalue != '.' && (*svalue < '0' || *svalue > '9')) || strchr (svalue, ':') != NULL) {
						*((double *) data) = Convert_Day_Time_String (svalue);
					} else {
						*((double *) data) = atof (svalue);
					}
					break;
				case DATE_TIME:
					*((double *) data) = (double) tvalue;
					break;
				default:
					return (Status (ERROR));
			}
			break;
		case STRING:
			switch (fld->Type ()) {
				case DAY_TIME:
					str_cpy (svalue, sizeof (svalue), Day_Time_String (dvalue));
					break;
				case DATE:
					str_cpy (svalue, sizeof (svalue), Date_String (tvalue));
					break;
				case TIME:
					str_cpy (svalue, sizeof (svalue), Time_String (tvalue));
					break;
				case DATE_TIME:
					str_cpy (svalue, sizeof (svalue), Date_Time_String (tvalue));
					break;
				default:
					if (fld_type == INTEGER) {
						str_fmt (svalue, sizeof (svalue), "%d", lvalue);
					} else if (fld_type == DOUBLE) {
						str_fmt (svalue, sizeof (svalue), "%.*lf", fld->Decimal (), dvalue);
					}
					break;
			}
			len = (int) strlen (svalue);
			size = fld->Size () + 1;

			if (size > (int) sizeof (svalue)) {
				size = (int) sizeof (svalue);
			}
			if (len >= size) {
				len = size - 1;
			}
			//if (!justify) {
			//	size = MIN (fld->Size ()+1, (int) strlen (svalue)+1);
			//}
			str_cpy ((char *) data, size, svalue, len);
			break;
		case DATE_TIME:
			switch (fld_type) {
				case INTEGER:
					*((time_t *) data) = (time_t) lvalue;
					break;
				case DOUBLE:
					*((time_t *) data) = (time_t) (dvalue + 0.5);
					break;
				case STRING:
					*((time_t *) data) = Convert_Time_String (svalue);
					break;
				case DATE_TIME:
					*((time_t *) data) = tvalue;
					break;
				default:
					return (Status (ERROR));
			}
			break;
		default:
			return (Status (ERROR));
	}
	return (true);
}

//----------------------------------------------------------
//	Write_Field
//----------------------------------------------------------

bool Db_Base::Write_Field (Db_Field *fld, void *data, Field_Type type)
{
	if (data == NULL) return (Status (NULL_POINTER));
	if (fld == NULL) return (Status (NO_FIELD, false));
	if (Record () == NULL) return (Status (RECORD_SIZE));

	int lvalue = 0;
	unsigned uvalue = 0;
	double dvalue = 0.0;
	time_t tvalue = 0;

	//---- convert the input data type to generic variables ----

	switch (fld->Type ()) {
		default:
			return (Status (ERROR));
			
		case INTEGER:
			switch (type) {
				case INTEGER:
					lvalue = *((int *) data);
					break;
				case DOUBLE:
					dvalue = *((double *) data);
					if (dvalue > MAX_INTEGER) {
						lvalue = MAX_INTEGER;
					} else if (dvalue < -MAX_INTEGER) {
						lvalue = -MAX_INTEGER;
					} else if (dvalue < 0.0) {
						lvalue = (int) (dvalue - 0.5);
					} else {
						lvalue = (int) (dvalue + 0.5);
					}
					break;
				case STRING:
					str_cpy (svalue, sizeof (svalue), (char *) data);
					lvalue = atol (svalue);
					break;
				case DATE_TIME:
					lvalue = (int) (*((time_t *) data));
					break;
				default:
					return (Status (ERROR));
			}
			break;

		case UNSIGNED:
			switch (type) {
				case INTEGER:
					uvalue = *((unsigned *) data);
					break;
				case DOUBLE:
					dvalue = *((double *) data);
					if (dvalue > 2.0 * MAX_INTEGER) {
						uvalue = MAX_INTEGER;
						uvalue *= 2;
					} else if (dvalue < 0.0) {
						uvalue = 0;
					} else {
						uvalue = (unsigned) (dvalue + 0.5);
					}
					break;
				case STRING:
					str_cpy (svalue, sizeof (svalue), (char *) data);
					uvalue = atol (svalue);
					break;
				case DATE_TIME:
					uvalue = (unsigned) (*((time_t *) data));
					break;
				default:
					return (Status (ERROR));
			}
			break;

		case FIXED:
		case DOUBLE:
		case DAY_TIME:
			switch (type) {
				case INTEGER:
					dvalue = (double) *((int *) data);
					break;
				case DOUBLE:
					dvalue = *((double *) data);
					break;
				case STRING:
					str_cpy (svalue, sizeof (svalue), (char *) data);
					if (fld->Type () == DAY_TIME) {
						dvalue = Convert_Day_Time_String (svalue);
					} else {
						dvalue = atof (svalue);
					}
					break;
				case DATE_TIME:
					dvalue = (double) *((time_t *) data);
					break;
				default:
					return (Status (ERROR));
			}
			break;
		
		case STRING:
		case CHAR:
			switch (type) {
				case INTEGER:
					str_fmt (svalue, sizeof (svalue), "%d", *((int *) data));
					break;
				case DOUBLE:
					str_fmt (svalue, sizeof (svalue), "%.*lf", fld->Decimal (), *((double *) data));
					break;
				case STRING:
					str_cpy (svalue, sizeof (svalue), (char *) data);
					break;
				case DATE_TIME:
					str_fmt (svalue, sizeof (svalue), Date_Time_String (*((time_t *) data)));
					break;
				default:
					return (Status (ERROR));
			}
			break;

		case DATE:
		case TIME:
		case DATE_TIME:
			switch (type) {
				case INTEGER:
					tvalue = (time_t) *((int *) data);
					break;
				case DOUBLE:
					tvalue = (time_t) (*((double *) data) + 0.5);
					break;
				case STRING:
					str_cpy (svalue, sizeof (svalue), (char *) data);
					tvalue = Convert_Time_String (svalue);
					break;
				case DATE_TIME:
					tvalue = *((time_t *) data);
					break;
				default:
					return (Status (ERROR));
			}
			break;
	}

	//---- place the data onto the data record -----

	Nest (fld->Nested ());

	char *field = Record ();

	bool asc_flag = false;
	bool justify = false;
	int len, position;
	int size = fld->Size ();
	int offset = fld->Offset ();

	switch (Record_Format ()) {
		case BINARY:
			field += offset;
			asc_flag = false;
			break;
		case FIXED_COLUMN:
			field += offset;
			memset (field, ' ', size);
			asc_flag = justify = true;
			break;
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
			field = fld->Buffer ();
			if (field != NULL) {
				asc_flag = true;
				justify = false;
				break;
			}
		default:		
			field = buffer;
			asc_flag = true;
			justify = false;
			break;
	}

	switch (fld->Type ()) {
		default:
			return (Status (ERROR));
			
		case INTEGER:
			if (asc_flag) {
				str_fmt (svalue, sizeof (svalue), "%d", lvalue);

				if (justify) {
					len = (int) strlen (svalue);
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue, len);
				} else {
					str_cpy (field, size+1, svalue);
				}
			} else if (size == sizeof (int)) {
				*((int *) field) = lvalue;
			} else if (size == sizeof (short)) {
				*((short *) field) = (short) lvalue;
			} else if (size == sizeof (char)) {
				*((char *) field) = (char) lvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;
			
		case UNSIGNED:
			if (asc_flag) {
				str_fmt (svalue, sizeof (svalue), "%d", uvalue);

				if (justify) {
					len = (int) strlen (svalue);
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue, len);
				} else {
					str_cpy (field, size + 1, svalue);
				}
			} else if (size == sizeof (int)) {
				*((unsigned int *) field) = (unsigned int) uvalue;
			} else if (size == sizeof (short)) {
				*((unsigned short *) field) = (unsigned short) uvalue;
			} else if (size == sizeof (char)) {
				*((unsigned char *) field) = (unsigned char) uvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;

		case DOUBLE:
			if (asc_flag) {
				str_fmt (svalue, sizeof (svalue), "%.*lf", fld->Decimal (), dvalue);
			
				if (justify) {
					len = (int) strlen (svalue);
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue, len);
				} else {
					str_cpy (field, size + 1, svalue);
				}
			} else if (size == sizeof (double)) {
				*((double *) field) = dvalue;
			} else if (size == sizeof (float)) {
				*((float *) field) = (float) dvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;

		case FIXED:
			if (asc_flag) {
				str_fmt (svalue, sizeof (svalue), "%.*lf", fld->Decimal (), dvalue);

				if (justify) {
					len = (int) strlen (svalue);
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue, len);
				} else {
					str_cpy (field, size + 1, svalue);
				}
			} else {
				if (dvalue < 0.0) {
					lvalue = (int) (dvalue * pow (10.0, fld->Decimal ()) - 0.5);
				} else {
					lvalue = (int) (dvalue * pow (10.0, fld->Decimal ()) + 0.5);
				}

				if (size == sizeof (int)) {
					*((int *) field) = lvalue;
				} else if (size == sizeof (short)) {
					*((short *) field) = (short) lvalue;
				} else if (size == sizeof (char)) {
					*((char *) field) = (char) lvalue;
				} else {
					return (Status (FIELD_BYTES));
				}
			}
			break;

		case STRING:
			if (asc_flag) {
				if (justify) {
					len = (int) strlen (svalue);
					if (len > size) len = size;
					memcpy (field, svalue, len);
				} else {
					str_cpy (field, size + 1, svalue);
				}
			} else {
				memset (field, '\0', size);
				memcpy (field, svalue, MIN ((int) strlen (svalue), size));
			}
			break;
			
		case CHAR:
			if (!asc_flag) {
				memset (field, '\0', size);
			}
			field [0] = svalue [0];

			if (asc_flag) {
				if (field [0] == '\0') field [0] = ' ';
				if (!justify) field [1] = '\0';
			}
			break;
			
		case DATE:
			if (asc_flag) {
				str_cpy (svalue, sizeof (svalue), Date_String (tvalue));
				if (justify) {
					len = (int) strlen (svalue);
					if (len > size) len = size;
					memcpy (field, svalue, len);
				} else {
					str_cpy (field, size + 1, svalue);
				}
			} else if (size == sizeof (time_t)) {
				*((time_t *) field) = tvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;
			
		case TIME:
			if (asc_flag) {
				str_cpy (svalue, sizeof (svalue), Time_String (tvalue));
				if (justify) {
					len = (int) strlen (svalue);
					if (len > size) len = size;
					memcpy (field, svalue, len);
				} else {
					str_cpy (field, size + 1, svalue);
				}
			} else if (size == sizeof (time_t)) {
				*((time_t *) field) = tvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;
			
		case DATE_TIME:
			if (asc_flag) {
				str_cpy (svalue, sizeof (svalue), Date_Time_String (tvalue));
				if (justify) {
					len = (int) strlen (svalue);
					if (len > size) len = size;
					memcpy (field, svalue, len);
				} else {
					str_cpy (field, size + 1, svalue);
				}
			} else if (size == sizeof (time_t)) {
				*((time_t *) field) = tvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;
			
		case DAY_TIME:
			if (asc_flag) {
				str_cpy (svalue, sizeof (svalue), Day_Time_String (dvalue));
				if (justify) {
					len = (int) strlen (svalue);
					if (len > size) len = size;
					memcpy (field, svalue, len);
				} else {
					str_cpy (field, size + 1, svalue);
				}
			} else {
				lvalue = (int) (dvalue * pow (10.0, fld->Decimal ()) + 0.5);

				if (size == sizeof (int)) {
					*((int *) field) = lvalue;
				} else if (size == sizeof (short)) {
					*((short *) field) = (short) lvalue;
				} else if (size == sizeof (char)) {
					*((char *) field) = (char) lvalue;
				} else {
					return (Status (FIELD_BYTES));
				}
			}
			break;
	}

	if (asc_flag && !justify && fld->Buffer () == NULL) {
		return (Set_Field_Number (offset, field));
	}
	return (true);
}

//---------------------------------------------------------
//	Copy_Fields
//---------------------------------------------------------

bool Db_Base::Copy_Fields (Db_Base *input)
{
	Db_Field *in_fld, *out_fld;

	int lvalue;
	double dvalue;
	char svalue [FIELD_BUFFER];
	time_t tvalue;

	int num = input->Num_Fields ();

	for (int i=1; i <= num; i++) {
		in_fld = input->Field (i);

		if (in_fld != NULL) {
			if (input->Nested ()) {
				if (in_fld->Nested () != input->Nest ()) continue;
			}
			out_fld = Field (in_fld->Name ());

			if (out_fld != NULL) {
				switch (in_fld->Type ()) {
					case INTEGER:
					case UNSIGNED:
						input->Read_Field (in_fld, (void *) &lvalue, INTEGER);
						if (!Write_Field (out_fld, (void *) &lvalue, INTEGER)) return (false);
						break;
					case DOUBLE:
					case FIXED:
					case DAY_TIME:
						input->Read_Field (in_fld, (void *) &dvalue, DOUBLE);
						if (!Write_Field (out_fld, (void *) &dvalue, DOUBLE)) return (false);
						break;
					case STRING:
					case CHAR:
						input->Read_Field (in_fld, (void *) svalue, STRING);
						if (!Write_Field (out_fld, (void *) svalue, STRING)) return (false);
						break;
					case DATE:
					case TIME:
					case DATE_TIME:
						input->Read_Field (in_fld, (void *) &tvalue, DATE_TIME);
						if (!Write_Field (out_fld, (void *) &tvalue, DATE_TIME)) return (false);
						break;
				}
			}
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Clear_Fields
//-----------------------------------------------------------

void Db_Base::Clear_Fields (void)
{
	for (void *fld = field.First (); fld != NULL; fld = field.Next ()) {
		((Db_Field *) fld)->Clear ();
	}
	field.Clear ();
}

//---------------------------------------------------------
//	Replicate_Fields
//---------------------------------------------------------

bool Db_Base::Replicate_Fields (Db_Base *file, bool notes_flag) 
{
	int i, offset, nfield;
	bool convert, binary;
	Db_Field *fld;

    convert = (file->Dbase_Format () != Dbase_Format () || Num_Fields () > 0);
	binary = (file->Record_Format () == BINARY);

	offset = END_OF_RECORD;
	nfield = file->Num_Fields ();

	for (i=1; i <= nfield; i++) {
		fld = file->Field (i);
		
		if (fld == NULL) continue;

		if (!convert) {
			offset = fld->Offset ();
		}
		if (notes_flag == false) {
			if (!str_cmp (fld->Name (), "NOTES")) continue;
		}
		Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), offset, binary, fld->Nested ());
	}
	return (Write_Fields ());
}

//-----------------------------------------------------------
//	Input_Record
//-----------------------------------------------------------

bool Db_Base::Input_Record (char *record, int size)
{
	Db_Field *fld;
	char *ptr = Record ();

	if (Dbase_Format () == SQLITE3) {
		int i, j, num, lvalue, result;
		const unsigned char *text;
		double dvalue;

		//---- get the next record ----

		if (Nest ()) {
			if (new_nest_flag) {
				new_nest_flag = false;

				sqlite3_reset (read_nest);
				sqlite3_bind_int (read_nest, 1, parent_id);
			}
			result = sqlite3_step (read_nest);
		} else {
			Record_Number (0);
			parent_id = Record_Number ();
			new_nest_flag = true;

			sqlite3_reset (read_stmt);
			sqlite3_bind_int (read_stmt, 1, parent_id);
			result = sqlite3_step (read_stmt);
		}
		if (result == SQLITE_DONE) return (false);
		if (result != SQLITE_ROW) {
			exe->Warning ("SQLITE3 Read Problem: %s", sqlite3_errmsg (db_file));
		}
		num = Num_Fields ();

		for (i=1, j=0; i <= num; i++) {
			fld = Field (i);
			if (Nest () != fld->Nested ()) continue;
			if (Nest ()) {
				if (fld->Type () == INTEGER) {
					lvalue = sqlite3_column_int (read_nest, j+1);
					Put_Field (i, lvalue);
				} else if (fld->Type () == DOUBLE) {
					dvalue = sqlite3_column_double (read_nest, j+1);
					Put_Field (i, dvalue);
				} else {
					text = sqlite3_column_text (read_nest, j+1);
					Put_Field (i, (char *) text);
				}
			} else {
				if (fld->Type () == INTEGER) {
					lvalue = sqlite3_column_int (read_stmt, j);
					Put_Field (i, lvalue);
				} else if (fld->Type () == DOUBLE) {
					dvalue = sqlite3_column_double (read_stmt, j);
					Put_Field (i, dvalue);
				} else {
					text = sqlite3_column_text (read_stmt, j);
					Put_Field (i, (char *) text);
				}
			}
			j++;
		}
	} else {
		for (fld = (Db_Field *) field.First (); fld != NULL; fld = (Db_Field *) field.Next ()) {
			if (fld->Buffer () == NULL) break;
			if (Nest () == fld->Nested ()) {
				ptr = Parse_String (ptr, buffer, sizeof (buffer));
				if (!fld->Buffer (buffer)) return (false);
			}
		}
	}
	return (Record (record, size));
}

//-----------------------------------------------------------
//	Output_Record
//-----------------------------------------------------------

char * Db_Base::Output_Record (char *record)
{
	if (record != Record ()) return (record);

	Db_Field *fld;

	if (Dbase_Format () == SQLITE3) {
		int i, j, num, lvalue;
		char buffer [STRING_BUFFER];
		double dvalue;

		num = Num_Fields ();

		for (i=j=1; i <= num; i++) {
			fld = Field (i);
			if (Nest () != fld->Nested ()) continue;

			if (Nest ()) {
				if (fld->Type () == INTEGER) {
					Get_Field (i, &lvalue);
					sqlite3_bind_int (insert_nest, j+1, lvalue);
				} else if (fld->Type () == DOUBLE) {
					Get_Field (i, &dvalue);
					sqlite3_bind_double (insert_nest, j+1, dvalue);
				} else {
					Get_Field (i, buffer);
					sqlite3_bind_text (insert_nest, j+1, buffer, -1, SQLITE_TRANSIENT);
				}
			} else {
				if (fld->Type () == INTEGER) {
					Get_Field (i, &lvalue);
					sqlite3_bind_int (insert_stmt, j, lvalue);
				} else if (fld->Type () == DOUBLE) {
					Get_Field (i, &dvalue);
					sqlite3_bind_double (insert_stmt, j, dvalue);
				} else {
					Get_Field (i, buffer);
					sqlite3_bind_text (insert_stmt, j, buffer, -1, SQLITE_TRANSIENT);
				}
			}
			j++;
			
		}
		if (Nest ()) {
			sqlite3_bind_int (insert_nest, 1, parent_id);

			if (sqlite3_step (insert_nest) != SQLITE_DONE) {
				exe->Warning ("Inserting Database: %s", sqlite3_errmsg (db_file));
			}
			if (sqlite3_reset (insert_nest) != SQLITE_OK) {
				exe->Warning ("Inserting Database: %s", sqlite3_errmsg (db_file));
			}
		} else {
			if (sqlite3_step (insert_stmt) != SQLITE_DONE) {
				exe->Warning ("Inserting Database: %s", sqlite3_errmsg (db_file));
			}
			parent_id = (int) sqlite3_last_insert_rowid (db_file);
			if (sqlite3_reset (insert_stmt) != SQLITE_OK) {
				exe->Warning ("Inserting Database: %s", sqlite3_errmsg (db_file));
			}
		}
	} else {
		char *ptr, *delimiter;
		bool first = true;

		delimiter = Delimiters ();
		record = Record ();

		for (fld = (Db_Field *) field.First (); fld != NULL; fld = (Db_Field *) field.Next ()) {
			if (Nest () == fld->Nested ()) {
				ptr = fld->Buffer ();
				if (ptr == NULL) break;

				if (first) {
					first = false;
				} else {
					*record++ = *delimiter;
				}
				while (*ptr != '\0') {
					*record++ = *ptr++;
				}
				*record = '\0';
			}
		}
	}
	return (Record ());
}

//-----------------------------------------------------------
//	Reset_Record
//-----------------------------------------------------------

char * Db_Base::Reset_Record (void)
{
	if (File_Format () == CSV_DELIMITED) {
		char *ptr;
		Db_Field *fld;

		for (fld = (Db_Field *) field.First (); fld; fld = (Db_Field *) field.Next ()) {
			ptr = fld->Buffer ();
			if (ptr != NULL && Nest () == fld->Nested ()) {
				*ptr = '\0';
			}
		}
	}
	return (Db_Record::Reset_Record ());
}

//---------------------------------------------------------
//	Binary_Text_Size
//---------------------------------------------------------

void Db_Base::Binary_Text_Size (Field_Type type, int *size, int *decimal) 
{
	switch (type) {
		case INTEGER:
		case UNSIGNED:
			if (*size == sizeof (char)) {
				*size = 3;
			} else if (*size == sizeof (short)) {
				*size = 5;
			} else {
				*size = 10;
			}
			break;
		case DOUBLE:
			if (*decimal == 0) {
				if (*size == sizeof (float)) {
					*size = 8;
					*decimal = 2;
				} else {
					*size = 12;
					*decimal = 4;
				}
			} else {
				if (*size == sizeof (float)) {
					*size = *decimal + 6;
				} else {
					*size = *decimal + 10;
				}
			}
			break;
		case FIXED:
			if (*decimal == 0) {
				if (*size == sizeof (char)) {
					*size = 4;
					*decimal = 2;
				} else if (*size == sizeof (short)) {
					*size = 6;
					*decimal = 2;
				} else {
					*size = 8;
					*decimal = 2;
				}
			} else {
				if (*size == sizeof (char)) {
					*size = 4;
				} else if (*size == sizeof (short)) {
					*size = 6;
				} else {
					*size = 10;
				}
			}
			break;
		case STRING:
		case CHAR:
			break;
		case DATE:
			*size = 10;
			break;
		case TIME:
			*size = 8;
			break;
		case DATE_TIME:
			*size = 23;
			break;
		case DAY_TIME:
			*size = 14;
			break;
	}
}

//---------------------------------------------------------
//	Text_Binary_Size
//---------------------------------------------------------

void Db_Base::Text_Binary_Size (Field_Type type, int *size) 
{
	switch (type) {
		case INTEGER:
		case UNSIGNED:
			if (*size < 3) {
				*size = sizeof (char);
			} else if (*size < 5) {
				*size = sizeof (short);
			} else {
				*size = sizeof (int);
			}
			break;
		case DOUBLE:
			if (*size < 9) {
				*size = sizeof (float);
			} else {
				*size = sizeof (double);
			}
			break;
		case FIXED:
			if (*size < 4) {
				*size = sizeof (char);
			} else if (*size < 6) {
				*size = sizeof (short);
			} else {
				*size = sizeof (int);
			}
			break;
		case STRING:
		case CHAR:
			break;
		case DATE:
		case TIME: 
		case DATE_TIME:
		case DAY_TIME:
			*size = sizeof (time_t);
			break;
	}
}
