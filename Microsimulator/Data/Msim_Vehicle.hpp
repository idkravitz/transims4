//*********************************************************
//	Msim_Vehicle.hpp - microsimulator vehicle data classes
//*********************************************************

#ifndef MSIM_VEHICLE_HPP
#define MSIM_VEHICLE_HPP

#include "Vehicle_Data.hpp"
#include "Position_Data.hpp"
#include "Travel_Plan_Data.hpp"

//---------------------------------------------------------
//	Msim_Veh_Data class definition
//---------------------------------------------------------

class Msim_Veh_Data : public Position_Data
{
public:
	Msim_Veh_Data (void);
	virtual ~Msim_Veh_Data (void)       { Clear (); }

	int  Status (void)                  { return (status); }
	bool Move_Flag (void)               { return (move_flag != 0); }
	bool Priority (void)                { return (priority != 0); }
	bool Blocked (void)                 { return (blocked != 0); }
	int  Lane_Change (void)             { return (lane_change); }

	bool Hold_Flag (void)               { return (hold_flag != 0); }
	int  Hold (void)                    { return (hold); }

	int  Waiting (void)                 { return (waiting); }
	int  Reserve (void)                 { return (reserve); }
	int  Cycle_Dir (void)               { return (cycle_dir); }

	int  Offset (void)                  { return (offset); }
	int  Speed (void)                   { return (speed); }
	int  Leg (void)                     { return (leg); }

	int  To_Offset (void)               { return (to_offset); }
	int  To_Speed (void)                { return (to_speed); }
	int  To_Leg (void)                  { return (to_leg); }

	void Status (int value)             { status = (char) value; }
	void Move_Flag (bool value)         { move_flag = (value) ? 1 : 0; }
	void Priority (bool value)          { priority = (value) ? 1 : 0; }
	void Blocked (bool value)           { blocked = (value) ? 1 : 0; }
	void Lane_Change (int value)        { lane_change = (char) value; }

	void Hold (int value)               { hold = (unsigned short) value; }
	void Hold_Down (void)               { hold--; }
	void Hold_Flag (bool value)         { hold_flag = (value) ? 1 : 0; }

	void Waiting (int value)            { waiting = value; }
	void Reserve (int value)            { reserve = value; }
	void Cycle_Dir (int value)          { cycle_dir = value; }

	void Offset (int value)             { offset = (short) value; }
	void Speed (int value)              { speed = (short) value; }
	void Leg (int value)                { leg = (short) value; }

	void To_Offset (int value)          { to_offset = (short) value; }
	void To_Speed (int value)           { to_speed = (short) value; }
	void To_Leg (int value)             { to_leg = (short) value; }

	Travel_Plan_Data * Travel_Plan (void)     { return (plan_ptr); }
	void Travel_Plan (Travel_Plan_Data *ptr)  { plan_ptr = ptr; }

private:
	char  status;
	char  move_flag;
	char  priority;
	char  blocked;
	char  lane_change;
	char  hold_flag;
	unsigned short  hold;

	int   waiting;
	int   reserve;
	int   cycle_dir;
	
	short offset;
	short speed;
	short leg;

	short to_offset;
	short to_speed;
	short to_leg;

	Travel_Plan_Data *plan_ptr;
};

//---------------------------------------------------------
//	Msim_Vehicle class definition
//---------------------------------------------------------

class Msim_Vehicle : public Vehicle_Data
{
public:
	Msim_Vehicle (int vehicle = 0);
	virtual ~Msim_Vehicle (void)        { Clear (); }

	int  Driver (void)                  { return (Household ()); }
	void Driver (int value)             { Household (value); }

	int  Num_Cells (void)               { return (num_cells); }
	bool Num_Cells (int value)          { num_cells = value; return (!Check ()); }

	bool Initialize (int num_cells = 0);
	void Clear (void)                   { if (Check ()) delete data; data = NULL;}

	//---- msim_veh_data mapping ----

	int  Status (void)                  { return (Check () ? data->Status () : 0); }
	bool Move_Flag (void)               { return (Check () ? data->Move_Flag () : false); }
	bool Priority (void)                { return (Check () ? data->Priority () : false); }
	bool Blocked (void)                 { return (Check () ? data->Blocked () : false); }
	int  Lane_Change (void)             { return (Check () ? data->Lane_Change () : 0); }

	bool Hold_Flag (void)               { return (Check () ? data->Hold_Flag () : 0); }
	int  Hold (void)                    { return (Check () ? data->Hold () : 0); }

	int  Waiting (void)                 { return (Check () ? data->Waiting () : 0); }
	int  Reserve (void)                 { return (Check () ? data->Reserve () : 0); }
	int  Cycle_Dir (void)               { return (Check () ? data->Cycle_Dir () : 0); }

	int  Offset (void)                  { return (Check () ? data->Offset () : 0); }
	int  Speed (void)                   { return (Check () ? data->Speed () : 0); }
	int  Leg (void)                     { return (Check () ? data->Leg () : 0); }

