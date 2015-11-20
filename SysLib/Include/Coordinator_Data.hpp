//*********************************************************
//	Coordinator_Data.hpp - network signal coordinator data
//*********************************************************

#ifndef COORDINATOR_DATA_HPP
#define COORDINATOR_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Coordinator_Data class definition
//---------------------------------------------------------

class Coordinator_Data : public Class_Index
{
public:
	Coordinator_Data (int id = 0);
	virtual ~Coordinator_Data (void) {}

private:

};

//---------------------------------------------------------
//	Coordinator_Array class definition
//---------------------------------------------------------

class Coordinator_Array : public Class_Array
{
public:
	Coordinator_Array (int max_records = 0);

	bool Add (Coordinator_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Coordinator_Data * New_Record (bool clear = false, int number = 1)
                                   { return ((Coordinator_Data *) Class_Array::New_Record (clear, number)); }

	Coordinator_Data * Record (int index)     { return ((Coordinator_Data *) Class_Array::Record (index)); }
	Coordinator_Data * Record (void)          { return ((Coordinator_Data *) Class_Array::Record ()); }

	Coordinator_Data * Get (int id)           { return ((Coordinator_Data *) Class_Array::Get (id)); }
	Coordinator_Data * Get_GE (int id)        { return ((Coordinator_Data *) Class_Array::Get_GE (id)); }
	Coordinator_Data * Get_LE (int id)        { return ((Coordinator_Data *) Class_Array::Get_LE (id)); }

	Coordinator_Data * First (void)           { return ((Coordinator_Data *) Class_Array::First ()); }
	Coordinator_Data * Next (void)            { return ((Coordinator_Data *) Class_Array::Next ()); }
	
	Coordinator_Data * Last (void)            { return ((Coordinator_Data *) Class_Array::Last ()); }
	Coordinator_Data * Previous (void)        { return ((Coordinator_Data *) Class_Array::Previous ()); }

	Coordinator_Data * First_Key (void)       { return ((Coordinator_Data *) Class_Array::First_Key ()); }
	Coordinator_Data * Next_Key (void)        { return ((Coordinator_Data *) Class_Array::Next_Key ()); }

	Coordinator_Data * operator[] (int index) { return (Record (index)); }
};

#endif
