//******************************************************** 
//	Stop_File.hpp - Transit Stop File Input/Output
//********************************************************

#ifndef STOP_FILE_HPP
#define STOP_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Stop_File Class definition
//---------------------------------------------------------

class  Stop_File : public Db_Header
{
public:

	Stop_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Stop_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Stop_File (void);

	int ID (void)					{ Get_Field (id, &lvalue); return (lvalue); }
	char * Name (void)				{ Get_Field (name, svalue); return (svalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	char * Use (void)				{ Get_Field (use, svalue); return (svalue); }
	char * Type (void)				{ Get_Field (type, svalue); return (svalue); }
	int Space (void)				{ Get_Field (space, &lvalue); return (lvalue); }

	void ID (int value)				{ Put_Field (id, value); }
	void Name (char * value)		{ Put_Field (name, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Use (char * value)			{ Put_Field (use, value); }
	void Type (char * value)		{ Put_Field (type, value); }
	void Space (int value)			{ Put_Field (space, value); }

	bool Use_Flag (void)			{ return (use != 0); }
	bool Type_Flag (void)			{ return (type != 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int id, link, dir, offset, use, type, space, name;
};

#endif
