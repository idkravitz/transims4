//*********************************************************
//	Link_Name.hpp - extended link data class
//*********************************************************

#ifndef LINK_NAME_HPP
#define LINK_NAME_HPP

#include "Link_Data.hpp"
#include "Integer_Array.hpp"

//---------------------------------------------------------
//	Link_Name class definition
//---------------------------------------------------------

class Link_Name : public Link_Data
{
public:
	Link_Name (void);
	virtual ~Link_Name (void)    { Clear (); }

	char * Name (void)           { return (name); }

	bool  Name (char *name);

	void  Clear (void);

private:
	char *name;
};

//---------------------------------------------------------
//	Link_Name_Array class definition
//---------------------------------------------------------

class Link_Name_Array : public Link_Array
{
public:
	Link_Name_Array (int max_records = 0);
	virtual ~Link_Name_Array (void)       { Clear (); }

	bool Add (Link_Name *data = NULL)     { return (Link_Array::Add (data)); }
	
	Link_Name * New_Record (bool clear = false, int number = 1)
	                                      { return ((Link_Name *) Link_Array::New_Record (clear, number)); }

	Link_Name * Get (int ID)              { return ((Link_Name *) Link_Array::Get (ID)); }
	Link_Name * Get_GE (int ID)           { return ((Link_Name *) Link_Array::Get_GE (ID)); }
	Link_Name * Get_LE (int ID)           { return ((Link_Name *) Link_Array::Get_LE (ID)); }

	Link_Name * Record (int index)        { return ((Link_Name *) Link_Array::Record (index)); }
	Link_Name * Record (void)             { return ((Link_Name *) Link_Array::Record ()); }

	Link_Name * First (void)              { return ((Link_Name *) Link_Array::First ()); }
	Link_Name * Next (void)               { return ((Link_Name *) Link_Array::Next ()); }
	
	Link_Name * Last (void)               { return ((Link_Name *) Link_Array::Last ()); }
	Link_Name * Previous (void)           { return ((Link_Name *) Link_Array::Previous ()); }
	
	Link_Name * First_Key (void)          { return ((Link_Name *) Link_Array::First_Key ()); }
	Link_Name * Next_Key (void)           { return ((Link_Name *) Link_Array::Next_Key ()); }

	Link_Name * operator[] (int index)    { return (Record (index)); }

	void Clear (void);
};

#endif
