//*********************************************************
//	Activity.cpp - convert the activity file
//*********************************************************

#include "NewFormat.hpp"

#include "Activity_File.hpp"
#include "Data_Array.hpp"
#include "System_Defines.hpp"

//---------------------------------------------------------
//	Activity
//---------------------------------------------------------

void NewFormat::Activity (void)
{
	int nact, nhhold, nperson, ntour, ntrip, hhold, person, lvalue, num, duration;
	int min_time, max_time;
	char time_buffer [24], *str_ptr;
	bool home_flag;

	Activity_File *new_file;
	Person_Number *person_ptr;

	Time_Step hours (1, "HOURS");

	new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

	//---- read each record ----

	nact = nhhold = nperson = ntour = ntrip = 0;
	hhold = person = num = 0;
	home_flag = true;

	Show_Message ("Reading %s -- Record", activity_file.File_Type ());
	Set_Progress ();

	while (activity_file.Read ()) {
		Show_Progress ();

		//---- copy general fields ----

		new_file->Copy_Fields (&activity_file);

		//---- household ----

		lvalue = activity_file.Household ();

		if (lvalue != hhold) {
			nhhold++;
			hhold = lvalue;
			person = 0;
			num = 1;
			home_flag = true;
		}
		new_file->Household (lvalue);

		//---- person ----

		lvalue = activity_file.Person ();

		person_ptr = (Person_Number *) person_number.Get (&lvalue);

		if (person_ptr == NULL) {
			Write (1, "Warning: Person ID %d was Not Found in Population File", lvalue);
			continue;
		}
		lvalue = person_ptr->person_num;

		if (lvalue != person) {
			nperson++;
			person = lvalue;
			num = 1;
			home_flag = true;
		}
		new_file->Person (lvalue);

		//---- activity ----

		new_file->Activity (num);
		num++;

		//---- purpose ----

		lvalue = activity_file.Purpose ();

		if (lvalue == 0) {
			if (!home_flag) ntour++;
			home_flag = true;
		} else {
			home_flag = false;
			ntrip++;
		}
		new_file->Purpose (lvalue);

		//---- start time ----

		str_ptr = activity_file.Start_Min ();

		if (str_ptr != NULL) {
			min_time = hours.Step (str_ptr);
		} else {
			min_time = 0;
		}
		str_ptr = activity_file.Start_Max ();

		if (str_ptr != NULL) {
			max_time = hours.Step (str_ptr);
		} else {
			max_time = min_time;
		}

		//---- save the average if not version3 ----

		if (new_file->Dbase_Format () != VERSION3) {
			min_time = max_time = (min_time + max_time) / 2;
		}
		new_file->Start_Min (activity_time.Format_Step (min_time, time_buffer));
		new_file->Start_Max (activity_time.Format_Step (max_time, time_buffer));

		duration = max_time;

		//---- end time ----

		str_ptr = activity_file.End_Min ();

		if (str_ptr != NULL) {
			min_time = hours.Step (str_ptr);
		} else {
			min_time = max_time;
		}
		str_ptr = activity_file.End_Max ();

		if (str_ptr != NULL) {
			max_time = hours.Step (str_ptr);
		} else {
			max_time = min_time;
		}

		//---- save the average if not version3 ----

		if (new_file->Dbase_Format () != VERSION3) {
			min_time = max_time = (min_time + max_time) / 2;
		}
		new_file->End_Min (activity_time.Format_Step (min_time, time_buffer));
		new_file->End_Max (activity_time.Format_Step (max_time, time_buffer));

		//---- duration ----

		str_ptr = activity_file.Time_Min ();

		if (str_ptr != NULL) {
			min_time = hours.Step (str_ptr);
		} else {
			min_time = max_time - duration;
		}
		str_ptr = activity_file.Time_Max ();

		if (str_ptr != NULL) {
			max_time = hours.Step (str_ptr);
		} else {
			max_time = min_time;
		}

		//---- save the average if not version3 ----

		if (new_file->Dbase_Format () != VERSION3) {
			min_time = max_time = (min_time + max_time) / 2;
		}
		new_file->Time_Min (activity_time.Format_Step (min_time, time_buffer));
		new_file->Time_Max (activity_time.Format_Step (max_time, time_buffer));

		//---- transfer other named variables ----

		new_file->Priority (activity_file.Priority ());
		new_file->Mode (activity_file.Mode ());
		new_file->Vehicle (activity_file.Vehicle ());
		new_file->Location (activity_file.Location ());
		new_file->Passengers (activity_file.Passengers ());
		new_file->Constraint (activity_file.Constraint ());

		//---- save the activity record ----

		if (!new_file->Write ()) {
			Error ("Writing Activity Record");
		}
		nact++;
	}
	End_Progress ();
	activity_file.Close ();

	Print (2, "Number of %s Records = %d", activity_file.File_Type (), Progress_Count ());

	new_file->Close ();

	Print (1);
	Write (1, "Number of %s Records = %d", new_file->File_Type (), nact);
	Write (1, "\tNumber of Households = %d", nhhold);
	Write (1, "\tNumber of Persons = %d", nperson);
	Write (1, "\tNumber of Tours = %d", ntour);
	Write (1, "\tNumber of Trips = %d", ntrip);
	Show_Message (1);
}
