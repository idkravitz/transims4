//******************************************************** 
//	Trip_File.hpp - Trip File Input/Output
//********************************************************

#ifndef TRIP_FILE_HPP
#define TRIP_FILE_HPP

#include "Ext_Header.hpp"

//---------------------------------------------------------
//	Trip_File Class definition
//---------------------------------------------------------

class  Trip_File : public Ext_Header
{
public:

	Trip_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Trip_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Trip_File (void);

	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Person (void)				{ Get_Field (person, &lvalue); return (lvalue); }
	int Trip (void)					{ Get_Field (trip, &lvalue); return (lvalue); }
	int Purpose (void)				{ Get_Field (purpose, &lvalue); return (lvalue); }
	int Mode (void)					{ Get_Field (mode, &lvalue); return (lvalue); }
	int Vehicle (void)				{ Get_Field (vehicle, &lvalue); return (lvalue); }
	char * Start (void)				{ Get_Field (start, svalue); return (svalue); }
	int Origin (void)				{ Get_Field (origin, &lvalue); return (lvalue); }
	char * Arrive (void)			{ Get_Field (arrive, svalue); return (svalue); }
	int Destination (void)			{ Get_Field (destination, &lvalue); return (lvalue); }
	int Constraint (void)           { Get_Field (limit, &lvalue); return (lvalue); }

	void Household (int value)		{ Put_Field (hhold, value); }
	void Person (int value)			{ Put_Field (person, value); }
	void Trip (int value)			{ Put_Field (trip, value); }
	void Purpose (int value)		{ Put_Field (purpose, value); }
	void Mode (int value)			{ Put_Field (mode, value); }
	void Vehicle (int value)		{ Put_Field (vehicle, value); }
	void Start (char * value)		{ Put_Field (start, value); }
	void Origin (int value)			{ Put_Field (origin, value); }
	void Arrive (char * value)		{ Put_Field (arrive, value); }
	void Destination (int value)	{ Put_Field (destination, value); }
	void Constraint (int value)     { Put_Field (limit, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int hhold, person, trip, purpose, mode, vehicle, start, origin, arrive, destination, limit;
};

#endif
