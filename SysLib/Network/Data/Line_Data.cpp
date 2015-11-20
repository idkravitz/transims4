//*********************************************************
//	Line_Data.cpp - network transit line data classes
//*********************************************************

#include "Line_Data.hpp"

#include "Utility.hpp"
#include <string.h>

//---------------------------------------------------------
//	Line_Data constructor
//---------------------------------------------------------

Line_Data::Line_Data (int id) : Class_Index (id)
{
	Mode (0);
	Driver (0);
	Reset ();
	Null_Name ();
	Null_Notes ();
}

int  Line_Data::s = 0;
int  Line_Data::r = 0;

void Line_Data::Clear (void)
{
	if (nstops > 0 && stop != NULL) {
		if (nruns > 0) {
			Line_Stop *stop_ptr;
			for (int i=0; i < nstops; i++) {
				stop_ptr = stop + i;
				if (stop_ptr->schedule != NULL) {
					delete [] stop_ptr->schedule;
					stop_ptr->schedule = NULL;
				}
				if (stop_ptr->penalty != NULL) {
					delete [] stop_ptr->penalty;
					stop_ptr->penalty = NULL;
				}
			}
		}
		delete [] stop;
		stop = NULL;
	}
	nruns = nstops = 0;

	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
	if (notes != NULL) {
		delete [] notes;
		notes = NULL;
	}
}

bool Line_Data::Stops (int value)
{
	if (nstops > 0) Clear ();
	if (value <= 0) return (true);

	stop = new Line_Stop [value];

	if (stop == NULL) return (false);

	nstops = value;
	memset (stop, '\0', nstops * sizeof (Line_Stop));
	return (true);
}

bool Line_Data::Runs (int value)
{
	int i;
	Line_Stop *stop_ptr;

	if (nstops == 0 || stop == NULL) return (false);

	if (nruns > 0) {
		for (i=0; i < nstops; i++) {
			stop_ptr = stop + i;
			if (stop_ptr->schedule != NULL) {
				delete [] stop_ptr->schedule;
				stop_ptr->schedule = NULL;
			}
			if (stop_ptr->penalty != NULL) {
				delete [] stop_ptr->penalty;
				stop_ptr->penalty = NULL;
			}
		}
		nruns = 0;
	}
	if (value <= 0) return (true);

	for (i=0; i < nstops; i++) {
		stop_ptr = stop + i;

		stop_ptr->schedule = new int [value];
		stop_ptr->penalty = NULL;

		if (stop_ptr->schedule == NULL) return (false);

		memset (stop_ptr->schedule, '\0', value * sizeof (int));
	}
	nruns = value;
	return (true);
}

int Line_Data::Penalty (int run, int num)
{
	if (!Index (run, num)) return (0);
	
	Line_Stop *stop_ptr = stop + s;

	if (stop_ptr->penalty == NULL) return (0);

	return (stop_ptr->penalty [r]);
}

void Line_Data::Penalty (int run, int num, int imp)
{
	if (imp == 0 || !Index (run, num)) return;
	
	Line_Stop *stop_ptr = stop + s;

	if (stop_ptr->penalty == NULL) {
		stop_ptr->penalty = new int [nruns];

		if (stop_ptr->penalty == NULL) return;

		memset (stop_ptr->penalty, '\0', nruns * sizeof (int));
	}
	stop_ptr->penalty [r] = imp; 
}

bool Line_Data::Name (char *value)
{
	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
	if (value != NULL) {
		int len = (int) strlen (value) + 1;

		name = new char [len];
		if (name == NULL) return (false);

		str_cpy (name, len, value);
	}
	return (true);
}

bool Line_Data::Notes (char *value)
{
	if (notes != NULL) {
		delete [] notes;
		notes = NULL;
	}
	if (value != NULL) {
		int len = (int) strlen (value) + 1;

		notes = new char [len];
		if (notes == NULL) return (false);

		str_cpy (notes, len, value);
	}
	return (true);
}

//---------------------------------------------------------
//	Line_Array constructor
//---------------------------------------------------------

Line_Array::Line_Array (int max_records) : 
	Class_Array (sizeof (Line_Data), max_records, false)
{
	num_runs = max_runs = veh_id = offset = route_records = schedule_records = 0;
}

Line_Array::~Line_Array (void)
{
	for (Line_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
}

int Line_Array::Vehicle_ID (int vehicles)
{
	int i, high_run;

	i = 1;

	while (vehicles > 10) {
		vehicles /= 10;
		i *= 10;
	}
	vehicles = (vehicles + 1) * i;

	//---- find the highest route number ----

	high_run = num_runs = max_runs = 0;

	for (Line_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		if (ptr->Route () > high_run) high_run = ptr->Route ();
		num_runs += ptr->Runs ();
		if (ptr->Runs () > max_runs) max_runs = ptr->Runs ();
	}
	offset = 1;
	i = max_runs;

	while (i > 0) {
		i /= 10;
		offset *= 10;
	}
	i = 1;

	while (high_run > 0) {
		high_run /= 10;
		i *= 10;
	}
	i *= offset;

	if (i > vehicles) {
		veh_id = i;
	} else {
		veh_id = vehicles;
	}
	return (veh_id);
}
