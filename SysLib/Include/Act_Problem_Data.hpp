//*********************************************************
//	Act_Problem_Data.hpp - activity problem summary class
//*********************************************************

#ifndef ACT_PROBLEM_DATA_HPP
#define ACT_PROBLEM_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Activity_Problem_Data class definition
//---------------------------------------------------------

class Activity_Problem_Data : public Class_Index
{
public:
	Activity_Problem_Data (int group = 0);

	int    Group (void)                { return (ID ()); }
	int    Count (void)                { return (count); }
	int    Schedule (void)             { return (schedule); }
	int    Vehicle (void)              { return (vehicle); }
	int    Location (void)             { return (location); }

	void   Group (int value)           { ID (value); }
	void   Count (int value)           { count = value; }
	void   Schedule (int value)        { schedule = value; }
	void   Vehicle (int value)         { vehicle = value; }
	void   Location (int value)        { location = value; }

	void   Add_Problem (int count = 1, int schedule = 0, int vehicle = 0, int location = 0);

private:
	int  count;
	int  schedule;
	int  vehicle;
	int  location;
};

//---------------------------------------------------------
//	Activity_Problem_Array class definition
//---------------------------------------------------------

class Activity_Problem_Array : public Class_Array, public Static_Service
{
public:
	Activity_Problem_Array (int max_records = 20);
	virtual ~Activity_Problem_Array (void);

	bool Add (Activity_Problem_Data *data = NULL)   { return (Class_Array::Add (data)); }
	
	Activity_Problem_Data * New_Record (bool clear = false, int number = 1)
	                                                { return ((Activity_Problem_Data *) Class_Array::New_Record (clear, number)); }

	Activity_Problem_Data * Get (int id)            { return ((Activity_Problem_Data *) Class_Array::Get (id)); }

	Activity_Problem_Data * Record (int index)      { return ((Activity_Problem_Data *) Class_Array::Record (index)); }
	Activity_Problem_Data * Record (void)           { return ((Activity_Problem_Data *) Class_Array::Record ()); }

	Activity_Problem_Data * First (void)            { return ((Activity_Problem_Data *) Class_Array::First ()); }
	Activity_Problem_Data * Next (void)             { return ((Activity_Problem_Data *) Class_Array::Next ()); }
	
	Activity_Problem_Data * First_Key (void)        { return ((Activity_Problem_Data *) Class_Array::First_Key ()); }
	Activity_Problem_Data * Next_Key (void)         { return ((Activity_Problem_Data *) Class_Array::Next_Key ()); }

	Activity_Problem_Data * operator[] (int index)  { return (Record (index)); }

	//---- standard processing methods -----

	bool Add_Problem (int group, int count = 1, int schedule = 0, int vehicle = 0, int locatio = 0);
	void Report (int number, char *title, char *key1, char *key2 = NULL);
	void Header ();

private:
	int keys;
	char *title, *key1, *key2;
};

#endif
