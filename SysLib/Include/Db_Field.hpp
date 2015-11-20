//*********************************************************
//	Db_Field.hpp - database field
//*********************************************************

#ifndef DB_FIELD_HPP
#define DB_FIELD_HPP

#include "Db_Code.hpp"

#include <stdlib.h>

//---------------------------------------------------------
//	Db_Field Class definition
//---------------------------------------------------------

class  Db_Field : public Db_Code
{
public:
	Db_Field (char *name, Field_Type type, int offset = 0, int size = 0, int decimal = 0, bool buffer = false, bool nested = false);
	Db_Field (void);
	virtual ~Db_Field ();
	
	bool Set_Field (char *name, Field_Type type, int offset = 0, int size = 0, int decimal = 0, bool buffer = false, bool nested = false);

	char *      Name (void)			{ return ((name != NULL) ? name : (char *) ""); }
	Field_Type  Type (void)			{ return (type); }
	int         Offset (void)		{ return (offset); }
	int         Size (void)			{ return (size); }
	int         Decimal (void)		{ return (decimal); }
	char *      Buffer (void)		{ return (buffer); }
	bool        Nested (void)		{ return (nested); }
	bool		Resized (void)		{ return (resized); }

	bool  Name (char *name);
	void  Type (Field_Type value)	{ type = value; }
	void  Offset (int value)		{ offset = (value > 0) ? value : 0; }
	void  Size (int value)			{ size = (value > 0) ? value : 0; if (buffer != NULL) Buffer (true); }
	void  Decimal (int value)		{ decimal = (value > 0) ? value : 0; }
	bool  Buffer (bool flag);
	bool  Buffer (char *value);
	void  Nested (bool flag)		{ nested = flag; }

	void Clear (void);

private:
	char       *name;
	Field_Type  type;
	int         offset;
	int         size;
	int         decimal;
	char       *buffer;
	bool		nested;
	bool		resized;
};
#endif
