//********************************************************* 
//	Db_Base.hpp - database file
//*********************************************************

#ifndef DB_BASE_HPP
#define DB_BASE_HPP

#include "Db_File.hpp"
#include "Db_Field.hpp"
#include "Data_Array.hpp"

#include <stdio.h>
#include <time.h>

#define END_OF_RECORD	-1

//---------------------------------------------------------
//	Db_Base Class definition
//---------------------------------------------------------

class  Db_Base : public Db_File
{
public:
	Db_Base (Access_Type access = READ, Format_Type format = UNFORMATED);
	Db_Base (char *filename, Access_Type access = READ, Format_Type format = UNFORMATED);
	virtual ~Db_Base (void);

	void Initialize (Access_Type access = READ, Format_Type format = UNFORMATED);

	Format_Type Dbase_Format (void)				{ return (format); }
	void Dbase_Format (Format_Type type);
	void Dbase_Format (char *text)				{ Dbase_Format (Format_Code (text)); }

	bool Add_Field (char *name, Field_Type type, int size = 0, int decimal = 0, int offset = END_OF_RECORD, bool binary = false, bool nested = false); 

	int  Num_Fields (void)						{ return (field.Num_Records ()); }

	int   Field_Number (char *name);

	Db_Field * Field (int fld)					{ return ((Db_Field *) field.Record (fld)); }
	Db_Field * Field (char *name);

	bool  Get_Field (int fld, int *data)		{ return (Read_Field (Field (fld), (void *) data, INTEGER)); }
	bool  Get_Field (int fld, double *data)		{ return (Read_Field (Field (fld), (void *) data, DOUBLE)); }
	bool  Get_Field (int fld, char *data)		{ return (Read_Field (Field (fld), (void *) data, STRING)); }
	bool  Get_Field (int fld, time_t *data)		{ return (Read_Field (Field (fld), (void *) data, DATE_TIME)); }
	
	bool  Get_Field (char *name, int *data)		{ return (Read_Field (Field (name), (void *) data, INTEGER)); }
	bool  Get_Field (char *name, double *data)	{ return (Read_Field (Field (name), (void *) data, DOUBLE)); }
	bool  Get_Field (char *name, char *data)	{ return (Read_Field (Field (name), (void *) data, STRING)); }
	bool  Get_Field (char *name, time_t *data)	{ return (Read_Field (Field (name), (void *) data, DATE_TIME)); }
	
	bool  Put_Field (int fld, int data)			{ return (Write_Field (Field (fld), (void *) &data, INTEGER)); }
	bool  Put_Field (int fld, double data)		{ return (Write_Field (Field (fld), (void *) &data, DOUBLE)); }
	bool  Put_Field (int fld, char *data)		{ return (Write_Field (Field (fld), (void *)  data, STRING)); }
	bool  Put_Field (int fld, time_t data)		{ return (Write_Field (Field (fld), (void *) &data, DATE_TIME)); }
	
	bool  Put_Field (char *name, int data)		{ return (Write_Field (Field (name), (void *) &data, INTEGER)); }
	bool  Put_Field (char *name, double data)	{ return (Write_Field (Field (name), (void *) &data, DOUBLE)); }
	bool  Put_Field (char *name, char *data)	{ return (Write_Field (Field (name), (void *)  data, STRING)); }
	bool  Put_Field (char *name, time_t data)	{ return (Write_Field (Field (name), (void *) &data, DATE_TIME)); }

	int Required_Field (char *name1, char *name2 = NULL, char *name3 = NULL, char *name4 = NULL, char *name5 = NULL);
	int Optional_Field (char *name1, char *name2 = NULL, char *name3 = NULL, char *name4 = NULL, char *name5 = NULL);

	bool Copy_Fields (Db_Base *input);
	void Clear_Fields (void);

	virtual bool Replicate_Fields (Db_Base *file, bool notes_flag = true);
	virtual bool Write_Fields (void)			{ return (false); }

	char * Reset_Record (void);

protected:

	virtual bool Input_Record (char *record, int size);
	virtual char * Output_Record (char *record);

	void Binary_Text_Size (Field_Type type, int *size, int *decimal);
	void Text_Binary_Size (Field_Type type, int *size);

private:
	Data_Array field;
	Format_Type format;

	bool  Read_Field (Db_Field *fld, void *data, Field_Type type);
	bool  Write_Field (Db_Field *fld, void *data, Field_Type type);

	static char buffer [FIELD_BUFFER];	
	static char svalue [FIELD_BUFFER];
};
#endif
