//******************************************************** 
//	Activity_File.hpp - Activity File Input/Output
//********************************************************

#ifndef ACTIVITY_FILE_HPP
#define ACTIVITY_FILE_HPP

#include "Ext_Header.hpp"

//---------------------------------------------------------
//	Activity_File Class definition
//---------------------------------------------------------

class  Activity_File : public Ext_Header
{
public:

	Activity_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Activity_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Activity_File (void);

	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Person (void)				{ Get_Field (person, &lvalue); return (lvalue); }
	int Activity (void)				{ Get_Field (activity, &lvalue); return (lvalue); }
	int Purpose (void)				{ Get_Field (purpose, &lvalue); return (lvalue); }
	int Priority (void)				{ Get_Field (priority, &lvalue); return (lvalue); }
	char * Start_Min (void)			{ Get_Field (st_min, svalue); return (svalue); }
	char * Start_Max (void)			{ Get_Field (st_max, svalue); return (svalue); }
	char * End_Min (void)			{ Get_Field (end_min, svalue); return (svalue); }
	char * End_Max (void)			{ Get_Field (end_max, svalue); return (svalue); }
	char * Time_Min (void)			{ Get_Field (time_min, svalue); return (svalue); }
	char * Time_Max (void)			{ Get_Field (time_max, svalue); return (svalue); }
	int Mode (void)					{ Get_Field (mode, &lvalue); return (lvalue); }
	int Vehicle (void)				{ Get_Field (vehicle, &lvalue); return (lvalue); }
	int Location (void)				{ Get_Field (location, &lvalue); return (lvalue); }
	int Passengers (void)			{ Get_Field (num_pass, &lvalue); return (lvalue); }
	int Constraint (void)			{ Get_Field (limit, &lvalue); return (lvalue); }

	void Household (int value)		{ Put_Field (hhold, value); }
	void Person (int value)			{ Put_Field (person, value); }
	void Activity (int value)		{ Put_Field (activity, value); }
	void Purpose (int value)		{ Put_Field (purpose, value); }
	void Priority (int value)		{ Put_Field (priority, value); }
	void Start_Min (char * value)	{ Put_Field (st_min, value); }
	void Start_Max (char * value)	{ Put_Field (st_max, value); }
	void End_Min (char * value)		{ Put_Field (end_min, value); }
	void End_Max (char * value)		{ Put_Field (end_max, value); }
	void Time_Min (char * value)	{ Put_Field (time_min, value); }
	void Time_Max (char * value)	{ Put_Field (time_max, value); }
	void Mode (int value)			{ Put_Field (mode, value); }
	void Vehicle (int value)		{ Put_Field (vehicle, value); }
	void Location (int value)		{ Put_Field (location, value); }
	void Passengers (int value)		{ Put_Field (num_pass, value); }
	void Constraint (int value)		{ Put_Field (limit, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int hhold, person, activity, purpose, priority;
	int st_min, st_max, end_min, end_max, time_min, time_max;
	int mode, vehicle, location, num_pass, limit;
};

#endif
