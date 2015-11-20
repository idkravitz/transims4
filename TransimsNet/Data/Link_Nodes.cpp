//*********************************************************
//	Link_Nodes.cpp - extended link data class
//*********************************************************

#include "Link_Nodes.hpp"

#include "Utility.hpp"

#include <string.h>

//---------------------------------------------------------
//	Link_Nodes constructor
//---------------------------------------------------------

Link_Nodes::Link_Nodes (void) : Link_Name ()
{
	list = NULL;
	notes = NULL;
}

bool Link_Nodes::Add_Node (int node)
{
	if (list == NULL) {
		list = new Integer_List (4);
		if (list == NULL) return (false);
	}
	list->Add (node);
	return (true);
}

void Link_Nodes::Clear (void)
{
	if (notes != NULL) {
		delete [] notes;
		notes = NULL;
	}
	if (list != NULL) {
		delete list;
		list = NULL;
	}
	Link_Name::Clear ();
}

bool Link_Nodes::Notes (char *value)
{
	if (value != NULL) {
		int len = (int) strlen (value) + 1;
		
		notes = new char [len];

		if (notes == NULL) return (false);

		str_cpy (notes, len, value);
	}
	return (true);
}

//---------------------------------------------------------
//	Link_Nodes_Array constructor
//---------------------------------------------------------

Link_Nodes_Array::Link_Nodes_Array (int max_records) : 
	Link_Name_Array (max_records)
{
	Data_Size (sizeof (Link_Nodes));
}

void Link_Nodes_Array::Clear (void)
{
	for (Link_Nodes *ptr = First (); ptr; ptr = Next ()) {
		ptr->Clear ();
	}
}
