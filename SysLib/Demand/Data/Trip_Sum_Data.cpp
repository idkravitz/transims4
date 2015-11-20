//*********************************************************
//	Trip_Sum_Data.cpp - trip summary data class
//*********************************************************

#include "Trip_Sum_Data.hpp"

#include <math.h>

//---------------------------------------------------------
//	Trip_Sum_Data constructor
//---------------------------------------------------------

Trip_Sum_Data::Trip_Sum_Data (int id) : Class_Index (id)
{
	Count (0);
	Distance (0.0);
	Time (0.0);
	Speed (0.0);
	Turns (0.0);
	Distance_Sq (0.0);
	Time_Sq (0.0);
	Speed_Sq (0.0);
	Turns_Sq (0.0);
	Min_Distance (0.0);
	Max_Distance (0.0);
	Min_Time (0.0);
	Max_Time (0.0);
	Min_Speed (0.0);
	Max_Speed (0.0);
	Min_Turns (0);
	Max_Turns (0);
}

void Trip_Sum_Data::Add_Trip (double _distance, double _time, int _turns, int _count)
{
	if (_count < 1) return;

	double _speed;
	if (_time > 0) {
		_speed = _distance / _time;
		if (_speed > 100) return;
	} else {
		_speed = _distance;
		if (_speed > 50) return;
	}
	
	if (count == 0) {
		min_distance = max_distance = _distance;
		distance = _distance * _count;
		distance2 = _distance * _distance * _count;

		min_time = max_time = _time;
		time = _time * _count;
		time2 = _time * _time * _count;

		min_speed = max_speed = _speed;
		speed = _speed * _count;
		speed2 = _speed * _speed * _count;

		min_turns = max_turns = _turns;
		turns = _turns * _count;
		turns2 = turns * turns * _count;
		count = _count;
	} else {
		if (_distance < min_distance) min_distance = _distance;
		if (_distance > max_distance) max_distance = _distance;

		distance += _distance * _count;
		distance2 += _distance * _distance * _count;

		if (_time < min_time) min_time = _time;
		if (_time > max_time) max_time = _time;

		time += _time * _count;
		time2 += _time * _time * _count;

		if (_speed < min_speed) min_speed = _speed;
		if (_speed > max_speed) max_speed = _speed;

		speed += _speed * _count;
		speed2 += _speed * _speed * _count;

		if (_turns < min_turns) min_turns = _turns;
		if (_turns > max_turns) max_turns = _turns;

		turns += _turns * _count;
		turns2 += (double) _turns * _turns * _count;
		count += _count;
	}
}

double Trip_Sum_Data::StdDev_Distance (void)
{
	if (count < 2) return (0.0);

	return (sqrt ((distance2 - (distance * distance / count)) / (count - 1)));
}

double Trip_Sum_Data::StdDev_Time (void)
{
	if (count < 2) return (0.0);

	return (sqrt ((time2 - (time * time / count)) / (count - 1)));
}

double Trip_Sum_Data::StdDev_Speed (void)
{
	if (count < 2) return (0.0);

	return (sqrt ((speed2 - (speed * speed / count)) / (count - 1)));
}

double Trip_Sum_Data::StdDev_Turns (void)
{
	if (count < 2) return (0.0);

	return (sqrt ((turns2 - (turns * turns / count)) / (count - 1)));
}

//---------------------------------------------------------
//	Trip_Sum_Array constructor
//---------------------------------------------------------

Trip_Sum_Array::Trip_Sum_Array (int max_records) : 
	Class_Array (sizeof (Trip_Sum_Data), max_records), Static_Service ()
{
	keys = 0;
	title = key1 = key2 = NULL;
}

//---- destructor ----

Trip_Sum_Array::~Trip_Sum_Array (void)
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

//---- Add Trip ----

bool Trip_Sum_Array::Add_Trip (int group, double distance, double time, int turns, int count)
{
	Trip_Sum_Data *data_ptr;

	data_ptr = Get (group);

	if (data_ptr == NULL) {
		data_ptr = New_Record (true);
		if (data_ptr == NULL) return (false);

		data_ptr->Group (group);
		if (!Add ()) return (false);
	}
	data_ptr->Add_Trip (distance, time, turns, count);
	return (true);
}

//---- Report ----

