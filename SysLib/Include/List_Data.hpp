//*********************************************************
//	List_Data.hpp - list data classes
//*********************************************************

#ifndef LIST_DATA_HPP
#define LIST_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	List_Data class definition
//---------------------------------------------------------

class List_Data
{
public:
	List_Data (void);

	int  From_List (void)            { return (from_list); }
	int  To_List (void)              { return (to_list); }
	
	void From_List (int value)       { from_list = value; }
	void To_List (int value)         { to_list = value; }

	int  Next (bool ab_flag)         { return ((ab_flag) ? from_list : to_list); }

private:
	int from_list;
	int to_list;
};

//---------------------------------------------------------
//	List_Array
//---------------------------------------------------------

class List_Array : public Data_Array
{
public:
	List_Array (int max_records = 0);

	bool Add (List_Data *data)          { return (Data_Array::Add ((void *) data)); }

	List_Data * First (void)            { return ((List_Data *) Data_Array::First ()); }
	List_Data * Next (void)             { return ((List_Data *) Data_Array::Next ()); }

	List_Data * Last (void)             { return ((List_Data *) Data_Array::Last ()); }
	List_Data * Previous (void)         { return ((List_Data *) Data_Array::Previous ()); }

	List_Data * operator[] (int index)  { return ((List_Data *) Record (index)); }
};

#endif
