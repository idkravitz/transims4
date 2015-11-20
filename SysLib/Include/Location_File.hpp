//******************************************************** 
//	Location_File.hpp - Activity Location File Input/Output
//********************************************************

#ifndef LOCATION_FILE_HPP
#define LOCATION_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Location_File Class definition
//---------------------------------------------------------

class  Location_File : public Db_Header
{
public:

	Location_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Location_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Location_File (void);

	int Location (void)				{ Get_Field (location, &lvalue); return (lvalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	double X (void)					{ Get_Field (x, &dvalue); return (dvalue); }
	double Y (void)					{ Get_Field (y, &dvalue); return (dvalue); }
	double Z (void)					{ Get_Field (z, &dvalue); return (dvalue); }
	int Zone (void)					{ Get_Field (zone, &lvalue); return (lvalue); }

	void Location (int value)		{ Put_Field (location, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Node (int value)			{ Put_Field (node, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void X (double value)			{ Put_Field (x, value); }
	void Y (double value)			{ Put_Field (y, value); }
	void Z (double value)			{ Put_Field (z, value); }
	void Zone (int value)			{ Put_Field (zone, value); }

	int  Zone_Field (void)			{ return (zone); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int location, link, node, offset, x, y, z, zone;
};

#endif
