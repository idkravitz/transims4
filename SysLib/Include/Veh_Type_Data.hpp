//*********************************************************
//	Veh_Type_Data.hpp - vehicle type data classes
//*********************************************************

#ifndef VEH_TYPE_DATA_HPP
#define VEH_TYPE_DATA_HPP

#include "Class_Array.hpp"
#include "Network_Code.hpp"

//---------------------------------------------------------
//	Veh_Type_Data class definition
//---------------------------------------------------------

class Veh_Type_Data : public Class_Index
{
public:
	Veh_Type_Data (int type = 0, int subtype = 0);

	int    Type (void)             { return ((ID () >> 8)); }
	int    Sub_Type (void)         { return ((ID () & 0xFF)); }
	int    Length (void)           { return (length); }
	int    Max_Speed (void)        { return (max_speed); }
	int    Max_Accel (void)        { return (max_accel); }
	int    Max_Decel (void)        { return (max_decel); }
	int    Capacity (void)         { return (capacity); }
	double Loading (void)          { return (loading); }
	double Unloading (void)        { return (unloading); }
	bool   Method (void)           { return (method); }
	int    Min_Dwell (void)        { return (min_dwell); }
	int    Max_Dwell (void)        { return (max_dwell); }

	void Type (int value)          { ID ((value << 8) + Sub_Type ()); }
	void Sub_Type (int value)      { ID ((Type () << 8) + value); }
	void Length (int value)        { length = value; }
	void Max_Speed (int value)     { max_speed = value; }
	void Max_Accel (int value)     { max_accel = value; }
	void Max_Decel (int value)     { max_decel = value; }
	void Capacity (int value)      { capacity = value; }
	void Loading (double value)    { loading = value; }
	void Unloading (double value)  { unloading = value; }
	void Method (bool flag)        { method = flag; }
	void Min_Dwell (int value)     { min_dwell = value; }
	void Max_Dwell (int value)     { max_dwell = value; }

	Network_Code::Use_Type Use (void)       { return (use); }
	void Use (Network_Code::Use_Type value) { use = value; }

private:
	Network_Code::Use_Type use;
	int    length;		//---- rounded ----
	int    max_speed;	//---- rounded ----
	int    max_accel;	//---- rounded ----
	int    max_decel;	//---- rounded ----
	int    capacity;
	double loading;		//---- rounded ----
	double unloading;	//---- rounded ----
	bool   method;		//---- parallel (true) / serial (false) ----
	int    min_dwell;	//---- rounded ----
	int    max_dwell;	//---- rounded ----
};

//---------------------------------------------------------
//	Veh_Type_Array class definition
//---------------------------------------------------------

class Veh_Type_Array : public Class_Array
{
public:
	Veh_Type_Array (int max_records = 10);

	bool Add (Veh_Type_Data *data = NULL)       { return (Class_Array::Add (data)); }

	Veh_Type_Data * New_Record (bool clear = false, int number = 1)
	                                            { return ((Veh_Type_Data *) Class_Array::New_Record (clear, number)); }

	Veh_Type_Data * Record (int index)          { return ((Veh_Type_Data *) Class_Array::Record (index)); }
	Veh_Type_Data * Record (void)               { return ((Veh_Type_Data *) Class_Array::Record ()); }

	Veh_Type_Data * Get (int type, int subtype) { Veh_Type_Data rec (type, subtype); 
												  return ((Veh_Type_Data *) Class_Array::Get (&rec)); }

	Veh_Type_Data * First (void)                { return ((Veh_Type_Data *) Class_Array::First ()); }
	Veh_Type_Data * Next (void)                 { return ((Veh_Type_Data *) Class_Array::Next ()); }

	Veh_Type_Data * operator[] (int index)      { return (Record (index)); }
};

#endif
