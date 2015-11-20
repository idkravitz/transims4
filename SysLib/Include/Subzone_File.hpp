//******************************************************** 
//	Subzone_File.hpp - Subzone File Input/Output
//********************************************************

#ifndef SUBZONE_FILE_HPP
#define SUBZONE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Subzone_File Class definition
//---------------------------------------------------------

class  Subzone_File : public Db_Header
{
public:

	Subzone_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Subzone_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Subzone_File (void);

	int ID (void)					{ Get_Field (id, &lvalue); return (lvalue); }
	int Zone (void)					{ Get_Field (zone, &lvalue); return (lvalue); }
	double X (void)					{ Get_Field (x, &dvalue); return (dvalue); }
	double Y (void)					{ Get_Field (y, &dvalue); return (dvalue); }
	int Data (void)					{ Get_Field (data, &lvalue); return (lvalue); }

	void ID (int value)				{ Put_Field (id, value); }
	void Zone (int value)			{ Put_Field (zone, value); }
	void X (double value)			{ Put_Field (x, value); }
	void Y (double value)			{ Put_Field (y, value); }
	void Data (int value)			{ Put_Field (data, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int id, zone, x, y, data;
};

#endif
