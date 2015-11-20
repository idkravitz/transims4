//*********************************************************
//	Adjust_Weights.cpp - adjust plan weights
//*********************************************************

#include "AdjustPlans.hpp"

//---------------------------------------------------------
//	Adjust_Weights
//---------------------------------------------------------

bool AdjustPlans::Adjust_Weights (void)
{
	int error, count, record, time;
	double abs_error;
	bool adjust_flag = false;

	Travel_Data *travel_ptr;
	Count_Data *count_ptr;

	Show_Message ("Adjusting Weights -- Plan");
	Set_Progress (10000);

	//---- process each plan ----

	for (travel_ptr = travel_data.First (); travel_ptr; travel_ptr = travel_data.Next ()) {
		Show_Progress ();

		//---- process each count along the path ----

		error = count = 0;

		for (record = travel_ptr->First (); record != 0; record = travel_ptr->Next ()) {

			count_ptr = count_data [record];

			time = count_ptr->End_Time () - count_ptr->Start_Time ();

			//---- normalize the error to hourly values ----

			if (time != 3600) {
				error += count_ptr->Difference () * 3600 / time;
				count += count_ptr->Count () * 3600 / time;
			} else {
				error += count_ptr->Difference ();
				count += count_ptr->Count ();
			}
		}
		if (error == 0 || count == 0) continue;

		//---- calculate the error ----

		abs_error = abs (error) / (double) count;

		if (random.Probability () > abs_error) continue;

		//---- adjust the weights ----

		if (error > 0) {

			if (delete_flag) {
				if (travel_ptr->Weight () == 1) continue;
			}

			//---- delete the plan ----

			if (!travel_ptr->Delete ()) continue;
			error = -1;

		} else {

			//---- replicate the plan ----

			if (!travel_ptr->Replicate (max_copy)) continue;
			error = 1;
		}

		//---- adjust the volumes ----

		adjust_flag = true;

		for (record = travel_ptr->First (); record != 0; record = travel_ptr->Next ()) {

			count_ptr = count_data [record];

			count_ptr->Add_Volume (error);
		}
	}
	End_Progress ();

	return (adjust_flag);
}
