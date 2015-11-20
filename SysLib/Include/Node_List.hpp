//*********************************************************
//	Node_List.hpp - extended node data class
//*********************************************************

#ifndef NODE_LIST_HPP
#define NODE_LIST_HPP

#include "Node_Data.hpp"

//---------------------------------------------------------
//	Node_List class definition
//---------------------------------------------------------

class Node_List : public Node_Data
{
public:
	Node_List (void);

	int  Type (void)                { return (type); }
	int  From_Dir (void)            { return (from_dir); }
	int  To_Dir (void)              { return (to_dir); }

	void Type (int value)           { type = value; }
	void From_Dir (int value)       { from_dir = value; }
	void To_Dir (int value)         { to_dir = value; }

private:
	int type;
	int from_dir;
	int to_dir;
};

//---------------------------------------------------------
//	Node_List_Array class definition
//---------------------------------------------------------

class Node_List_Array : public Node_Array
{
public:
	Node_List_Array (int max_records = 0);
	virtual ~Node_List_Array (void)       { Clear (); }

	bool Add (Node_List *data = NULL)     { return (Node_Array::Add (data)); }
	
	Node_List * New_Record (bool clear = false, int number = 1)
	                                      { return ((Node_List *) Node_Array::New_Record (clear, number)); }

	Node_List * Get (int ID)              { return ((Node_List *) Node_Array::Get (ID)); }
	Node_List * Get_GE (int ID)           { return ((Node_List *) Node_Array::Get_GE (ID)); }
	Node_List * Get_LE (int ID)           { return ((Node_List *) Node_Array::Get_LE (ID)); }

	Node_List * Record (int index)        { return ((Node_List *) Node_Array::Record (index)); }
	Node_List * Record (void)             { return ((Node_List *) Node_Array::Record ()); }

	Node_List * First (void)              { return ((Node_List *) Node_Array::First ()); }
	Node_List * Next (void)               { return ((Node_List *) Node_Array::Next ()); }
	
	Node_List * Last (void)               { return ((Node_List *) Node_Array::Last ()); }
	Node_List * Previous (void)           { return ((Node_List *) Node_Array::Previous ()); }
	
	Node_List * First_Key (void)          { return ((Node_List *) Node_Array::First_Key ()); }
	Node_List * Next_Key (void)           { return ((Node_List *) Node_Array::Next_Key ()); }

	Node_List * operator[] (int index)    { return (Record (index)); }
};

#endif
