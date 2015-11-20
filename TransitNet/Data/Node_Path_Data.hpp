//*********************************************************
//	Node_Path_Data.hpp - node data class for path building
//*********************************************************

#ifndef NODE_PATH_DATA_HPP
#define NODE_PATH_DATA_HPP

#include "Node_Data.hpp"

//---------------------------------------------------------
//	Node_Path_Data class definition
//---------------------------------------------------------

class Node_Path_Data : public Node_Data
{
public:
	Node_Path_Data (void);

	int  From_Dir (void)            { return (from_dir); }
	int  To_Dir (void)              { return (to_dir); }
	int  Distance (void)            { return (distance); }
	int  Next_List (void)           { return (next_list); }
	int  From_Node (void)           { return (from_node); }
	int  Link_Dir (void)            { return (link_dir); }

	void From_Dir (int value)       { from_dir = value; }
	void To_Dir (int value)         { to_dir = value; }
	void Distance (int value)       { distance = value; }
	void Next_List (int value)      { next_list = value; }
	void From_Node (int value)      { from_node = value; }
	void Link_Dir (int value)       { link_dir = value; }

private:
	int from_dir;
	int to_dir;
	int distance;
	int next_list;
	int from_node;
	int link_dir;
};

//---------------------------------------------------------
//	Node_Path_Array class definition
//---------------------------------------------------------

class Node_Path_Array : public Node_Array
{
public:
	Node_Path_Array (int max_records = 0);
	virtual ~Node_Path_Array (void)       { Clear (); }

	bool Add (Node_Path_Data *data = NULL)     { return (Node_Array::Add (data)); }
	
	Node_Path_Data * New_Record (bool clear = false, int number = 1)
	                                           { return ((Node_Path_Data *) Node_Array::New_Record (clear, number)); }

	Node_Path_Data * Get (int ID)              { return ((Node_Path_Data *) Node_Array::Get (ID)); }
	Node_Path_Data * Get_GE (int ID)           { return ((Node_Path_Data *) Node_Array::Get_GE (ID)); }
	Node_Path_Data * Get_LE (int ID)           { return ((Node_Path_Data *) Node_Array::Get_LE (ID)); }

	Node_Path_Data * Record (int index)        { return ((Node_Path_Data *) Node_Array::Record (index)); }
	Node_Path_Data * Record (void)             { return ((Node_Path_Data *) Node_Array::Record ()); }

	Node_Path_Data * First (void)              { return ((Node_Path_Data *) Node_Array::First ()); }
	Node_Path_Data * Next (void)               { return ((Node_Path_Data *) Node_Array::Next ()); }
	
	Node_Path_Data * Last (void)               { return ((Node_Path_Data *) Node_Array::Last ()); }
	Node_Path_Data * Previous (void)           { return ((Node_Path_Data *) Node_Array::Previous ()); }
	
	Node_Path_Data * First_Key (void)          { return ((Node_Path_Data *) Node_Array::First_Key ()); }
	Node_Path_Data * Next_Key (void)           { return ((Node_Path_Data *) Node_Array::Next_Key ()); }

	Node_Path_Data * operator[] (int index)    { return (Record (index)); }
};

#endif
