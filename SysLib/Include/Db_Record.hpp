//*********************************************************
//	Db_Record.hpp - database record
//*********************************************************

#ifndef DB_RECORD_HPP
#define DB_RECORD_HPP

#include "Db_Status.hpp"
#include "System_Defines.hpp"

#include <stdlib.h>

//---------------------------------------------------------
//	Db_Record Class definition
//---------------------------------------------------------

class  Db_Record : public Db_Status
{
public:
	Db_Record (Format_Type format = UNFORMATED);
	virtual ~Db_Record ();

	virtual bool  Record_Size (int size);
	int   Record_Size (void)							{ return ((nest) ? nest_rec_size : record_size); }

	int   Max_Size (void)								{ return ((nest) ? nest_max_size : max_size); }

	void  Record_Format (Format_Type format);
	Format_Type  Record_Format (void)					{ return (record_format); }

	void  Delimiters (char *_delimiters)				{ if (_delimiters != NULL) delimiters = _delimiters; }
	char *Delimiters (void)								{ return (delimiters); }

	bool  Record (void *record, int size = 0);	
	char *Record (void)									{ return ((nest) ? nest_rec : record); }

	char *Reset_Record (void);
	char *Clean_Record (char *string = NULL);
	char *Parse_String (char *string, char field [], int field_size);

	char *Get_Field_Number (int number, char field [], int field_size);
	bool  Set_Field_Number (int number, char *string);

	bool Nested (void)									{ return (nested); }
	void Nested (bool nest_flag)						{ nested = nest_flag; nest = false;}

	bool Nest (void)									{ return (nest); }
	void Nest (bool nest_flag)							{ nest = (nested) ? nest_flag : false; }

private:
	Format_Type  record_format;

	bool nested, nest;

	int record_size, nest_rec_size;
	int max_size, nest_max_size;

	char *record, *nest_rec;
	char *delimiters;

	void Max_Size (int size)							{ if (nest) nest_max_size = size; else max_size = size; }

	static char *comma_delimiter;
	static char *space_delimiter;
	static char *tab_delimiter;
	static char *csv_delimiter;
};
#endif
