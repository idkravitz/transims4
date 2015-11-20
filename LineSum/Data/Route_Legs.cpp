//*********************************************************
//	Route_Legs.cpp - transit leg distance and subarea flags
//*********************************************************

#include "Route_Legs.hpp"

//---------------------------------------------------------
//	Route_Legs constructor
//---------------------------------------------------------

Route_Legs::Route_Legs (void)
{
	subarea_flag = false;
	num_legs = 0;
	percent = NULL;
	distance = NULL;
}

//---- Num_Legs ----

bool Route_Legs::Num_Legs (int num) 
{
	Clear ();

	if (num > 0) {
		percent = new int [num];
		distance = new int [num];

		if (percent == NULL || distance == NULL) return (false);
		num_legs = num;

		memset (percent, '\0', num * sizeof (int));
		memset (distance, '\0', num * sizeof (int));
	}
	return (true);
}


//---- Clear ----

void Route_Legs::Clear (void)
{
	if (percent != NULL) {
		delete [] percent;
		percent = NULL;
	}
	if (distance != NULL) {
		delete [] distance;
		distance = NULL;
	}
	num_legs = 0;
}

//---------------------------------------------------------
//	Route_Legs_Array constructor
//---------------------------------------------------------

Route_Legs_Array::Route_Legs_Array (int max_records) : 
	Data_Array (sizeof (Route_Legs), max_records)
{
}

Route_Legs_Array::~Route_Legs_Array (void)
{
	for (Route_Legs *leg = First (); leg != NULL; leg = Next ()) {
		leg->Clear ();
	}
}
