//*********************************************************
//	Link_Name.cpp - extended link data class
//*********************************************************

#include "Link_Name.hpp"

#include "Utility.hpp"

#include <string.h>

//---------------------------------------------------------
//	Link_Name constructor
//---------------------------------------------------------

Link_Name::Link_Name (void) : Link_Data (0)
{
	name = NULL;
}

bool Link_Name::Name (char *_name)
{
	if (_name != NULL) {
		int len = (int) strlen (_name) + 1;
		
		name = new char [len];

		if (name == NULL) return (false);

		str_cpy (name, len, _name);
	}
	return (true);
}

void Link_Name::Clear (void)
{
	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
}

//---------------------------------------------------------
//	Link_Name_Array constructor
//---------------------------------------------------------

Link_Name_Array::Link_Name_Array (int max_records) : 
	Link_Array (max_records)
{
	Data_Size (sizeof (Link_Name));
}

void Link_Name_Array::Clear (void)
{
	for (Link_Name *ptr = First (); ptr; ptr = Next ()) {
		ptr->Clear ();
	}
}
