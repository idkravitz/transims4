//*********************************************************
//	Summary_Data.cpp - emissions summary data classes
//*********************************************************

#include "Summary_Data.hpp"

//---------------------------------------------------------
//	Summary_Data constructor
//---------------------------------------------------------

Summary_Data::Summary_Data (int num) : Class_Index (0)
{
	pollutants = 0;
	emissions = NULL;
	meters = 0.0;
	seconds = 0.0;

	if (num > 0) {
		Pollutants (num);
	}
}

//---- Pollutants ----

bool Summary_Data::Pollutants (int num) 
{
	Clear ();
	if (num <= 0) return (false);

	pollutants = num;

	emissions = new double [num];

	return (emissions != NULL);
}

//---- Sum ----

void Summary_Data::Sum (Summary_Data *ptr) 
{
	meters += ptr->Meters ();
	seconds += ptr->Seconds ();

	for (int i=0; i < pollutants && emissions; i++) {
		emissions [i] += ptr->Emissions (i+1);
	}
}

//---- Zero ----

void Summary_Data::Zero (Summary_Data *ptr) 
{
	if (ptr != NULL) {
		ID (ptr->ID ());
	} else {
		ID (0);
	}
	meters = seconds = 0.0;

	for (int i=0; i < pollutants && emissions; i++) {
		emissions [i] = 0.0;
	}
}

//---------------------------------------------------------
//	Summary_Array constructor
//---------------------------------------------------------

Summary_Array::Summary_Array (int num_pollutants, int max_records) : 
	Class_Array (sizeof (Summary_Data), max_records)
{
	Pollutants (num_pollutants);
}

//---- destructor ----

Summary_Array::~Summary_Array (void) 
{
	Summary_Data *ptr;

	for (ptr = First (); ptr; ptr = Next ()) {
		ptr->Clear ();
	}
}
