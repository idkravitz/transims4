//*********************************************************
//	Rider_Data.cpp - transit ridership data class
//*********************************************************

#include "Rider_Data.hpp"

#include <string.h>

//---------------------------------------------------------
//	Rider_Data constructor
//---------------------------------------------------------

Rider_Data::Rider_Data (void) : Line_Data (0)
{
	on_rider = NULL;
	rider = NULL;
}

void Rider_Data::Clear (void)
{
	if (Runs () > 0 && rider != NULL) {
		Run_Rider *rider_ptr;
		for (int i=0; i < Runs (); i++) {
			rider_ptr = rider + i;
			if (rider_ptr->stop != NULL) {
				delete [] rider_ptr->stop;
				rider_ptr->stop = NULL;
			}
		}
		delete [] rider;
		rider = NULL;
	}
	if (Stops () > 0 && on_rider != NULL) {
		delete [] on_rider;
		on_rider = NULL;
	}
	Line_Data::Clear ();
}

bool Rider_Data::Check (void)
{
	if (Stops () == 0 || Runs () == 0) return (false);
	if (rider != NULL) return (true);

	int i;
	Run_Rider *rider_ptr;

	rider = new Run_Rider [Runs ()];
	on_rider = new int [Stops ()];

	if (rider == NULL || on_rider == NULL) {
		Clear ();
		return (false);
	}
	memset (rider, '\0', Runs () * sizeof (Run_Rider));
	memset (on_rider, '\0', Stops () * sizeof (int));

	for (i=0; i < Runs (); i++) {
		rider_ptr = rider + i;

		rider_ptr->stop = new Run_Stop [Stops ()];

		if (rider_ptr->stop == NULL) {
			Clear ();
			return (false);
		}
		memset (rider_ptr->stop, '\0', Stops () * sizeof (Run_Stop));
	}
	return (true);
}

void Rider_Data::Summarize_Riders (void)
{
	if (Stops () == 0 || Runs () == 0 || rider == NULL) return;

	int i, j, riders;
	Run_Rider *rider_ptr;
	Run_Stop *stop_ptr;

	for (i=0; i < Runs (); i++) {
		rider_ptr = rider + i;
		rider_ptr->load = riders = 0;

		for (j=0; j < Stops (); j++) {
			stop_ptr = rider_ptr->stop + j;

			riders += stop_ptr->on - stop_ptr->off;
			stop_ptr->rider = riders;

			if (riders > rider_ptr->load) rider_ptr->load = riders;
		}
	}
	return;
}

//---------------------------------------------------------
//	Rider_Array constructor
//---------------------------------------------------------

Rider_Array::Rider_Array (int max_records) : 
	Line_Array (max_records)
{
	Data_Size (sizeof (Rider_Data));
}

Rider_Array::~Rider_Array (void)
{
	for (Line_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
}

void Rider_Array::Summarize_Riders (void)
{
	for (Rider_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Summarize_Riders ();
	}
}
