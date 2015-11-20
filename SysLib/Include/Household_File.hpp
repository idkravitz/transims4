//******************************************************** 
//	Household_File.hpp - Household File Input/Output
//********************************************************

#ifndef HOUSEHOLD_FILE_HPP
#define HOUSEHOLD_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Household_File Class definition
//---------------------------------------------------------

class  Household_File : public Db_Header
{
public:

	Household_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Household_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Household_File (void);

	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Location (void)				{ Get_Field (location, &lvalue); return (lvalue); }
	int Persons (void)				{ Get_Field (persons, &lvalue); return (lvalue); }
	int Workers (void)				{ Get_Field (workers, &lvalue); return (lvalue); }
	int Vehicles (void)				{ Get_Field (vehicles, &lvalue); return (lvalue); }
	int Type (void)					{ Get_Field (type, &lvalue); return (lvalue); }

	void Household (int value)		{ Put_Field (hhold, value); }
	void Location (int value)		{ Put_Field (location, value); }
	void Persons (int value)		{ Put_Field (persons, value); }
	void Workers (int value)		{ Put_Field (workers, value); }
	void Vehicles (int value)		{ Put_Field (vehicles, value); }
	void Type (int value)			{ Put_Field (type, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int hhold, location, persons, workers, vehicles, type;
};

#endif
