//*********************************************************
//	Household_Model.cpp - household model class
//*********************************************************

#include "Household_Model.hpp"

//---------------------------------------------------------
//	Household_Model constructor
//---------------------------------------------------------

Household_Model::Household_Model (int model) :
	PUMA_Data_Array ()
{
	Model (model);
	Location_Field (0);
	Total_Field (0);
}

//---------------------------------------------------------
//	Household_Model_Array constructor
//---------------------------------------------------------

Household_Model_Array::Household_Model_Array (int max_records) : 
	Data_Pointer_Array (max_records)
{
}

void Household_Model_Array::Delete_Pointers (void)
{
	for (Household_Model *hh_ptr = First (); hh_ptr; hh_ptr = Next ()) {
		delete hh_ptr;
	}
	Data_Array::Reset ();
}

