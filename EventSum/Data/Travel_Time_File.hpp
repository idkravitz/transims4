//******************************************************** 
//	Travel_Time_File.hpp - Travel Time Input/Output
//********************************************************

#ifndef TRAVEL_TIME_FILE_HPP
#define TRAVEL_TIME_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Travel_Time_File Class definition
//---------------------------------------------------------

class  Travel_Time_File : public Db_Header
{
public:

	Travel_Time_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Travel_Time_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Travel_Time_File (void);

	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Person (void)				{ Get_Field (person, &lvalue); return (lvalue); }
	int Trip (void)					{ Get_Field (trip, &lvalue); return (lvalue); }
	int Leg (void)					{ Get_Field (leg, &lvalue); return (lvalue); }
	int Mode (void)					{ Get_Field (mode, &lvalue); return (lvalue); }
	int Purpose (void)				{ Get_Field (purpose, &lvalue); return (lvalue); }
	int Constraint (void)			{ Get_Field (constraint, &lvalue); return (lvalue); }
	int Start_Link (void)			{ Get_Field (start_link, &lvalue); return (lvalue); }
	int End_Link (void)				{ Get_Field (end_link, &lvalue); return (lvalue); }
	char * Trip_Start (void)		{ Get_Field (trip_start, svalue); return (svalue); }
	char * Base_Start (void)		{ Get_Field (base_start, svalue); return (svalue); }
	char * Started (void)			{ Get_Field (started, svalue); return (svalue); }
	char * Start_Diff (void)		{ Get_Field (start_diff, svalue); return (svalue); }    
	char * Trip_End (void)			{ Get_Field (trip_end, svalue); return (svalue); }
	char * Base_End (void)			{ Get_Field (base_end, svalue); return (svalue); }
	char * Ended (void)				{ Get_Field (ended, svalue); return (svalue); }
	char * End_Diff (void)			{ Get_Field (end_diff, svalue); return (svalue); }
	char * Mid_Trip (void)			{ Get_Field (mid_trip, svalue); return (svalue); }
	char * Travel_Time (void)		{ Get_Field (ttime, svalue); return (svalue); }

	void Household (int value)		{ Put_Field (hhold, value); }
	void Person (int value)			{ Put_Field (person, value); }
	void Trip (int value)			{ Put_Field (trip, value); }
	void Leg (int value)			{ Put_Field (leg, value); }
	void Mode (int value)			{ Put_Field (mode, value); }
	void Purpose (int value)		{ Put_Field (purpose, value); }
	void Constraint (int value)		{ Put_Field (constraint, value); }
	void Start_Link (int value)		{ Put_Field (start_link, value); }
	void End_Link (int value)		{ Put_Field (end_link, value); }
	void Trip_Start (char *value)	{ Put_Field (trip_start, value); }
	void Base_Start (char *value)	{ Put_Field (base_start, value); }
	void Started (char *value)		{ Put_Field (started, value); }
	void Start_Diff (char *value)	{ Put_Field (start_diff, value); }
	void Trip_End (char *value)		{ Put_Field (trip_end, value); }
	void Base_End (char *value)		{ Put_Field (base_end, value); }
	void Ended (char *value)		{ Put_Field (ended, value); }
	void End_Diff (char *value)		{ Put_Field (end_diff, value); }
	void Mid_Trip (char *value)		{ Put_Field (mid_trip, value); }
	void Travel_Time (char *value)	{ Put_Field (ttime, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int hhold, person, trip, leg, mode, base_start, started, start_diff;
	int base_end, ended, end_diff, mid_trip, ttime;
	int purpose, constraint, start_link, end_link, trip_start, trip_end;
};

#endif
