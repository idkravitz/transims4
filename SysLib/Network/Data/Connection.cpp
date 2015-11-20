//*********************************************************
//	Connection.cpp - directional link connections
//*********************************************************

#include "Connection.hpp"

//---------------------------------------------------------
//	Connection constructor
//---------------------------------------------------------

Connection::Connection (int change) : Class_Index (change)
{
	Dir (0);
	Facility (0);
	Index (0);
	Type (0);
}

//---------------------------------------------------------
//	Connection_Array constructor
//---------------------------------------------------------

Connection_Array::Connection_Array (int max_records) : 
	Class_Array (sizeof (Connection), max_records)
{
}

Connection * Connection_Array::Get_Thru (void)
{
	for (Connection *ptr = First (); ptr; ptr = Next ()) {
		if (ptr->Is_Thru ()) return (ptr);
	}
	return (NULL);
}

Connection * Connection_Array::Get_Dir (int dir)
{
	for (Connection *ptr = First (); ptr; ptr = Next ()) {
		if (ptr->Dir () == dir) return (ptr);
	}
	return (NULL);
}
