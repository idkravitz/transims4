//*********************************************************
//	File_Group.hpp - file group control parameters
//*********************************************************

#ifndef FILE_GROUP_HPP
#define FILE_GROUP_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"
#include "Diurnal_File.hpp"
#include "Factor_File.hpp"
#include "Time_Range.hpp"
#include "Time_Equiv.hpp"
#include "Diurnal_Target.hpp"
#include "Tour_File.hpp"

//---------------------------------------------------------
//	File_Group class definition
//---------------------------------------------------------

class File_Group : public Class_Index
{
public:
	File_Group (void);
	virtual ~File_Group (void)           { Clear (); }

	int  Group (void)                    { return (ID ()); }
	Diurnal_File * Time_File ()          { return (time_file); }
	Factor_File *Trip_Factor ()          { return (trip_factor); }
	int  Purpose ()                      { return (purpose); }
	int  Mode ()                         { return (mode); }
	int  Method ()                       { return (method); }
	int  Type ()                         { return (type); }
	int  SubType ()                      { return (subtype); }
	int  Org_Wt ()                       { return (org_wt); }
	int  Des_Wt ()                       { return (des_wt); }
	bool Dist_Wt ()                      { return (dist_wt); }
	int  Stop_Wt ()                      { return (stop_wt); }
	int  Duration ()                     { return (duration); }
	int  Speed ()                        { return (speed); }
	double Scaling_Factor ()             { return (scaling); }

	void Group (int value)               { ID (value); }
	void Time_File (Diurnal_File *file)  { time_file = file; }
	void Trip_Factor (Factor_File *file) { trip_factor = file; }
	void Purpose (int value)             { purpose = value; }
	void Mode (int value)                { mode = value; }
	void Method (int value)              { method = value; }
	void Type (int value)                { type = value; }
	void SubType (int value)             { subtype = value; }
	void Org_Wt (int value)              { org_wt = value; }
	void Des_Wt (int value)              { des_wt = value; }
	void Dist_Wt (bool flag)             { dist_wt = flag; }
	void Stop_Wt (int value)             { stop_wt = value; }
	void Duration (int value)            { duration = value; }
	void Speed (int value)               { speed = value; }
	void Scaling_Factor (double value)   { scaling = value; }

	void Diurnal_Data (Diurnal_Target_Array *value) { diurnal_data = value; }
	Diurnal_Target_Array * Diurnal_Data (void) { return (diurnal_data); }

	int  Time_Field (void)               { return ((time_file) ? time_file->Share_Field () : 0); }
	bool Time_Field (char *field);

	bool In_Range (Range_Data *r)        { return ((period) ? period->Span_Range (r) : true); }
	bool Time_Periods (char *str);

	bool Time_Equiv_File (char *filename);
	int  Factor_Periods (bool equiv_report = false);
	int  Factor_Period (int time)         { return ((time_equiv) ? time_equiv->Period (time) : 1); }

	int  Num_Shares (void)               { return ((time_file) ? time_file->Num_Fields () : 0); }
	double Share (int num);

	void Clear (void);

private:
	Diurnal_File *time_file;
	Factor_File *trip_factor;
	int  purpose;
	int  mode;
	int  method;
	int  type;
	int  subtype;
	int  org_wt;
	int  des_wt;
	bool dist_wt;
	int  stop_wt;
	int  duration;
	int  speed;				//---- rounded ----
	double scaling;
	Diurnal_Target_Array *diurnal_data;
	Time_Range *period;
	Time_Equiv *time_equiv;
};

//---------------------------------------------------------
//	File_Group_Array
//---------------------------------------------------------

class File_Group_Array : public Class_Array
{
public:
	File_Group_Array (int max_records = 0);
	virtual ~File_Group_Array (void);

	bool Add (File_Group *data = NULL)   { return (Class_Array::Add (data)); }

	File_Group * New_Record (bool clear = false, int number = 1)
										 { return ((File_Group *) Class_Array::New_Record (clear, number)); }
	
	File_Group * Record (int index)      { return ((File_Group *) Class_Array::Record (index)); }
	File_Group * Record (void)           { return ((File_Group *) Class_Array::Record ()); }

	File_Group * Get (int node)          { return ((File_Group *) Class_Array::Get (node)); }
	File_Group * Get_GE (int node)       { return ((File_Group *) Class_Array::Get_GE (node)); }
	File_Group * Get_LE (int node)       { return ((File_Group *) Class_Array::Get_LE (node)); }

	File_Group * First (void)            { return ((File_Group *) Class_Array::First ()); }
	File_Group * Next (void)             { return ((File_Group *) Class_Array::Next ()); }

	File_Group * Last (void)             { return ((File_Group *) Class_Array::Last ()); }
	File_Group * Previous (void)         { return ((File_Group *) Class_Array::Previous ()); }

	File_Group * First_Key (void)        { return ((File_Group *) Class_Array::First_Key ()); }
	File_Group * Next_Key (void)         { return ((File_Group *) Class_Array::Next_Key ()); }

	File_Group * operator[] (int index)  { return ((File_Group *) Record (index)); }
};

#endif

