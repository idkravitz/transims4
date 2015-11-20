//******************************************************** 
//	Boundary_File.hpp - Boundary Parking Input/Output
//********************************************************

#ifndef BOUNDARY_FILE_HPP
#define BOUNDARY_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Boundary_File Class definition
//---------------------------------------------------------

class  Boundary_File : public Db_Header
{
public:

	Boundary_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Boundary_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Boundary_File (void);

	int Parking (void)				{ Get_Field (parking, &lvalue); return (lvalue); }
	char *Start_Time (void)			{ Get_Field (start, svalue); return (svalue); }
	char *End_Time (void)			{ Get_Field (end, svalue); return (svalue); }
	double Speed (void)				{ Get_Field (speed, &dvalue); return (dvalue); }

	void Parking (int value)		{ Put_Field (parking, value); }
	void Start_Time (char *value)	{ Put_Field (start, value); }
	void End_Time (char *value)		{ Put_Field (end, value); }
	void Speed (double value)		{ Put_Field (speed, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int parking, start, end, speed;
};

#endif
