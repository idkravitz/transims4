//*********************************************************
//	Write_Output.cpp - write the output data
//*********************************************************

#include "SmoothData.hpp"

//---------------------------------------------------------
//	Write_Output
//---------------------------------------------------------

void SmoothData::Write_Output (void)
{
	int m, i, j, i1, i2, nrec, num;
	double factor, forward_total, backward_total, forward, backward, share;

	Diurnal_Data *diurnal_ptr;
	Diurnal_File *output_file = group_ptr->Output_File ();

	//---- adjust the factors ----

	factor = 0.0;
	num = group_ptr->Num_Average () / 2;

	for (i=1; i <= num; i++) {
		factor += i;
	}
	factor = 1.0 / factor;
	forward = group_ptr->Forward () / 100.0;
	backward = group_ptr->Backward () / 100.0;

	nrec = diurnal_data.Num_Records ();

	//---- process each iteration ----

	for (m=0; m < group_ptr->Num_Iteration (); m++) {

		for (diurnal_ptr = diurnal_data.First (); diurnal_ptr; diurnal_ptr = diurnal_data.Next ()) {
			diurnal_ptr->Share (2, diurnal_ptr->Share (1));
			diurnal_ptr->Share (1, 0.0);
		}
		
		for (i=1; i <= nrec; i++) {
			diurnal_ptr = diurnal_data [i];

			share = diurnal_ptr->Share (2);

			forward_total = share * forward;
			backward_total = share * backward;

			diurnal_ptr->Add_Share (1, share - forward_total - backward_total);

			i1 = i2 = i;
			forward_total *= factor;
			backward_total *= factor;

			for (j=num; j > 0; j--) {
				i1--;
				i2++;

				if (i1 < 1) {
					i1 = (group_ptr->Loop_Flag ()) ? nrec : 1;
				}
				if (i2 > nrec) {
					i2 = (group_ptr->Loop_Flag ()) ? 1 : nrec;
				}
				diurnal_ptr = diurnal_data [i1];
				diurnal_ptr->Add_Share (1, backward_total * j);

				diurnal_ptr = diurnal_data [i2];
				diurnal_ptr->Add_Share (1, forward_total * j);
			}
		}
	}

	for (diurnal_ptr = diurnal_data.First (); diurnal_ptr; diurnal_ptr = diurnal_data.Next ()) {
		Show_Progress ();

		output_file->Start_Time (group_ptr->Output_Time (diurnal_ptr->Start_Time ()));
		output_file->End_Time (group_ptr->Output_Time (diurnal_ptr->End_Time ()));
		output_file->Share (diurnal_ptr->Share (1));

		if (!output_file->Write ()) {
			Error ("Writing Output File");
		}
		num_out++;
	}
	num_group++;
}
