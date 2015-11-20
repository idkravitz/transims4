//*********************************************************
//	Time_Process.cpp - Trip Time Processing
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Time_Processing
//---------------------------------------------------------

void ConvertTrips::Time_Processing (File_Group *group)
{
	int i, j, t, t1, start, end, num, num_rec, num_shares, temp;
	char *str_ptr;
	double share, total;
	bool share_flag;

	Range_Data range;
	Time_Step time_step (1, "HOURS");

	Diurnal_File *file;
	Diurnal_Target *diurnal_ptr;

	//---- read the trip time into memory ----

	diurnal_data.Clear ();
	file = group->Time_File ();

	num_shares = group->Num_Shares ();
	share_flag = (num_shares > 0);
	if (!share_flag) num_shares = 1;

	Show_Message (0, "\tReading %s -- Record", file->File_Type ());
	Set_Progress ();
	num = 0;

	while (file->Read ()) {
		Show_Progress ();

		str_ptr = file->Start_Time ();
		if (*str_ptr == '\0') continue;

		start = time_step.Step (str_ptr);

		end = time_step.Step (file->End_Time ());

		if (start == 0 && end == 0) continue;
		if (start >= end) {
			Error ("Time Range is Inappropriate (%d >= %d)", start, end);
		}
		share = (end - start) / 60.0;
		if (share < 1.0) share = 1.0;
		num++;

		for (t = start; t < end; t = t1) {
			t1 = t + 60;

			range.Low (t);
			range.High (t1);

			if (!group->In_Range (&range)) continue;

			diurnal_ptr = diurnal_data.New_Record (true);

			diurnal_ptr->Start_Time (t);
			diurnal_ptr->End_Time (t1);

			//---- get the share data ----

			if (!diurnal_ptr->Num_Shares (num_shares + 1)) goto diurnal_error;

			if (share_flag) {
				for (i=1; i <= num_shares; i++) {
					diurnal_ptr->Share (i, group->Share (i) / share);
				}
			} else {
				diurnal_ptr->Share (1, file->Share () / share);
			}
			if (!diurnal_data.Add ()) goto diurnal_error;
		}
	}
	End_Progress ();

	file->Close ();

	//---- smooth, normalize and convert to a cumulative distribution ----

	temp = num_shares + 1;
	num_rec = diurnal_data.Num_Records ();
	num = num_rec / num;
	num = (num + 1) / 2;
	if (num > 100) num = 100;
	num *= num;

	for (i=1; i <= num_shares; i++) {

		//---- smooth the distribution ----

		for (j=0; j < num; j++) {
			for (diurnal_ptr = diurnal_data.First (); diurnal_ptr; diurnal_ptr = diurnal_data.Next ()) {
				diurnal_ptr->Share (temp, diurnal_ptr->Share (i));
				diurnal_ptr->Share (i, 0.0);
			}
			for (t = 1; t <= num_rec; t++) {
				diurnal_ptr = diurnal_data [t];

				total = diurnal_ptr->Share (temp);

				share = total * 0.2;

				diurnal_ptr->Add_Share (i, total - 2.0 * share);

				t1 = t - 1;
				if (t1 < 1) t1 = 1;

				diurnal_ptr = diurnal_data [t1];
				diurnal_ptr->Add_Share (i, share);

				t1 = t + 1;
				if (t1 > num_rec) t1 = num_rec;

				diurnal_ptr = diurnal_data [t1];
				diurnal_ptr->Add_Share (i, share);
			}
		}
		total = 0.0;

		for (diurnal_ptr = diurnal_data.First (); diurnal_ptr != NULL; diurnal_ptr = diurnal_data.Next ()) {
			total += diurnal_ptr->Share (i);
		}
		if (total == 0.0) {
			Error ("Total Diurnal Distribution is Zero");
		}
		share = 1.0 / total;
		total = 0.0;

		for (diurnal_ptr = diurnal_data.First (); diurnal_ptr != NULL; diurnal_ptr = diurnal_data.Next ()) {
			total = diurnal_ptr->Share (i) * share;

			diurnal_ptr->Share (i, total);
			diurnal_ptr->Target (i, 0.0);
			diurnal_ptr->Trips (i, 0);
		}
	}
	return;

diurnal_error:
	Error ("Adding Time Point to Diurnal Distribution");
}
