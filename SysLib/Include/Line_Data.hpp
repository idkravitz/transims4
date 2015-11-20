//*********************************************************
//	Line_Data.hpp - network transit route data
//*********************************************************

#ifndef LINE_DATA_HPP
#define LINE_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Line_Data class definition
//---------------------------------------------------------

class Line_Data : public Class_Index
{
public:
	Line_Data (int id = 0);
	virtual ~Line_Data (void)                   { Line_Data::Clear (); }

	int  Route (void)                           { return (ID ()); }
	int  Mode (void)                            { return (mode); }
	int  Stops (void)                           { return (nstops); }
	int  Runs (void)                            { return (nruns); }
	int  Stop (int num)                         { return (Stop_Index (num) ? stop [s].id : 0); }
	int  Zone (int num)                         { return (Stop_Index (num) ? stop [s].zone : 0); }
	int  Time_Flag (int num)                    { return (Stop_Index (num) ? stop [s].flag : 0); }
	int  Schedule (int run, int num)            { return (Index (run, num) ? stop [s].schedule [r] : 0); }
	int  Penalty (int run, int num);
	int  Driver (void)                          { return (driver); }

	void Route (int value)                      { ID (value); }
	void Mode (int value)                       { mode = value; }
	bool Stops (int value);
	bool Runs (int value);
	void Stop (int num, int value)              { if (Stop_Index (num)) stop [s].id = value; }
	void Zone (int num, int value)              { if (Stop_Index (num)) stop [s].zone = value; }
	void Time_Flag (int num, int value)         { if (Stop_Index (num)) stop [s].flag = value; }
	void Schedule (int run, int num, int time)  { if (Index (run, num)) stop [s].schedule [r] = time; }
	void Penalty (int run, int num, int imp);
	void Driver (int value)                     { driver = value; }

	void   Null_Name (void)                     { name = NULL; }
	const char * Name (void)                    { return ((name != NULL) ? name : ""); }
	bool   Name (char *value);
	
	void   Null_Notes (void)                    { notes = NULL; }
	const char * Notes (void)                   { return ((notes != NULL) ? notes : ""); }
	bool   Notes (char *value);

	void Clear (void);
	void Reset (void)                           { nstops = 0; nruns = 0; }

	//---- overloaded methods ----

	int  New_Line (int num)                     { return (Stop_Index (num) ? stop [s].zone : 0); }
	int  New_Stop (int num)                     { return (Stop_Index (num) ? stop [s].flag : 0); }
	
	void New_Line (int num, int value)          { if (Stop_Index (num)) stop [s].zone = value; }
	void New_Stop (int num, int value)          { if (Stop_Index (num)) stop [s].flag = value; }

protected:

	bool Stop_Index (int num)                   { s = num-1; return (s >= 0 && s < nstops); }
	bool Run_Index (int run)                    { r = run-1; return (r >= 0 && r < nruns); }
	bool Index (int run, int num)               { return (Stop_Index (num) && Run_Index (run)); }

	static int  s, r;

private:
	typedef struct {
		int id;
		int zone;
		int flag;
		int *schedule;		//---- rounded ----
		int *penalty;		//---- rounded ----
	} Line_Stop;

	int mode;
	int nstops;
	int nruns;
	int driver;
	Line_Stop *stop;
	char * name;
	char * notes;
};

//---------------------------------------------------------
//	Line_Array class definition
//---------------------------------------------------------

class Line_Array : public Class_Array
{
public:
	Line_Array (int max_records = 0);
	virtual ~Line_Array (void);

	bool Add (Line_Data *data = NULL)    { return (Class_Array::Add (data)); }
	
	Line_Data * New_Record (bool clear = false, int number = 1)
	                                     { return ((Line_Data *) Class_Array::New_Record (clear, number)); }

	Line_Data * Record (int index)       { return ((Line_Data *) Class_Array::Record (index)); }
	Line_Data * Record (void)            { return ((Line_Data *) Class_Array::Record ()); }

	Line_Data * Get (int id)             { return ((Line_Data *) Class_Array::Get (id)); }
	Line_Data * Get_GE (int id)          { return ((Line_Data *) Class_Array::Get_GE (id)); }
	Line_Data * Get_LE (int id)          { return ((Line_Data *) Class_Array::Get_LE (id)); }

	Line_Data * First (void)             { return ((Line_Data *) Class_Array::First ()); }
	Line_Data * Next (void)              { return ((Line_Data *) Class_Array::Next ()); }
	
	Line_Data * Last (void)              { return ((Line_Data *) Class_Array::Last ()); }
	Line_Data * Previous (void)          { return ((Line_Data *) Class_Array::Previous ()); }

	Line_Data * First_Key (void)         { return ((Line_Data *) Class_Array::First_Key ()); }
	Line_Data * Next_Key (void)          { return ((Line_Data *) Class_Array::Next_Key ()); }

	Line_Data * Last_Key (void)          { return ((Line_Data *) Class_Array::Last_Key ()); }
	Line_Data * Previous_Key (void)      { return ((Line_Data *) Class_Array::Previous_Key ()); }

	Line_Data * operator[] (int index)   { return (Record (index)); }

	int Num_Runs (void)                  { return (num_runs); }
	int Max_Runs (void)                  { return (max_runs); }
	int Vehicle_ID (void)                { return (veh_id); }
	int Route_Records (void)             { return (route_records); }
	int Schedule_Records (void)          { return (schedule_records); }

	int Vehicle_ID (int route, int run)  { return (veh_id + route * offset + run); }
	int Route (int vehicle_id)           { return ((vehicle_id - veh_id) / offset); }
	int Run (int vehicle_id)             { return (vehicle_id % offset); }

	void Num_Runs (int value)            { num_runs = value; }
	void Max_Runs (int value)            { max_runs = value; }
	int  Vehicle_ID (int vehicles);
	void Route_Records (int value)       { route_records = value; }
	void Schedule_Records (int value)    { schedule_records = value; }

private:
	int num_runs;
	int max_runs;
	int veh_id;
	int offset;
	int route_records;
	int schedule_records;
};

#endif
