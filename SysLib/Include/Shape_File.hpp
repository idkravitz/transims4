//******************************************************** 
//	Shape_File.hpp - Shape Point File Input/Output
//********************************************************

#ifndef SHAPE_FILE_HPP
#define SHAPE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Shape_File Class definition
//---------------------------------------------------------

class  Shape_File : public Db_Header
{
public:
	Shape_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Shape_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Shape_File (void);

	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Points (void)				{ Get_Field (points, &lvalue); return (lvalue); }
	double X (void)					{ Get_Field (x, &dvalue); return (dvalue); }
	double Y (void)					{ Get_Field (y, &dvalue); return (dvalue); }
	double Z (void)                 { Get_Field (z, &dvalue); return (dvalue); }

	void Link (int value)			{ Put_Field (link, value); }
	void Points (int value)			{ Put_Field (points, value); }
	void X (double value)			{ Put_Field (x, value); }
	void Y (double value)			{ Put_Field (y, value); }
	void Z (double value)           { Put_Field (z, value); }

	virtual bool Create_Fields (void);

	bool Z_Flag (void)              { return (z_flag); }
	void Z_Flag (bool flag)         { z_flag = flag; }

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;
	bool z_flag;

	int link, points, x, y, z;
};

#endif
