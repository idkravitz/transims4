//******************************************************** 
//	Occupancy_File.hpp - Occupancy File Input/Output
//********************************************************

#ifndef OCCUPANCY_FILE_HPP
#define OCCUPANCY_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Occupancy_File Class definition
//---------------------------------------------------------

class  Occupancy_File : public Db_Header
{
public:

	Occupancy_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Occupancy_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Occupancy_File (void);

	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	char *Start (void)				{ Get_Field (start, svalue); return (svalue); }
	char *End (void)				{ Get_Field (end, svalue); return (svalue); }
	int Lane (void)					{ Get_Field (lane, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	int Occupancy (void)			{ Get_Field (occupancy, &lvalue); return (lvalue); }

	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }
	void Start (char *value)		{ Put_Field (start, value); }
	void End (char *value)			{ Put_Field (end, value); }
	void Lane (int value)			{ Put_Field (lane, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Occupancy (int value)		{ Put_Field (occupancy, value); }

	virtual bool Create_Fields (void);

	int  Link_Field (void)			{ return (link); }
	int  Dir_Field (void)			{ return (dir); }
	int  Start_Field (void)			{ return (start); }
	int  End_Field (void)			{ return (end); }
	int  Lane_Field (void)			{ return (lane); }
	int  Offset_Field (void)		{ return (offset); }
	int  Occupancy_Field (void)		{ return (occupancy); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int link, dir, start, end, lane, offset, occupancy;
};

#endif
