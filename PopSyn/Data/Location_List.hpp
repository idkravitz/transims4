//*********************************************************
//	Location_List.hpp - activity location data
//*********************************************************

#ifndef LOCATION_LIST_HPP
#define LOCATION_LIST_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Location_List class definition
//---------------------------------------------------------

class Location_List : public Class_Index
{
public:
	Location_List (int id = 0);

	int    Location (void)                { return (ID ()); }
	int    List (void)                    { return (list); }
	int    Parking (void)                 { return (parking); }
	int    Num_Weight (void)              { return (num_weight); }
	double Weight (int num)               { return (Check_Num (num) ? weight [num-1] : 0.0); }

	void   Location (int value)           { ID (value); }
	void   List (int value)               { list = value; }
	void   Parking (int value)            { parking = value; }
	void   Num_Weight (int value)         { num_weight = value; }
	void   Weight (int num, double value) { if (Check_Num (num)) weight [num-1] = value; }

private:
	bool   Check_Num (int num)            { return ((num > 0 && num <= num_weight)); }

	int    list;
	int    parking;
	int    num_weight;
	double weight [1];
};

//---------------------------------------------------------
//	Location_List_Array class definition
//---------------------------------------------------------

class Location_List_Array : public Class_Array
{
public:
	Location_List_Array (int max_records = 0);
	virtual ~Location_List_Array (void)     { Clear (); }

	bool Add (Location_List *data = NULL)   { return (Class_Array::Add (data)); }
	
	Location_List * New_Record (bool clear = false, int number = 1)
	                                        { return ((Location_List *) Class_Array::New_Record (clear, number)); }

	Location_List * Get (int ID)            { return ((Location_List *) Class_Array::Get (ID)); }
	Location_List * Get_GE (int ID)         { return ((Location_List *) Class_Array::Get_GE (ID)); }
	Location_List * Get_LE (int ID)         { return ((Location_List *) Class_Array::Get_LE (ID)); }

	Location_List * Record (int index)      { return ((Location_List *) Class_Array::Record (index)); }
	Location_List * Record (void)           { return ((Location_List *) Class_Array::Record ()); }

	Location_List * First (void)            { return ((Location_List *) Class_Array::First ()); }
	Location_List * Next (void)             { return ((Location_List *) Class_Array::Next ()); }
	
	Location_List * Last (void)             { return ((Location_List *) Class_Array::Last ()); }
	Location_List * Previous (void)         { return ((Location_List *) Class_Array::Previous ()); }
	
	Location_List * First_Key (void)        { return ((Location_List *) Class_Array::First_Key ()); }
	Location_List * Next_Key (void)         { return ((Location_List *) Class_Array::Next_Key ()); }

	Location_List * operator[] (int index)  { return (Record (index)); }

	int  Num_Weight (void)                  { return (num_weight); }
	void Num_Weight (int value);

	Location_List * New_List (int location);

private:
	int num_weight;
};

#endif
