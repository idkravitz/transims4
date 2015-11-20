//*********************************************************
//	Boundary_Data.hpp - boundary data class
//*********************************************************

#ifndef BOUNDARY_DATA_HPP
#define BOUNDARY_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"
#include "Lookup_Table.hpp"

//---------------------------------------------------------
//	Boundary_Data class definition
//---------------------------------------------------------

class Boundary_Data : public Class_Index, public Lookup_Table
{
public:
	Boundary_Data (int lot = 0);
	virtual ~Boundary_Data (void)           { Clear (); }

	int  Parking (void)                     { return (ID ()); }
	int  Speed (int period)                 { return (Data (period, 1)); }

	void Parking (int lot)                  { ID (lot); }
	void Speed (int period, int value)      { Data (period, 1, value); }

	int  Periods (void)                     { return (Rows ()); }
	bool Periods (int periods)              { return (Size (periods, 1)); }
};


//---------------------------------------------------------
//	Boundary_Array class definition
//---------------------------------------------------------

class Boundary_Array : public Class_Array, public Lookup_Size
{
public:
	Boundary_Array (int max_records = 0);
	virtual ~Boundary_Array (void)           { Boundary_Array::Clear (); }

	bool Add (Boundary_Data *data = NULL)    { return (Class_Array::Add (data)); }
	
	Boundary_Data * New_Record (bool clear = false, int number = 1)
	                                         { return ((Boundary_Data *) Class_Array::New_Record (clear, number)); }

	Boundary_Data * Get (int lot)            { return ((Boundary_Data *) Class_Array::Get (lot)); }
	Boundary_Data * Get_GE (int lot)         { return ((Boundary_Data *) Class_Array::Get_GE (lot)); }
	Boundary_Data * Get_LE (int lot)         { return ((Boundary_Data *) Class_Array::Get_LE (lot)); }

	Boundary_Data * Record (int index)       { return ((Boundary_Data *) Class_Array::Record (index)); }
	Boundary_Data * Record (void)            { return ((Boundary_Data *) Class_Array::Record ()); }

	Boundary_Data * First (void)             { return ((Boundary_Data *) Class_Array::First ()); }
	Boundary_Data * Next (void)              { return ((Boundary_Data *) Class_Array::Next ()); }
	
	Boundary_Data * Last (void)              { return ((Boundary_Data *) Class_Array::Last ()); }
	Boundary_Data * Previous (void)          { return ((Boundary_Data *) Class_Array::Previous ()); }
	
	Boundary_Data * First_Key (void)         { return ((Boundary_Data *) Class_Array::First_Key ()); }
	Boundary_Data * Next_Key (void)          { return ((Boundary_Data *) Class_Array::Next_Key ()); }

	Boundary_Data * operator[] (int index)   { return (Record (index)); }

	virtual void Clear (void);

	int  Period (int tod)                    { return (Row (tod)); }

	int  Period_Size (void)                  { return (Row_Size ()); }
	void Period_Size (int value)             { Row_Size (value); Rows ((MIDNIGHT + Row_Size () - 1) / Row_Size ()); }

	int  Periods (void)                      { return (Rows ()); }
	void Periods (int value)                 { Rows (value); }
};

#endif
