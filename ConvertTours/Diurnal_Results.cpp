//*********************************************************
//	Diurnal_Results.cpp - Output the Diurnal Allocations
//*********************************************************

#include "ConvertTours.hpp"

//---------------------------------------------------------
//	Diurnal_Results
//---------------------------------------------------------

void ConvertTours::Diurnal_Results (void)
{
	int i, j, num, num_rec;

	FILE *file;
	Diurnal_Target *diurnal_ptr;
	Diurnal_Target_Array *diurnal_data;
	File_Group *group;

	//---- create the file header ----

	file = diurnal_file.File ();

	fprintf (file, "MINUTE");
	num_rec = 0;

	for (group = file_group.First (); group != NULL; group = file_group.Next ()) {
		diurnal_data = group->Diurnal_Data ();
		if (diurnal_data == NULL) continue;
		if (num_rec < diurnal_data->Num_Records ()) num_rec = diurnal_data->Num_Records ();

		diurnal_ptr = diurnal_data->First ();
		if (diurnal_ptr == NULL) continue;
		num = diurnal_ptr->Num_Shares () - 1;
		if (num < 3) continue;

		for (i=3; i <= num; i++) {
			fprintf (file, "\tSHARE%d_%d", group->Group (), i);
			fprintf (file, "\tTARGET%d_%d", group->Group (), i);
			fprintf (file, "\tTRIPS%d_%d", group->Group (), i);
		}
	}
	if (num_rec == 0) return;
	if (num_rec > 24*60) num_rec = 24*60;

	//---- print the data ----

	for (j=1; j <= num_rec; j++) {
		fprintf (file, "\n%d", j);
	
		for (group = file_group.First (); group != NULL; group = file_group.Next ()) {
			diurnal_data = group->Diurnal_Data ();
			if (diurnal_data == NULL) continue;

			diurnal_ptr = diurnal_data->Record (j);
			if (diurnal_ptr == NULL) continue;
			num = diurnal_ptr->Num_Shares () - 1;
			if (num < 3) continue;

			for (i=3; i <= num; i++) {
				fprintf (file, "\t%lf\t%.2lf\t%d", diurnal_ptr->Share (i), diurnal_ptr->Target (i), diurnal_ptr->Trips (i));
			}
		}
	}
	diurnal_file.Close ();
}
