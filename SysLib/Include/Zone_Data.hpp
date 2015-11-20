//*********************************************************
//	Zone_Data.hpp - zone data
//*********************************************************

#ifndef ZONE_DATA_HPP
#define ZONE_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Zone_Data class definition
//---------------------------------------------------------

class Zone_Data : public Class_Index
{
public:
	Zone_Data (int zone = 0);
	virtual ~Zone_Data (void)  {}

	int  Zone (void)           { return (ID ()); }
	int  X (void)              { return (x); }
	int  Y (void)              { return (y); }
	int  Area_Type (void)      { return (area_type); }

	void Zone (int value)      { ID (value); }
	void X (int value)         { x = value; }
	void Y (int value)         { y = value; }
	void Area_Type (int value) { area_type = value; }

private:
	int  x;				//---- rounded ----
	int  y;
	int  area_type;
};

//---------------------------------------------------------
//	Zone_Array class definition
//---------------------------------------------------------

class Zone_Array : public Class_Array
{
public:
	Zone_Array (int max_records = 0);

	bool Add (Zone_Data *data = NULL)       { return (Class_Array::Add (data)); }
	
	Zone_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Zone_Data *) Class_Array::New_Record (clear, number)); }

	Zone_Data * Record (int index)          { return ((Zone_Data *) Class_Array::Record (index)); }
	Zone_Data * Record (void)               { return ((Zone_Data *) Class_Array::Record ()); }

	Zone_Data * Get (int zone)              { return ((Zone_Data *) Class_Array::Get (zone)); }
	Zone_Data * Get_GE (int zone)           { return ((Zone_Data *) Class_Array::Get_GE (zone)); }
	Zone_Data * Get_LE (int zone)           { return ((Zone_Data *) Class_Array::Get_LE (zone)); }

	Zone_Data * First (void)                { return ((Zone_Data *) Class_Array::First ()); }
	Zone_Data * Next (void)                 { return ((Zone_Data *) Class_Array::Next ()); }
	
	Zone_Data * Last (void)                 { return ((Zone_Data *) Class_Array::Last ()); }
	Zone_Data * Previous (void)             { return ((Zone_Data *) Class_Array::Previous ()); }

	Zone_Data * First_Key (void)            { return ((Zone_Data *) Class_Array::First_Key ()); }
	Zone_Data * Next_Key (void)             { return ((Zone_Data *) Class_Array::Next_Key ()); }

	Zone_Data * Last_Key (void)             { return ((Zone_Data *) Class_Array::Last_Key ()); }
	Zone_Data * Previous_Key (void)         { return ((Zone_Data *) Class_Array::Previous_Key ()); }

	Zone_Data * operator[] (int index)		{ return (Record (index)); }
};

#endif
