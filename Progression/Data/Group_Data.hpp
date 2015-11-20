//*********************************************************
//	Group_Data.hpp - progression link group class
//*********************************************************

#ifndef GROUP_DATA_HPP
#define GROUP_DATA_HPP

#include "Class_Pointer.hpp"
#include "Class_Index.hpp"
#include "Group_Link.hpp"
#include "Point_Array.hpp"

//---------------------------------------------------------
//	Group_Data class definition
//---------------------------------------------------------

class Group_Data : public Class_Index
{
public:
	Group_Data (int group = 0);
	virtual ~Group_Data (void)        { Clear (); }

	int  Group (void)                 { return (ID ()); }
	int  Next_Group (void)            { return (next); }

	void Group (int value)            { ID (value); }
	void Next_Group (int value)       { next = value; }

	Group_Link * First_Link (void)    { return (link.First ()); }
	Group_Link * Next_Link (void)     { return (link.Next ()); }

	bool Add_Link (Group_Link *lnk)   { return (link.Add (lnk)); }

	void Clear (void)                 { link.Clear (); }
	void Reset (void)                 { link.Reset (); points.Reset (); }

	Point_Data points;

private:
	int next;
	Group_Link_Array link;
};

//---------------------------------------------------------
//	Group_Array class definition
//---------------------------------------------------------

class Group_Array : public Class_Pointer
{
public:
	Group_Array (int max_records = 0);
	virtual ~Group_Array (void)              { Clear (); }

	bool Add (Group_Data *data)              { return (Class_Pointer::Add (data)); }
	
	Group_Data * Get (int group)             { return ((Group_Data *) Class_Pointer::Get (group)); }

	Group_Data * Record (int index)          { return ((Group_Data *) Class_Pointer::Record (index)); }
	Group_Data * Record (void)               { return ((Group_Data *) Class_Pointer::Record ()); }

	Group_Data * First (void)                { return ((Group_Data *) Class_Pointer::First ()); }
	Group_Data * Next (void)                 { return ((Group_Data *) Class_Pointer::Next ()); }
	
	Group_Data * First_Key (void)            { return ((Group_Data *) Class_Pointer::First_Key ()); }
	Group_Data * Next_Key (void)             { return ((Group_Data *) Class_Pointer::Next_Key ()); }

	Group_Data * operator[] (int index)      { return (Record (index)); }

	virtual void Clear (void);
	virtual void Reset (void);
};

#endif
