//*********************************************************
//	File_Group.hpp - trip table control parameters
//*********************************************************

#ifndef FILE_GROUP_HPP
#define FILE_GROUP_HPP

#include "Data_Array.hpp"
#include "Matrix_File.hpp"
#include "Diurnal_File.hpp"
#include "Factor_File.hpp"
#include "Db_File.hpp"
#include "Time_Range.hpp"
#include "Time_Equiv.hpp"
#include "User_Program.hpp"

//---------------------------------------------------------
//	File_Group class definition
//---------------------------------------------------------

class File_Group
{
public:
	File_Group (void);
	virtual ~File_Group (void)           { Clear (); }

	int  Group ()                        { return (group); }
	Matrix_File * Trip_File ()           { return (trip_file); }
	Diurnal_File * Time_File ()          { return (time_file); }
	Factor_File *Trip_Factor ()          { return (trip_factor); }
	Db_File *Script_File ()              { return (script_file); }
	int  Purpose ()                      { return (purpose); }
	int  Mode ()                         { return (mode); }
	int  Method ()                       { return (method); }
	int  Duration ()                     { return (duration); }
	int  Type ()                         { return (type); }
	int  SubType ()                      { return (subtype); }
	int  Org_Wt ()                       { return (org_wt); }
	int  Des_Wt ()                       { return (des_wt); }
	bool Dist_Wt ()                      { return (dist_wt); }
	int  Speed ()                        { return (speed); }
	double Scaling_Factor ()             { return (scaling); }

	void Group (int value)               { group = value; }
	void Trip_File (Matrix_File *file)   { trip_file = file; }
	void Time_File (Diurnal_File *file)  { time_file = file; }
	void Trip_Factor (Factor_File *file) { trip_factor = file; }
	void Script_File (Db_File *file)     { script_file = file; }
	void Purpose (int value)             { purpose = value; }
	void Mode (int value)                { mode = value; }
	void Method (int value)              { method = value; }
	void Duration (int value)            { duration = value; }
	void Type (int value)                { type = value; }
	void SubType (int value)             { subtype = value; }
	void Org_Wt (int value)              { org_wt = value; }
	void Des_Wt (int value)              { des_wt = value; }
	void Dist_Wt (bool flag)             { dist_wt = flag; }
	void Speed (int value)               { speed = value; }
	void Scaling_Factor (double value)   { scaling = value; }

	int  Time_Field (void)               { return ((time_file) ? time_file->Share_Field () : 0); }
	bool Time_Field (char *field);

	bool In_Range (Range_Data *r)        { return ((period) ? period->Span_Range (r) : true); }
	bool Time_Periods (char *str);

	bool Time_Equiv_File (char *filename);
	int  Factor_Periods (bool equiv_report = false);
	int  Factor_Period (int time)         { return ((time_equiv) ? time_equiv->Period (time) : 1); }

	int  Execute (void);
	bool Compile (bool report);
	void Stack_Report (bool header)      { if (program) program->Print_Commands (header); }

	int  Num_Shares (void)               { return ((header) ? header->Num_Fields () : 0); }
	double Share (int num);

	void Clear (void);

private:
	Matrix_File *trip_file;
	Diurnal_File *time_file;
	Factor_File *trip_factor;
	Db_File *script_file;
	int  group;
	int  purpose;
	int  mode;
	int  method;
	int  duration;
	int  type;
	int  subtype;
	int  org_wt;
	int  des_wt;
	bool dist_wt;
	int  speed;				//---- rounded ----
	double scaling;
	Time_Range *period;
	Time_Equiv *time_equiv;
	User_Program *program;
	Db_Base *header;
};

//---------------------------------------------------------
//	File_Group_Array
//---------------------------------------------------------

class File_Group_Array : public Data_Array
{
public:
	File_Group_Array (int max_records = 0);
	virtual ~File_Group_Array (void);

	bool Add (File_Group *data = NULL)   { return (Data_Array::Add ((void *) data)); }

	File_Group * New_Record (bool clear = false, int number = 1)
										 { return ((File_Group *) Data_Array::New_Record (clear, number)); }

	File_Group * First (void)            { return ((File_Group *) Data_Array::First ()); }
	File_Group * Next (void)             { return ((File_Group *) Data_Array::Next ()); }

	File_Group * Last (void)             { return ((File_Group *) Data_Array::Last ()); }
	File_Group * Previous (void)         { return ((File_Group *) Data_Array::Previous ()); }

	File_Group * operator[] (int index)  { return ((File_Group *) Record (index)); }
};

#endif

