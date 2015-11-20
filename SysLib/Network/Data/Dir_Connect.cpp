//*********************************************************
//	Dir_Connect.cpp - directional link list
//*********************************************************

#include "Dir_Connect.hpp"

//---------------------------------------------------------
//	Dir_Connect constructor
//---------------------------------------------------------

Dir_Connect::Dir_Connect (int link_dir) : Dir_List (link_dir)
{
	Set_Dir ();
}

bool Dir_Connect::From_Connection (Connection *ptr)
{
	if (from == NULL) {
		from = new Connection_Array (5);
		if (from == NULL) return (false);
	}
	return (from->Add (ptr));
}

bool Dir_Connect::To_Connection (Connection *ptr)
{
	if (to == NULL) {
		to = new Connection_Array (5);
		if (to == NULL) return (false);
	}
	return (to->Add (ptr));
}

void Dir_Connect::Set_Dir (void)
{
	Type (0);
	Length (0);
	Offset (0);
	Setback (0);
	Left_Turn (0);
	Right_Turn (0);
	Left_Merge (0);
	Right_Merge (0);

	from = to = NULL;
}

void Dir_Connect::Clear (void)
{
	if (from != NULL) {
		delete from;
		from = NULL;
	}
	if (to != NULL) {
		delete to;
		to = NULL;
	}
}

//---------------------------------------------------------
//	Dir_Connect_Array constructor
//---------------------------------------------------------

Dir_Connect_Array::Dir_Connect_Array (int max_records) : 
	Dir_Array (max_records)
{
	Data_Size (sizeof (Dir_Connect));
}

Dir_Connect_Array::~Dir_Connect_Array (void)
{
	for (Dir_Connect *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
}
