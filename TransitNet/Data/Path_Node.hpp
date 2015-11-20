//*********************************************************
//	Path_Node.hpp - path node data
//*********************************************************

#ifndef PATH_NODE_HPP
#define PATH_NODE_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Path_Node class definition
//---------------------------------------------------------

class Path_Node
{
public:
	Path_Node (void);

	int  Node (void)              { return (node); }
	int  Dir (void)               { return (dir); }

	void Node (int value)         { node = value; }
	void Dir (int value)          { dir = value; }

private:
	int node;
	int dir;
};

//---------------------------------------------------------
//	Path_Node_Array class definition
//---------------------------------------------------------

class Path_Node_Array : public Data_Array
{
public:
	Path_Node_Array (int max_records = 20);
	virtual ~Path_Node_Array (void)       { Clear (); }

	bool Add (Path_Node *data = NULL)     { return (Data_Array::Add (data)); }
	
	Path_Node * New_Record (bool clear = false, int number = 1)
	                                       { return ((Path_Node *) Data_Array::New_Record (clear, number)); }

	Path_Node * Record (int index)        { return ((Path_Node *) Data_Array::Record (index)); }
	Path_Node * Record (void)             { return ((Path_Node *) Data_Array::Record ()); }

	Path_Node * First (void)              { return ((Path_Node *) Data_Array::First ()); }
	Path_Node * Next (void)               { return ((Path_Node *) Data_Array::Next ()); }
	
	Path_Node * Last (void)               { return ((Path_Node *) Data_Array::Last ()); }
	Path_Node * Previous (void)           { return ((Path_Node *) Data_Array::Previous ()); }

	Path_Node * operator[] (int index)    { return (Record (index)); }
};

#endif
