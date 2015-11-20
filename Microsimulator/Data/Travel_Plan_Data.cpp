//*********************************************************
//	Travel_Plan_Data.cpp - travel plan data classes
//*********************************************************

#include "Travel_Plan_Data.hpp"

//---------------------------------------------------------
//	Travel_Plan_Data constructor
//---------------------------------------------------------

Travel_Plan_Data::Travel_Plan_Data (void)
{
	Traveler (0);
	Vehicle (0); 
	Start_Time (0); 
	End_Time (0);
	Start_Index (0);
	End_Index (0);
	Start_Type (0);
	End_Type (0);
	Mode (0);
	Trip (0);
	Leg (0);
	Start_Cell (0);
	End_Cell (0);
	Start_Offset (0);
	End_Offset (0);
}

//---------------------------------------------------------
//	Travel_Plan_Array constructor
//---------------------------------------------------------

Travel_Plan_Array::Travel_Plan_Array (int max_records) : 
	Data_Array (sizeof (Travel_Plan_Data), max_records)
{
}

Travel_Plan_Array::~Travel_Plan_Array (void)
{
	for (Travel_Plan_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
}

Travel_Plan_Data * Travel_Plan_Array::Copy (int index)
{
	Travel_Plan_Data *plan_ptr, *copy_ptr;

	plan_ptr = Record (index);

	if (plan_ptr == NULL) return (NULL);

	copy_ptr = new Travel_Plan_Data ();

	if (copy_ptr == NULL) return (NULL);

	int nlegs = plan_ptr->Num_Travel_Legs ();

	if (!copy_ptr->Num_Travel_Legs (nlegs)) return (NULL);

	for (int i=1; i <= nlegs; i++) {
		copy_ptr->Travel_Leg (plan_ptr->Travel_Leg (i));
	}
	copy_ptr->Traveler (plan_ptr->Traveler ());
	copy_ptr->Vehicle (plan_ptr->Vehicle ()); 
	copy_ptr->Start_Time (plan_ptr->Start_Time ()); 
	copy_ptr->End_Time (plan_ptr->End_Time ());
	copy_ptr->Start_Index (plan_ptr->Start_Index ());
	copy_ptr->End_Index (plan_ptr->End_Index ());
	copy_ptr->Mode (plan_ptr->Mode ());
	copy_ptr->Trip (plan_ptr->Trip ());
	copy_ptr->Leg (plan_ptr->Leg ());
	copy_ptr->Start_Type (plan_ptr->Start_Type ());
	copy_ptr->End_Type (plan_ptr->End_Type ());
	copy_ptr->Start_Cell (plan_ptr->Start_Cell ());
	copy_ptr->End_Cell (plan_ptr->End_Cell ());
	copy_ptr->Start_Offset (plan_ptr->Start_Offset ());
	copy_ptr->End_Offset (plan_ptr->End_Offset ());

	return (copy_ptr);
}

//---------------------------------------------------------
//	Travel_Plan_Queue constructor
//---------------------------------------------------------

Travel_Plan_Queue::Travel_Plan_Queue (int max_records) : 
	Data_Queue (max_records)
{
}

void Travel_Plan_Queue::Remove (bool delete_flag)
{
	if (delete_flag) {
		Travel_Plan_Data *ptr = (Travel_Plan_Data *) Record ();
		delete ptr;
		//ptr->leg.Clear ();
	}
	Data_Queue::Remove (false);
}
