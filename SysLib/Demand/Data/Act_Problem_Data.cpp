//*********************************************************
//	Act_Problem_Data.cpp - activity problem summary class
//*********************************************************

#include "Act_Problem_Data.hpp"

#include <math.h>

//---------------------------------------------------------
//	Activity_Problem_Data constructor
//---------------------------------------------------------

Activity_Problem_Data::Activity_Problem_Data (int id) : Class_Index (id)
{
	Count (0);
	Schedule (0);
	Vehicle (0);
	Location (0);
}

void Activity_Problem_Data::Add_Problem (int _count, int _schedule, int _vehicle, int _location)
{
	count += _count;
	schedule += _schedule;
	vehicle += _vehicle;
	location += _location;
}

//---------------------------------------------------------
//	Activity_Problem_Array constructor
//---------------------------------------------------------

Activity_Problem_Array::Activity_Problem_Array (int max_records) : 
	Class_Array (sizeof (Activity_Problem_Data), max_records), Static_Service ()
{
	keys = 0;
	title = key1 = key2 = NULL;
}

//---- destructor ----

Activity_Problem_Array::~Activity_Problem_Array (void)
{
	if (title != NULL) {
		delete [] title;
	}
	if (key1 != NULL) {
		delete [] key1;
	}
	if (key2 != NULL) {
		delete [] key2;
	}
}

//---- Add Problem ----

bool Activity_Problem_Array::Add_Problem (int group, int count, int schedule, int vehicle, int location)
{
	Activity_Problem_Data *data_ptr;

	data_ptr = Get (group);

	if (data_ptr == NULL) {
		data_ptr = New_Record (true);
		if (data_ptr == NULL) return (false);

		data_ptr->Group (group);
		if (!Add ()) return (false);
	}
	data_ptr->Add_Problem (count, schedule, vehicle, location);
	return (true);
}

//---- Report ----

void Activity_Problem_Array::Report (int number, char *_title, char *_key1, char *_key2)
{
	int size, problems;
	double prob;

	Activity_Problem_Data *data_ptr, total;

	//---- set header values ----

	keys = 0;
	if (_title != NULL) {
		size = (int) strlen (_title) + 1;
		title = new char [size];
		str_cpy (title, size, _title);

		exe->Show_Message (title);
	}
	if (_key1 != NULL) {
		size = (int) strlen (_key1) + 1;
		key1 = new char [size];
		str_cpy (key1, size, _key1);
		keys++;
	}
	if (_key2 != NULL) {
		size = (int) strlen (_key2) + 1;
		key2 = new char [size];
		str_cpy (key2, size, _key2);
		keys++;
	}

	//---- print the report ----

	exe->Header_Number (number);

	if (!exe->Break_Check (Num_Records () + 7)) {
		exe->Print (1);
		Header ();
	}

	for (data_ptr = First_Key (); data_ptr; data_ptr = Next_Key ()) {
		if (data_ptr->Count () == 0) continue;

		if (keys == 2) {
			exe->Print (1, "%3d-%-3d", (data_ptr->Group () >> 16), (data_ptr->Group () & 0x00FF));
		} else {
			exe->Print (1, "%5d  ", data_ptr->Group ());
		}

		prob = problems = data_ptr->Schedule () + data_ptr->Vehicle () + data_ptr->Location ();
		if (prob < 1.0) prob = 1.0;
		
		exe->Print (0, " %9d   %7d %5.1lf  %7d %5.1lf  %7d %5.1lf  %7d %5.1lf", 
			data_ptr->Count (), problems, 100.0 * problems / data_ptr->Count (),
			data_ptr->Schedule (), 100.0 * data_ptr->Schedule () / prob,
			data_ptr->Vehicle (), 100.0 * data_ptr->Vehicle () / prob,
			data_ptr->Location (), 100.0 * data_ptr->Location () / prob);

		total.Add_Problem (data_ptr->Count (), data_ptr->Schedule (), 
			data_ptr->Vehicle (), data_ptr->Location ());
	}
	prob = problems = total.Schedule () + total.Vehicle () + total.Location ();
	if (prob < 1.0) prob = 1.0;	

	exe->Print (2, "Total   %9d   %7d %5.1lf  %7d %5.1lf  %7d %5.1lf  %7d %5.1lf", 
		total.Count (), problems, 100.0 * problems / total.Count (),
		total.Schedule (), 100.0 * total.Schedule () / prob,
		total.Vehicle (), 100.0 * total.Vehicle () / prob,
		total.Location (), 100.0 * total.Location () / prob);

	exe->Header_Number (0);
}

//---- Header ----

void Activity_Problem_Array::Header (void)
{
	if (title != NULL) {
		exe->Print (1, title);
	} else {
		exe->Print (1, "Activity Problem Summary Report");
	}
	if (keys < 2 || key1 == NULL) {
		exe->Print (2, "%12c", BLANK);
	} else {
		exe->Print (2, "%-7.7s%5c", key1, BLANK);
	}
	exe->Print (0, "Total     Total       ------------- Problem Types ----------------");

	if (keys == 2 && key2 != NULL) {
		exe->Print (1, "%-7.7s", key2);
	} else if (keys == 1 && key1 != NULL) {
		exe->Print (1, "%-7.7s", key1);
	} else {
		exe->Print (1, "Group  ");
	}
	exe->Print (0, "     Trips  Problems   %%   Schedule   %%    Vehicle   %%   Location   %%");
	exe->Print (1);
}

/*********************************************|***********************************************

	[title]

	[key1]      Total     Total       ------------- Problem Types ---------------- 
	[key2]      Trips  Problems   %   Schedule   %    Vehicle   %   Location   %

	ddd-ddd ddddddddd   ddddddd fff.f  ddddddd fff.f  ddddddd fff.f  ddddddd fff.f
 
	Total   ddddddddd   ddddddd fff.f  ddddddd fff.f  ddddddd fff.f  ddddddd fff.f

**********************************************|***********************************************/ 
