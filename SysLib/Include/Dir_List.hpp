//*********************************************************
//	Dir_List.hpp - directional link list
//*********************************************************

#ifndef DIR_LIST_HPP
#define DIR_LIST_HPP

#include "Dir_Data.hpp"
#include "List_Data.hpp"

//---------------------------------------------------------
//	Dir_List class definition
//---------------------------------------------------------

class Dir_List : public Dir_Data, public List_Data
{
public:
	Dir_List (int link_dir = 0);
	virtual ~Dir_List (void) {}
};

//---------------------------------------------------------
//	Dir_List_Array class definition
//---------------------------------------------------------

class Dir_List_Array : public Dir_Array
{
public:
	Dir_List_Array (int max_records = 0);
	virtual ~Dir_List_Array (void)         {}

	bool Add (Dir_List *data = NULL)       { return (Dir_Array::Add (data)); }
	
	Dir_List * New_Record (bool clear = false, int number = 1)
	                                       { return ((Dir_List *) Dir_Array::New_Record (clear, number)); }

	Dir_List * Record (int index)          { return ((Dir_List *) Dir_Array::Record (index)); }
	Dir_List * Record (void)               { return ((Dir_List *) Dir_Array::Record ()); }

	Dir_List * Get (int link)              { return ((Dir_List *) Dir_Array::Get (link)); }
	Dir_List * Get_GE (int link)           { return ((Dir_List *) Dir_Array::Get_GE (link)); }
	Dir_List * Get_LE (int link)           { return ((Dir_List *) Dir_Array::Get_LE (link)); }

	Dir_List * First (void)                { return ((Dir_List *) Dir_Array::First ()); }
	Dir_List * Next (void)                 { return ((Dir_List *) Dir_Array::Next ()); }
	
	Dir_List * Last (void)                 { return ((Dir_List *) Dir_Array::Last ()); }
	Dir_List * Previous (void)             { return ((Dir_List *) Dir_Array::Previous ()); }

	Dir_List * First_Key (void)            { return ((Dir_List *) Dir_Array::First_Key ()); }
	Dir_List * Next_Key (void)             { return ((Dir_List *) Dir_Array::Next_Key ()); }

	Dir_List * operator[] (int index)      { return (Record (index)); }
};

#endif