	int  To_Offset (void)               { return (Check () ? data->To_Offset () : 0); }
	int  To_Speed (void)                { return (Check () ? data->To_Speed () : 0); }
	int  To_Leg (void)                  { return (Check () ? data->To_Leg () : 0); }

	void Status (int value)             { if (Check ()) data->Status (value); }
	void Move_Flag (bool value)         { if (Check ()) data->Move_Flag (value); }
	void Priority (bool value)          { if (Check ()) data->Priority (value); }
	void Blocked (bool value)           { if (Check ()) data->Blocked (value); }
	void Lane_Change (int value)        { if (Check ()) data->Lane_Change (value); }

	void Hold (int value)               { if (Check ()) data->Hold (value); }
	void Hold_Down (void)               { if (Check ()) data->Hold_Down (); }
	void Hold_Flag (bool value)         { if (Check ()) data->Hold_Flag (value); }

	void Waiting (int value)            { if (Check ()) data->Waiting (value); }
	void Reserve (int value)            { if (Check ()) data->Reserve (value); }
	void Cycle_Dir (int value)          { if (Check ()) data->Cycle_Dir (value); }

	void Offset (int value)             { if (Check ()) data->Offset (value); }
	void Speed (int value)              { if (Check ()) data->Speed (value); }
	void Leg (int value)                { if (Check ()) data->Leg (value); }

	void To_Offset (int value)          { if (Check ()) data->To_Offset (value); }
	void To_Speed (int value)           { if (Check ()) data->To_Speed (value); }
	void To_Leg (int value)             { if (Check ()) data->To_Leg (value); }

	Travel_Plan_Data * Travel_Plan (void)     { return (Check () ? data->Travel_Plan () : NULL); }
	void Travel_Plan (Travel_Plan_Data *ptr)  { if (Check ()) data->Travel_Plan (ptr); }

	//---- position_data mapping ----

	int  Dir (void)                     { return (Check () ? data->Dir () : 0); }
	int  Cell (void)                    { return (Check () ? data->Cell () : 0); }
	int  Lane (void)                    { return (Check () ? data->Lane () : 0); }

	int  To_Dir (void)                  { return (Check () ? data->To_Dir () : 0); }
	int  To_Cell (void)                 { return (Check () ? data->To_Cell () : 0); }
	int  To_Lane (void)                 { return (Check () ? data->To_Lane () : 0); }

	int  Dir_Num (int num)              { return (Check () ? data->Dir_Num (num) : 0); }
	int  Cell_Num (int num)             { return (Check () ? data->Cell_Num (num) : 0); }
	int  Lane_Num (int num)             { return (Check () ? data->Lane_Num (num) : 0); }

	void Dir (int value)                { if (Check ()) data->Dir (value); }
	void Cell (int value)               { if (Check ()) data->Cell (value); }
	void Lane (int value)               { if (Check ()) data->Lane (value); }

	void To_Dir (int value)             { if (Check ()) data->To_Dir (value); }
	void To_Cell (int value)            { if (Check ()) data->To_Cell (value); }
	void To_Lane (int value)            { if (Check ()) data->To_Lane (value); }

	void Dir_Num (int num, int value)   { if (Check ()) data->Dir_Num (num, value); }
	void Cell_Num (int num, int value)  { if (Check ()) data->Cell_Num (num, value); }
	void Lane_Num (int num, int value)  { if (Check ()) data->Lane_Num (num, value); }

private:

	bool Check (void)                   { return (data != NULL); }

	int num_cells;

	Msim_Veh_Data *data;
};

//---------------------------------------------------------
//	Msim_Vehicle_Array class definition
//---------------------------------------------------------

class Msim_Vehicle_Array : public Vehicle_Array
{
public:
	Msim_Vehicle_Array (int max_records = 0);

	bool Add (Msim_Vehicle *data = NULL)    { return (Class_Array::Add (data)); }

	Msim_Vehicle * New_Record (bool clear = false, int number = 1)
	                                        { return ((Msim_Vehicle *) Class_Array::New_Record (clear, number)); }


	Msim_Vehicle * Record (int index)       { return ((Msim_Vehicle *) Class_Array::Record (index)); }
	Msim_Vehicle * Record (void)            { return ((Msim_Vehicle *) Class_Array::Record ()); }

	Msim_Vehicle * Get (int vehicle)        { return ((Msim_Vehicle *) Class_Array::Get (vehicle)); }

	Msim_Vehicle * First (void)             { return ((Msim_Vehicle *) Class_Array::First ()); }
	Msim_Vehicle * Next (void)              { return ((Msim_Vehicle *) Class_Array::Next ()); }

	Msim_Vehicle * Last_Key (void)          { return ((Msim_Vehicle *) Class_Array::Last_Key ()); }

	Msim_Vehicle * operator[] (int index)   { return (Record (index)); }
};

#endif
