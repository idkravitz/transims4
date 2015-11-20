//*********************************************************
//	Vol_Spd_Data.cpp - volume speed data classes
//*********************************************************

#include "Vol_Spd_Data.hpp"

//---------------------------------------------------------
//	Vol_Spd_Data constructor
//---------------------------------------------------------

Vol_Spd_Data::Vol_Spd_Data (int link, int period) : Class2_Index (link, period)
{
	num_vol = 0;
	volume = NULL;

	Fac_Type (0);
	Area_Type (0);
	Road_Type (0);
	Length (0.0);
	Speed (0.0); 
}

bool Vol_Spd_Data::Num_Volume (int num) 
{
	Clear ();

	if (num < 1) return (false);

	volume = new double [num];
	if (volume == NULL) return (false);
	num_vol = num;
	return (true);
}

void Vol_Spd_Data::Clear (void)
{
	if (num_vol > 0) {
		delete [] volume;
		volume = NULL;
		num_vol = 0;
	}
}

//---------------------------------------------------------
//	Vol_Spd_Array constructor
//---------------------------------------------------------

Vol_Spd_Array::Vol_Spd_Array (int max_records) : 
	Class2_Array (sizeof (Vol_Spd_Data), max_records)
{
	num_vol = 0;
}

void Vol_Spd_Array::Clear (void)
{
	Vol_Spd_Data *ptr;

	for (ptr = First (); ptr; ptr = Next ()) {
		ptr->Clear ();
	}
}
