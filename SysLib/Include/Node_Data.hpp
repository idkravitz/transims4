//*********************************************************
//	Node_Data.hpp - network node data
//*********************************************************

#ifndef NODE_DATA_HPP
#define NODE_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Node_Data class definition
//---------------------------------------------------------

class Node_Data : public Class_Index
{
public:
	Node_Data (int node = 0);
	virtual ~Node_Data (void) {}

	int  Node (void)          { return (ID ()); }
	int  X (void)             { return (x); }
	int  Y (void)             { return (y); }
	int  Z (void)             { return (z); }

	void Node (int value)     { ID (value); }
	void X (int value)        { x = value; }
	void Y (int value)        { y = value; }
	void Z (int value)        { z = value; }

	int  Type (void)          { return (z); }
	void Type (int value)     { z = value; };

private:
	int  x;				//---- rounded ----
	int  y;
	int  z;
};

//---------------------------------------------------------
//	Node_Array class definition
//---------------------------------------------------------

class Node_Array : public Class_Array
{
public:
	Node_Array (int max_records = 0);

	bool Add (Node_Data *data = NULL)       { return (Class_Array::Add (data)); }
	
	Node_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Node_Data *) Class_Array::New_Record (clear, number)); }

	Node_Data * Record (int index)          { return ((Node_Data *) Class_Array::Record (index)); }
	Node_Data * Record (void)               { return ((Node_Data *) Class_Array::Record ()); }

	Node_Data * Get (int node)              { return ((Node_Data *) Class_Array::Get (node)); }
	Node_Data * Get_GE (int node)           { return ((Node_Data *) Class_Array::Get_GE (node)); }
	Node_Data * Get_LE (int node)           { return ((Node_Data *) Class_Array::Get_LE (node)); }

	Node_Data * First (void)                { return ((Node_Data *) Class_Array::First ()); }
	Node_Data * Next (void)                 { return ((Node_Data *) Class_Array::Next ()); }
	
	Node_Data * Last (void)                 { return ((Node_Data *) Class_Array::Last ()); }
	Node_Data * Previous (void)             { return ((Node_Data *) Class_Array::Previous ()); }

	Node_Data * First_Key (void)            { return ((Node_Data *) Class_Array::First_Key ()); }
	Node_Data * Next_Key (void)             { return ((Node_Data *) Class_Array::Next_Key ()); }

	Node_Data * operator[] (int index)		{ return (Record (index)); }
};

#endif
