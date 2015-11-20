//*********************************************************
//	AB_Data.cpp - network link A->B access classes
//*********************************************************

#include "AB_Data.hpp"

//---------------------------------------------------------
//	AB_Key constructor
//---------------------------------------------------------

AB_Key::AB_Key (int max_records) :
	Complex_Array (sizeof (AB_Data), 2, false, max_records, false)
{
}