void Trip_Sum_Array::Report (int number, char *_title, char *_key1, char *_key2)
{
	int size;
	Trip_Sum_Data *data_ptr, total;

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

		exe->Print (0, " %9d %8d %8d %8d %8d %8.2lf %8.2lf %8.2lf %8.2lf", 
			data_ptr->Count (), (int) (data_ptr->Min_Distance () + 0.5),
			(int) (data_ptr->Max_Distance () + 0.5), (int) (data_ptr->Average_Distance () + 0.5),
			(int) (data_ptr->StdDev_Distance () + 0.5), data_ptr->Min_Time () / 60.0,
			data_ptr->Max_Time () / 60.0, data_ptr->Average_Time () / 60.0, 
			data_ptr->StdDev_Time () / 60.0);

		if (total.Count () == 0) {
			total.Count (data_ptr->Count ());
			total.Distance (data_ptr->Distance ());
			total.Time (data_ptr->Time ());
			total.Distance_Sq (data_ptr->Distance_Sq ());
			total.Time_Sq (data_ptr->Time_Sq ());

			total.Min_Distance (data_ptr->Min_Distance ());
			total.Max_Distance (data_ptr->Max_Distance ());

			total.Min_Time (data_ptr->Min_Time ());
			total.Max_Time (data_ptr->Max_Time ());
		} else {
			total.Count (total.Count () + data_ptr->Count ());
			total.Distance (total.Distance () + data_ptr->Distance ());
			total.Time (total.Time () + data_ptr->Time ());
			total.Distance_Sq (total.Distance_Sq () + data_ptr->Distance_Sq ());
			total.Time_Sq (total.Time_Sq () + data_ptr->Time_Sq ());

			if (total.Min_Distance () > data_ptr->Min_Distance ()) total.Min_Distance (data_ptr->Min_Distance ());
			if (total.Max_Distance () < data_ptr->Max_Distance ()) total.Max_Distance (data_ptr->Max_Distance ());

			if (total.Min_Time () > data_ptr->Min_Time ()) total.Min_Time (data_ptr->Min_Time ());
			if (total.Max_Time () < data_ptr->Max_Time ()) total.Max_Time (data_ptr->Max_Time ());
		}
	}
	exe->Print (2, "Total   %9d %8d %8d %8d %8d %8.2lf %8.2lf %8.2lf %8.2lf", 
		total.Count (), (int) (total.Min_Distance () + 0.5), (int) (total.Max_Distance () + 0.5), 
		(int) (total.Average_Distance () + 0.5), (int) (total.StdDev_Distance () + 0.5), 
		total.Min_Time () / 60.0, total.Max_Time () / 60.0, total.Average_Time () / 60.0,
		total.StdDev_Time () / 60.0);

	exe->Header_Number (0);
}

//---- Header ----

void Trip_Sum_Array::Header (void)
{
	if (title != NULL) {
		exe->Print (1, title);
	} else {
		exe->Print (1, "Trip Length Summary Report");
	}
	if (keys < 2 || key1 == NULL) {
		exe->Print (2, "%19c", BLANK);
	} else {
		exe->Print (2, "%-7.7s%12c", key1, BLANK);
	}
	exe->Print (0, "------- Distance (meters) --------  --------- Time (minutes) ---------");

	if (keys == 2 && key2 != NULL) {
		exe->Print (1, "%-7.7s", key2);
	} else if (keys == 1 && key1 != NULL) {
		exe->Print (1, "%-7.7s", key1);
	} else {
		exe->Print (1, "Group  ");
	}
	exe->Print (0, "     Trips  Minimum  Maximum  Average   StdDev  Minimum  Maximum  Average   StdDev");
	exe->Print (1);
}

/*********************************************|***********************************************

	[title]

	[key1]             ------- Distance (meters) --------  --------- Time (minutes) ---------
	[key2]      Trips  Minimum  Maximum  Average   StdDev  Minimum  Maximum  Average   StdDev 

	ddd-ddd ddddddddd  ddddddd  ddddddd  ddddddd  ddddddd  ffff.ff  ffff.ff  ffff.ff  ffff.ff
 
	Total   ddddddddd  ddddddd  ddddddd  ddddddd  ddddddd  ffff.ff  ffff.ff  ffff.ff  ffff.ff

**********************************************|***********************************************/ 
