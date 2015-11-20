//*********************************************************
//	Skim_Data.cpp - skim data classes
//*********************************************************

#include "Skim_Data.hpp"

//---------------------------------------------------------
//	Skim_Data constructor
//---------------------------------------------------------

Skim_Data::Skim_Data (int key) : Class_Index (key)
{
	New_Skim ();
}

void Skim_Data::Add_Skim (Skim_Data *skim) 
{
	Add_Count (skim->Count ());
	Add_Walk (skim->Walk ());
	Add_Wait (skim->Wait ());
	Add_Transit (skim->Transit ());
	Add_Drive (skim->Drive ());
	Add_Other (skim->Other ());
	Add_Length (skim->Length ());
	Add_Cost (skim->Cost ());
}

void Skim_Data::New_Skim (void) 
{
	Count (0);
	Walk (0);
	Wait (0);
	Transit (0);
	Drive (0);
	Other (0);
	Length (0);
	Cost (0);
}

//---------------------------------------------------------
//	Skim_Array constructor
//---------------------------------------------------------

Skim_Array::Skim_Array (int num_periods, int num_zones, int max_records) : 
	Class_Array (sizeof (Skim_Data), max_records, false), Matrix_Key (num_periods, num_zones)
{
}
