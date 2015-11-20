//*********************************************************
//	Set_Time.cpp - set the trip time
//*********************************************************

#include "ConvertTours.hpp"

#include "math.h"

//---------------------------------------------------------
//	Set_Time
//---------------------------------------------------------

bool ConvertTours::Set_Time (File_Group *group, int &start, int &end, int ttim, int num)
{
	int t, time, time1;
	double prob, share, share1, total;
	int retry_count;

	Diurnal_Target *diurnal_ptr;
	Diurnal_Target_Array *diurnal_data;

	if (start > end) {
		Warning ("Diurnal Period %d..%d is Out of Range", start, end);
		return (false);
	}

	//---- distribute the trips to the time targets ----

	diurnal_data = group->Diurnal_Data ();

	if (num == 0) num = group->Time_Field ();
	if (num == 0) num = 3;

	total = 0.0;

	for (t=start; t <= end; t++) {
		diurnal_ptr = diurnal_data->Record (t);
		if (diurnal_ptr == NULL) {
			Warning ("Diurnal Period %d was Not Found", t);
			return (false);
		}
		total += diurnal_ptr->Share (num);
	}
	if (total == 0.0) return (false);

	for (t=start; t <= end; t++) {
		diurnal_ptr = diurnal_data->Record (t);
		share = diurnal_ptr->Share (num) / total;
		diurnal_ptr->Add_Target (num, share);
	}

	retry_count = 0;

retry:
	prob = random_tod.Probability ();
	
	time = time1 = 0;
	share = share1 = 0.0;

	for (t=start; t <= end; t++) {
		diurnal_ptr = diurnal_data->Record (t);

		share1 = share + diurnal_ptr->Share (num) / total;

		if (prob < share1) {
			if (diurnal_ptr->Target (num) < diurnal_ptr->Trips (num) + 0.5) {
				if (retry_count < 10) {
					retry_count++;
					goto retry;
				}
			}
			time = diurnal_ptr->Start_Time ();
			time1 = diurnal_ptr->End_Time ();
			diurnal_ptr->Add_Trips (num);
			break;
		}
		share = share1;
	}
	if (share != share1) {
		time += (int) ((time1 - time) * (prob - share) / (share1 - share) + 0.5);
	}

	//---- time methods ----

	if (group->Method () == 0) {
		start = time;
	} else if (group->Method () == 1) {
		start = time - ttim;
		if (start < 0) start = 1;
	} else {
		start = time - ttim / 2;
		if (start < 0) start = 1;
	}
	end = start + ttim;

	return (true);
}
