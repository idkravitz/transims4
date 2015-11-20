//*********************************************************
//	Link_Nodes.hpp - extended link data class
//*********************************************************

#ifndef LINK_NODES_HPP
#define LINK_NODES_HPP

#include "Link_Name.hpp"
#include "Integer_Array.hpp"

//---------------------------------------------------------
//	Link_Nodes class definition
//---------------------------------------------------------

class Link_Nodes : public Link_Name
{
public:
	Link_Nodes (void);
	virtual ~Link_Nodes (void)   { Clear (); }

	int   First_Node (void)      { return ((list) ? list->First () : 0); }
	int   Next_Node (void)       { return ((list) ? list->Next () : 0); }

	bool  Add_Node (int node);

	void  Clear (void);

	void  Set_Nodes (void)       { list = NULL; }
	void  Flip_Nodes (void)      { if (list) list->Flip_List (); }

	bool  Notes (char *value);
	char *Notes (void)           { return (notes); }

private:
	Integer_List *list;
	char *notes;
};

//---------------------------------------------------------
//	Link_Nodes_Array class definition
//---------------------------------------------------------

class Link_Nodes_Array : public Link_Name_Array
{
public:
	Link_Nodes_Array (int max_records = 0);
	virtual ~Link_Nodes_Array (void)      { Clear (); }

	bool Add (Link_Nodes *data = NULL)    { return (Link_Name_Array::Add (data)); }
	
	Link_Nodes * New_Record (bool clear = false, int number = 1)
	                                      { return ((Link_Nodes *) Link_Name_Array::New_Record (clear, number)); }

	Link_Nodes * Get (int ID)             { return ((Link_Nodes *) Link_Name_Array::Get (ID)); }
	Link_Nodes * Get_GE (int ID)          { return ((Link_Nodes *) Link_Name_Array::Get_GE (ID)); }
	Link_Nodes * Get_LE (int ID)          { return ((Link_Nodes *) Link_Name_Array::Get_LE (ID)); }

	Link_Nodes * Record (int index)       { return ((Link_Nodes *) Link_Name_Array::Record (index)); }
	Link_Nodes * Record (void)            { return ((Link_Nodes *) Link_Name_Array::Record ()); }

	Link_Nodes * First (void)             { return ((Link_Nodes *) Link_Name_Array::First ()); }
	Link_Nodes * Next (void)              { return ((Link_Nodes *) Link_Name_Array::Next ()); }
	
	Link_Nodes * Last (void)              { return ((Link_Nodes *) Link_Name_Array::Last ()); }
	Link_Nodes * Previous (void)          { return ((Link_Nodes *) Link_Name_Array::Previous ()); }
	
	Link_Nodes * First_Key (void)         { return ((Link_Nodes *) Link_Name_Array::First_Key ()); }
	Link_Nodes * Next_Key (void)          { return ((Link_Nodes *) Link_Name_Array::Next_Key ()); }

	Link_Nodes * operator[] (int index)   { return (Record (index)); }

	void Clear (void);
};

#endif
