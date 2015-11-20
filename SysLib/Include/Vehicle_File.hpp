//******************************************************** 
//	Vehicle_File.hpp - Vehicle File Input/Output
//********************************************************

#ifndef VEHICLE_FILE_HPP
#define VEHICLE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Vehicle_File Class definition
//---------------------------------------------------------

class  Vehicle_File : public Db_Header
{
public:

	Vehicle_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Vehicle_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Vehicle_File (void);

	int Vehicle (void)				{ Get_Field (vehicle, &lvalue); return (lvalue); }
	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Location (void)				{ Get_Field (location, &lvalue); return (lvalue); }
	int Type (void)					{ Get_Field (type, &lvalue); return (lvalue); }
	int Sub_Type (void)				{ Get_Field (sub_type, &lvalue); return (lvalue); }

	void Vehicle (int value)		{ Put_Field (vehicle, value); }
	void Household (int value)		{ Put_Field (hhold, value); }
	void Location (int value)		{ Put_Field (location, value); }
	void Type (int value)			{ Put_Field (type, value); }
	void Sub_Type (int value)		{ Put_Field (sub_type, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int vehicle, hhold, location, type, sub_type;
};

#endif
