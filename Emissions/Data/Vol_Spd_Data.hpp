//*********************************************************
//	Vol_Spd_Data.hpp - volume speed data classes
//*********************************************************

#ifndef VOL_SPD_DATA_HPP
#define VOL_SPD_DATA_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Vol_Spd_Data class definition
//---------------------------------------------------------

class Vol_Spd_Data : public Class2_Index
{
public:
	Vol_Spd_Data (int link = 0, int period = 0);
	~Vol_Spd_Data (void)            { Clear (); }

	int    Link (void)              { return (Key1 ()); }
	int    Period (void)            { return (Key2 ()); }
	int    Fac_Type (void)          { return (fac_type); }
	int    Area_Type (void)         { return (area_type); }
	int    Road_Type (void)         { return (road_type); }
	double Length (void)            { return (length); } 
	double Speed (void)             { return (speed); }
	double Volume (int n)           { return ((n > 0 && n <= num_vol) ? volume [n-1] : 0.0); }

	void   Link (int value)         { Key1 (value); }
	void   Period (int value)       { Key2 (value); }
	void   Fac_Type (int type)      { fac_type = (char) type; }
	void   Area_Type (int type)     { area_type = (char) type; }
	void   Road_Type (int type)     { road_type = (char) type; }
	void   Length (double value)    { length = value; }
	void   Speed (double value)     { speed = value; }
	void   Volume (int n, double v) { if (n > 0 && n <= num_vol) volume [n-1] = v; }

	bool   Num_Volume (int num);
	int    Num_Volume (void)        { return (num_vol); }

	void   Clear (void);

private:
	char   fac_type;
	char   area_type;
	char   road_type;
	char   num_vol;
	double length;
	double speed;
	double *volume;
};

//---------------------------------------------------------
//	Vol_Spd_Array class definition
//---------------------------------------------------------

class Vol_Spd_Array : public Class2_Array
{
public:
	Vol_Spd_Array (int max_records = 10);
	~Vol_Spd_Array (void)                      { Clear (); }

	bool Add (Vol_Spd_Data *data = NULL)       { return (Class2_Array::Add (data)); }

	Vol_Spd_Data * New_Record (bool clear = false, int number = 1)
	                                           { return ((Vol_Spd_Data *) Class2_Array::New_Record (clear, number)); }

	int Get_Index (int key1, int key2)         { Vol_Spd_Data key (key1, key2); return (Get_Index (&key)); }
	int Get_Index (Vol_Spd_Data *data)         { return (Class2_Array::Get_Index (data)); }	

	Vol_Spd_Data * Record (int index)          { return ((Vol_Spd_Data *) Class2_Array::Record (index)); }
	Vol_Spd_Data * Record (void)               { return ((Vol_Spd_Data *) Class2_Array::Record ()); }

	Vol_Spd_Data * Get (int key1, int key2)    { Vol_Spd_Data key (key1, key2); return (Get (&key)); }
	Vol_Spd_Data * Get (Vol_Spd_Data *data)    { return ((Vol_Spd_Data *) Class2_Array::Get (data)); }

	Vol_Spd_Data * Get_GE (int key1, int key2) { Vol_Spd_Data key (key1, key2); return (Get_GE (&key)); }
	Vol_Spd_Data * Get_GE (Vol_Spd_Data *data) { return ((Vol_Spd_Data *) Class2_Array::Get_GE (data)); }

	Vol_Spd_Data * Get_LE (int key1, int key2) { Vol_Spd_Data key (key1, key2); return (Get_LE (&key)); }
	Vol_Spd_Data * Get_LE (Vol_Spd_Data *data) { return ((Vol_Spd_Data *) Class2_Array::Get_LE (data)); }

	Vol_Spd_Data * First (void)                { return ((Vol_Spd_Data *) Class2_Array::First ()); }
	Vol_Spd_Data * Next (void)                 { return ((Vol_Spd_Data *) Class2_Array::Next ()); }
	
	Vol_Spd_Data * Last (void)                 { return ((Vol_Spd_Data *) Class2_Array::Last ()); }
	Vol_Spd_Data * Previous (void)             { return ((Vol_Spd_Data *) Class2_Array::Previous ()); }

	Vol_Spd_Data * First_Key (void)            { return ((Vol_Spd_Data *) Class2_Array::First_Key ()); }
	Vol_Spd_Data * Next_Key (void)             { return ((Vol_Spd_Data *) Class2_Array::Next_Key ()); }

	Vol_Spd_Data * Last_Key (void)             { return ((Vol_Spd_Data *) Class2_Array::Last_Key ()); }
	Vol_Spd_Data * Previous_Key (void)         { return ((Vol_Spd_Data *) Class2_Array::Previous_Key ()); }

	Vol_Spd_Data * operator[] (int index)      { return (Record (index)); }

	int  Num_Volume (void)                     { return (num_vol); }
	void Num_Volume (int num)                  { num_vol = num; }

	void Clear (void);

private:
	int num_vol;
};

#endif
