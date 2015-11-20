//*********************************************************
//	Select_Set.cpp - transit route selection set
//*********************************************************

#include "Select_Set.hpp"

#include "Network_Service.hpp"
#include "Utility.hpp"
#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Select_Set constructor
//---------------------------------------------------------

Select_Set::Select_Set (void) : Static_Service ()
{
	Reset ();
}

//---- Select_Label ----

bool Select_Set::Select_Label (char *text)
{
	if (text == NULL) return (false);

	int size = (int) strlen (text) + 1;

	select_label = new char [size];
	if (select_label == NULL) return (false);

	str_cpy (select_label, size, text);
	return (true);
}

//---- Time_Periods ----

bool Select_Set::Time_Periods (char *range, char *inc_ptr)
{
	int increment;

	time_periods = new Time_Range (1, "24_HOUR_CLOCK");
	if (time_periods == NULL) return (false);

	if (inc_ptr != NULL) {
		increment = atol (inc_ptr);

		if (increment < 0 || increment > 240) {
			exe->Error ("Summary Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	} else {
		increment = 0;
	}
	if (increment != 0) {
		time_periods->Period_Flag (true);
	}
	time_periods->Increment (increment);

	if (!time_periods->Add_Ranges (range)) {
		exe->File_Error ("Time Period Range", range);
	}
	if (increment > 0) {
		exe->Print (1, "Summary Time Increment = %d minutes", (increment / 60));
	} else {
		exe->Print (1, "One Summary Increment will be Output");
	}
	return (true);
}

//---- Select_Modes ----

bool Select_Set::Select_Modes (char *string)
{
	if (string == NULL) return (false);

	Network_Service *net = (Network_Service *) exe;

	char buffer [FIELD_BUFFER];
	int size = net->REGIONRAIL + 1;

	select_modes = new bool [size];
	if (select_modes == NULL) return (false);

	memset (select_modes, '\0', size * sizeof (bool));

	while (string != NULL) {
		string = Get_Token (string, buffer);

		if (buffer [0] == '\0') break;

		select_modes [net->Transit_Code (buffer)] = true;
	}
	return (true);
}

//---- Select_Routes ----

bool Select_Set::Select_Routes (char *string)
{
	if (string == NULL) return (false);

	select_routes = new Data_Range ();
	if (select_routes == NULL) return (false);

	if (!select_routes->Add_Ranges (string)) {
		exe->File_Error ("Transit Route Range", string);
	}
	return (true);
}

//---- Select_Subarea ----

bool Select_Set::Select_Subarea (char *filename)
{
	if (filename == NULL) return (false);

	select_subarea = new Arcview_File ();
	if (select_subarea == NULL) return (false);

	route_legs = new Route_Legs_Array ();
	if (route_legs == NULL) return (false);

	select_subarea->File_Type ("Select Subarea Polygon");

	select_subarea->Open (exe->Project_Filename (filename));
	
	if (!select_subarea->Read_Record ()) {
		exe->Error ("Reading %s", select_subarea->File_Type ());
	}
	return (true);
}

//---- In_Subarea ----

bool Select_Set::In_Subarea (double x, double y)
{
	if (select_subarea) {
		return (In_Polygon (x, y, &select_subarea->points));
	} else {
		return (true);
	}
}

//---- Copy ----

void Select_Set::Copy (Select_Set *set_ptr)
{
	number = set_ptr->Number ();
	time_method = set_ptr->Time_Method ();
	select_label = set_ptr->Select_Label ();
	time_periods = set_ptr->Time_Periods ();
	select_modes = set_ptr->Select_Modes ();
	select_routes = set_ptr->Select_Routes ();
	select_subarea = set_ptr->Select_Subarea ();
	route_legs = set_ptr->Get_Route_Legs_Array ();
}

//---- Replace ----

void Select_Set::Replace (Select_Set *set_ptr)
{
	number = set_ptr->Number ();
	if (set_ptr->Time_Method () != 0) time_method = set_ptr->Time_Method ();
	if (set_ptr->Select_Label () != NULL) select_label = set_ptr->Select_Label ();
	if (set_ptr->Time_Periods () != NULL) time_periods = set_ptr->Time_Periods ();
	if (set_ptr->Select_Modes () != NULL) select_modes = set_ptr->Select_Modes ();
	if (set_ptr->Select_Routes () != NULL) select_routes = set_ptr->Select_Routes ();
	if (set_ptr->Select_Subarea () != NULL) select_subarea = set_ptr->Select_Subarea ();
	if (set_ptr->Get_Route_Legs_Array () != NULL) route_legs = set_ptr->Get_Route_Legs_Array ();
}

//---- Reset ----

void Select_Set::Reset (void)
{
	number = 0;
	time_method = 0;
	select_label = NULL;
	time_periods = NULL;
	select_modes = NULL;
	select_routes = NULL;
	select_subarea = NULL;
	route_legs = NULL;
}

//---- Clear ----

void Select_Set::Clear (void)
{
	if (select_label != NULL) {
		delete [] select_label;
		select_label = NULL;
	}
	if (time_periods != NULL) {
		delete time_periods;
		time_periods = NULL;
	}
	if (select_modes != NULL) {
		delete [] select_modes;
		select_modes = NULL;
	}
	if (select_routes != NULL) {
		delete select_routes;
		select_routes = NULL;
	}
	if (select_subarea != NULL) {
		delete select_subarea;
		select_subarea = NULL;
	}
	if (route_legs != NULL) {
		delete route_legs;
		route_legs = NULL;
	}
}

//---------------------------------------------------------
//	Select_Set_Array constructor
//---------------------------------------------------------

Select_Set_Array::Select_Set_Array (int max_records) : 
	Data_Array (sizeof (Select_Set), max_records)
{
	current_set = new Select_Set ();
}

Select_Set_Array::~Select_Set_Array (void)
{
	for (Select_Set *set = First (); set != NULL; set = Next ()) {
		set->Clear ();
	}
	if (current_set != NULL) {
		current_set->Reset ();
		delete current_set;
		current_set = NULL;
	}
}

//---- retrieve select set ----
	
Select_Set * Select_Set_Array::Select_Record (int index)
{
	Select_Set *set_ptr;

	current_set->Reset ();

	for (set_ptr = First (); set_ptr; set_ptr = Next ()) {
		if (set_ptr->Number () == 0) {
			current_set->Copy (set_ptr);
		} else if (set_ptr->Number () == index) {
			current_set->Replace (set_ptr);
			break;
		}
	}
	return (current_set);
}
