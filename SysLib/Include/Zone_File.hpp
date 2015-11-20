//******************************************************** 
//	Zone_File.hpp - Zone File Input/Output
//********************************************************

#ifndef ZONE_FILE_HPP
#define ZONE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Zone_File Class definition
//---------------------------------------------------------

class  Zone_File : public Db_Header
{
public:

	Zone_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Zone_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Zone_File (void);

	int Zone (void)					{ Get_Field (zone, &lvalue); return (lvalue); }
	double X (void)					{ Get_Field (x, &dvalue); return (dvalue); }
	double Y (void)					{ Get_Field (y, &dvalue); return (dvalue); }
	int Area_Type (void)			{ Get_Field (area, &lvalue); return (lvalue); }
	double Min_X (void)				{ Get_Field (min_x, &dvalue); return (dvalue); }
	double Min_Y (void)				{ Get_Field (min_y, &dvalue); return (dvalue); }
	double Max_X (void)				{ Get_Field (max_x, &dvalue); return (dvalue); }
	double Max_Y (void)				{ Get_Field (max_y, &dvalue); return (dvalue); }

	void Zone (int value)			{ Put_Field (zone, value); }
	void X (double value)			{ Put_Field (x, value); }
	void Y (double value)			{ Put_Field (y, value); }
	void Area_Type (int value)		{ Put_Field (area, value); }
	void Min_X (double value)		{ Put_Field (min_x, value); }
	void Min_Y (double value)		{ Put_Field (min_y, value); }
	void Max_X (double value)		{ Put_Field (max_x, value); }
	void Max_Y (double value)		{ Put_Field (max_y, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int zone, x, y, area, min_x, min_y, max_x, max_y;
};

#endif
