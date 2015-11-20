//*********************************************************
//	Purpose_Group.hpp - activity purpose control parameters
//*********************************************************

#ifndef PURPOSE_GROUP_HPP
#define PURPOSE_GROUP_HPP

#include "Demand_Code.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Db_File.hpp"
#include "User_Program.hpp"
#include "Network_Code.hpp"

//---------------------------------------------------------
//	Purpose_Group class definition
//---------------------------------------------------------

class Purpose_Group
{
public:
	Purpose_Group (void);
	virtual ~Purpose_Group (void)                 { Clear (); }

	int    Group ()                               { return (group); }
	bool   Anchor ()                              { return (anchor); }
	bool   Model ()                               { return (model); }
	bool   Method ()                              { return (method); }
	int    Zone_Field ()                          { return (zone_field); }
	double Zone_Factor ()                         { return (zone_factor); }
	int    Max_Zone ()                            { return (max_zone); }
	int    Balance_Field ()                       { return (balance_field); }
	double Balance_Factor (int zone)              { return ((Zone_Check (zone)) ? balance_factor [zone] : 1.0); }
	int    Location_Field ()                      { return (location_field); }
	double Location_Factor ()                     { return (location_factor); }
	double Distance_Factor (int mode)             { return ((Mode_Check (mode)) ? distance_factor [mode] : 0); }
	double Time_Factor (int mode)                 { return ((Mode_Check (mode)) ? time_factor [mode] : 0 ); }
	int    Time_Skim ()                           { return (time_skim); }
	int    Time_Field ()                          { return (time_field); }
	char * Skim_Field ()                          { return (skim_field); }

	void Group (int value)                        { group = value; }
	void Anchor (bool flag)                       { anchor = flag; }
	void Method (bool flag)                       { method = flag; }
	void Script_File (Db_File *file)              { script_file = file; model = (file != NULL);}
	void Zone_Field (int value)                   { zone_field = value; }
	void Zone_Factor (double value)               { zone_factor = value; }
	bool Max_Zone (int value);
	void Balance_Field (int value)                { balance_field = value; }
	void Balance_Factor (int zone, double value)  { if (Zone_Check (zone)) balance_factor [zone] = value; }
	void Location_Field (int value)               { location_field = value; }
	void Location_Factor (double value)           { location_factor = value; }
	void Distance_Factor (int mode, double value) { if (Mode_Check (mode)) distance_factor [mode] = value; }
	void Time_Factor (int mode, double value)     { if (Mode_Check (mode)) time_factor [mode] = value; }
	void Time_Skim (int value)                    { time_skim = value; }
	void Time_Field (int value)                   { time_field = value; }
	void Skim_Field (char *value)                 { str_cpy (skim_field, sizeof (skim_field), value); }
	
	Demand_Code::Constraint_Type  Constraint ()   { return (constraint); }
	void Constraint (Demand_Code::Constraint_Type value)  { constraint = value; }

	bool Purpose_Range (char *str);
	bool In_Range (int value)                     { return ((purpose) ? purpose->In_Range (value) : false); }
	int  Max_Purpose (void);

	int  Execute (void);
	bool Compile (int count, Db_Base **files, bool report);
	void Stack_Report (bool header)               { if (program) program->Print_Commands (header); }

	void Clear (void);

private:
	bool Mode_Check (int mode)                    { return (mode > 0 && mode < Network_Code::MAX_MODE); }
	bool Zone_Check (int zone)                    { return (balance_factor != NULL && zone > 0 && zone <= max_zone); }

	Data_Range *purpose;
	Db_File *script_file;
	User_Program *program;
	int  group;
	bool anchor;
	bool model;
	bool method;
	int  zone_field;
	int  location_field;
	int  time_skim;
	int  time_field;
	int  balance_field;
	int  max_zone;
	char skim_field [48];
	double zone_factor;
	double *balance_factor;
	double location_factor;
	double distance_factor [Network_Code::MAX_MODE];
	double time_factor [Network_Code::MAX_MODE];
	Demand_Code::Constraint_Type  constraint;
};

//---------------------------------------------------------
//	Purpose_Group_Array
//---------------------------------------------------------

class Purpose_Group_Array : public Data_Array
{
public:
	Purpose_Group_Array (int max_records = 0);
	virtual ~Purpose_Group_Array (void);

	bool Add (Purpose_Group *data = NULL)   { return (Data_Array::Add ((void *) data)); }

	Purpose_Group * New_Record (bool clear = false, int number = 1)
	                                        { return ((Purpose_Group *) Data_Array::New_Record (clear, number)); }

	Purpose_Group * First (void)            { return ((Purpose_Group *) Data_Array::First ()); }
	Purpose_Group * Next (void)             { return ((Purpose_Group *) Data_Array::Next ()); }

	Purpose_Group * Last (void)             { return ((Purpose_Group *) Data_Array::Last ()); }
	Purpose_Group * Previous (void)         { return ((Purpose_Group *) Data_Array::Previous ()); }

	Purpose_Group * operator[] (int index)  { return ((Purpose_Group *) Record (index)); }

	int Max_Purpose (void);
};

#endif

