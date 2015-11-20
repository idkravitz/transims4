//*********************************************************
//	Access_Data.hpp - network process link data
//*********************************************************

#ifndef ACCESS_DATA_HPP
#define ACCESS_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Access class definition
//---------------------------------------------------------

class Access_Data : public Class_Index
{
public:
	Access_Data (int id = 0);
	virtual ~Access_Data (void)  {}

	int  From_ID (void)          { return (from_id); }
	int  To_ID (void)            { return (to_id); }
	int  From_Type (void)        { return (from_type); }
	int  To_Type (void)          { return (to_type); }
	int  Time (void)             { return (time); }
	int  Cost (void)             { return (cost); }

	void From_ID (int value)     { from_id = value; }
	void To_ID (int value)       { to_id = value; }
	void From_Type (int value)   { from_type = (unsigned char) value; }
	void To_Type (int value)     { to_type = (unsigned char) value; }
	void Time (int value)        { time = (unsigned short) value; }
	void Cost (int value)        { cost = (unsigned short) value; }

private:
	int            from_id;
	int            to_id;
	unsigned char  from_type;
	unsigned char  to_type;
	unsigned short time;			//---- rounded ----
	unsigned short cost;
};

//---------------------------------------------------------
//	Access_Array class definition
//---------------------------------------------------------

class Access_Array : public Class_Array
{
public:
	Access_Array (int max_records = 0);

	bool Add (Access_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Access_Data * New_Record (bool clear = false, int number = 1)
	                                     { return ((Access_Data *) Class_Array::New_Record (clear, number)); }

	Access_Data * Record (int index)     { return ((Access_Data *) Class_Array::Record (index)); }
	Access_Data * Record (void)          { return ((Access_Data *) Class_Array::Record ()); }

	Access_Data * Get (int id)           { return ((Access_Data *) Class_Array::Get (id)); }
	Access_Data * Get_GE (int id)        { return ((Access_Data *) Class_Array::Get_GE (id)); }
	Access_Data * Get_LE (int id)        { return ((Access_Data *) Class_Array::Get_LE (id)); }

	Access_Data * First (void)           { return ((Access_Data *) Class_Array::First ()); }
	Access_Data * Next (void)            { return ((Access_Data *) Class_Array::Next ()); }
	
	Access_Data * Last (void)            { return ((Access_Data *) Class_Array::Last ()); }
	Access_Data * Previous (void)        { return ((Access_Data *) Class_Array::Previous ()); }

	Access_Data * First_Key (void)       { return ((Access_Data *) Class_Array::First_Key ()); }
	Access_Data * Next_Key (void)        { return ((Access_Data *) Class_Array::Next_Key ()); }

	Access_Data * operator[] (int index) { return (Record (index)); }
};

#endif
