//******************************************************** 
//	Fare_File.hpp - Fare File Input/Output
//********************************************************

#ifndef FARE_FILE_HPP
#define FARE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Fare_File Class definition
//---------------------------------------------------------

class  Fare_File : public Db_Header
{
public:

	Fare_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Fare_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Fare_File (void);

	char * From_Zone (void)			{ Get_Field (from_zone, svalue); return (svalue); }
	char * To_Zone (void)			{ Get_Field (to_zone, svalue); return (svalue); }
	char * From_Mode (void)			{ Get_Field (from_mode, svalue); return (svalue); }
	char * To_Mode (void)			{ Get_Field (to_mode, svalue); return (svalue); }
	char * Period (void)			{ Get_Field (period, svalue); return (svalue); }
	char * Class (void)				{ Get_Field (type, svalue); return (svalue); }
	int  Fare (void)				{ Get_Field (fare, &lvalue); return (lvalue); }

	void From_Zone (char * value)	{ Put_Field (from_zone, value); }
	void To_Zone (char * value)		{ Put_Field (to_zone, value); }
	void From_Mode (char * value)	{ Put_Field (from_mode, value); }
	void To_Mode (char * value)		{ Put_Field (to_mode, value); }
	void Period (char * value)		{ Put_Field (period, value); }
	void Class (char * value)		{ Put_Field (type, value); }
	void Fare (int value)			{ Put_Field (fare, value); }

	virtual bool Create_Fields (void);

	bool Old4_Flag (void)			{ return (transfer > 0); }

	int  Transfer (void)			{ Get_Field (transfer, &lvalue); return (lvalue); }
	int  Max_Fare (void)			{ Get_Field (max_fare, &lvalue); return (lvalue); }

	void Transfer (int value)		{ Put_Field (transfer, value); }
	void Max_Fare (int value)		{ Put_Field (max_fare, value); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int from_zone, to_zone, from_mode, to_mode, period, type, fare;
	int transfer, max_fare;
};

#endif
