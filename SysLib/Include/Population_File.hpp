//******************************************************** 
//	Population_File.hpp - Population File Input/Output
//********************************************************

#ifndef POPULATION_FILE_HPP
#define POPULATION_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Population_File Class definition
//---------------------------------------------------------

class  Population_File : public Db_Header
{
public:

	Population_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Population_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Population_File (void);

	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Person (void)				{ Get_Field (person, &lvalue); return (lvalue); }
	int Relate (void)				{ Get_Field (relate, &lvalue); return (lvalue); }
	int Age (void)					{ Get_Field (age, &lvalue); return (lvalue); }
	int Gender (void)				{ Get_Field (gender, &lvalue); return (lvalue); }
	int Work (void)					{ Get_Field (work, &lvalue); return (lvalue); }
	int Drive (void)				{ Get_Field (drive, &lvalue); return (lvalue); }

	void Household (int value)		{ Put_Field (hhold, value); }
	void Person (int value)			{ Put_Field (person, value); }
	void Relate (int value)			{ Put_Field (relate, value); }
	void Age (int value)            { Put_Field (age, value); }
	void Gender (int value)         { Put_Field (gender, value); }
	void Work (int value)			{ Put_Field (work, value); }
	void Drive (int value)			{ Put_Field (drive, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int hhold, person, relate, age, gender, work, drive;
};

#endif
