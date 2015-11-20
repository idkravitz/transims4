//*********************************************************
//	Speed_Bins.cpp - distribute speed bin data
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Speed_Bin_Distribution
//---------------------------------------------------------

int Emissions::Speed_Bin_Distribution (int group)
{
	int i, speed_bin, bin_id, num_out;
	double vmt, vmt2, vht, vht2, bin_wt;

	Value_Map *bin_map;

	//---- TRANSIMS speed bin loop ----

	num_out = 0;

	if (smooth_flag) {

		//---- exclude stop time from the smoothing ----

		vht = spd_bin [0] - spd_bin [1];
		if (vht < 0) vht = 0;

		smooth_data.Input (1, (spd_bin [0] - vht));

		for (i=1; i < num_bins; i++) {
			smooth_data.Input (i+1, spd_bin [i]);
		}
		smooth_data.Smooth ();

		vht = MIN (smooth_data.Output (1) + vht, spd_bin [0]);

		smooth_data.Output (1, vht);

		//---- calculate VMT and VHT ----

		if (moves_bin_flag) {
			for (i=1; i <= smooth_data.Num_Output (); i += 2) {
				bin_id = (i + 1) / 2;

				vht = smooth_data.Output (i);

				if (i == 1) {
					vmt = 0.0;
					vht += vht2 = smooth_data.Output (i + 1) / 2.0;
					vmt += vht2 * i * bin_size;
				} else {
					vmt = vht * (i - 1) * bin_size;

					if (i < smooth_data.Num_Output () + 1) {
						vht += vht2 = smooth_data.Output (i + 1) / 2.0;
						vmt += vht2 * i * bin_size;
					} else if (i < smooth_data.Num_Output ()) {
						vht += vht2 = smooth_data.Output (i + 1);
						vmt += vht2 * i * bin_size;
					}
					vht += vht2 = smooth_data.Output (i - 1) / 2.0;
					vmt += vht2 * (i - 2) * bin_size;
				}
				if (bin_id > max_bin) {
					vmt_bin [max_bin] += vmt;
					vht_bin [max_bin] += vht;
					num_out = max_bin;
				} else {
					vmt_bin [bin_id] = vmt;
					vht_bin [bin_id] = vht;
					if (bin_id > num_out) num_out = bin_id;
				}
			}
		} else {
			for (bin_id=1; bin_id <= smooth_data.Num_Output (); bin_id++) {
				vht = smooth_data.Output (bin_id);
				if (bin_id == 1) {
					vmt = 0.0;
				} else {
					vmt = vht * (bin_id - 1) * bin_size;
				}
				if (bin_id > max_bin) {
					vmt_bin [max_bin] += vmt;
					vht_bin [max_bin] += vht;
					num_out = max_bin;
				} else {
					vmt_bin [bin_id] = vmt;
					vht_bin [bin_id] = vht;
					if (bin_id > num_out) num_out = bin_id;
				}
			}
		}
	} else {
		memset (vmt_bin, '\0', sizeof (vmt_bin));
		memset (vht_bin, '\0', sizeof (vht_bin));

		for (i=0; i < num_bins; i++) {

			//---- get the speed summary record ----

			vht = spd_bin [i];
			if (vht == 0.0) continue;

			vmt = i * UnRound (cell_size) * vht;

			//---- speed bin ----

			speed_bin = i + 1;

			for (bin_id = 0; ; bin_id++) {
				if (bin_id == 0) {
					bin_map = bin_ptr->First_Member (speed_bin, group);
				} else {
					bin_map = bin_ptr->Next_Member (speed_bin, group);
					if (bin_map == NULL) break;
				}
				if (bin_map == NULL) {
					bin_id = speed_bin;
					bin_wt = 1.0;
				} else {
					bin_id = bin_map->Value ();
					bin_wt = bin_map->Weight ();
				}
				vmt_bin [bin_id] += vmt * bin_wt;
				vht_bin [bin_id] += vht * bin_wt;
				if (bin_id > num_out) num_out = bin_id;
			}
		}
	}

	//---- normalized the total vht ----

	vht = vmt = vht2 = vmt2 = 0.0;
	bin_wt = UnRound (cell_size);

	for (i=0; i < num_bins; i++) {
		vht += spd_bin [i];
		vmt += spd_bin [i] * i * bin_wt;
	}
	for (i=1; i <= num_out; i++) {
		vht2 += vht_bin [i];
		vmt2 += vmt_bin [i];
	}
	vht = (vht2 > 0) ? vht / vht2 : 1.0;
	vmt = (vmt2 > 0) ? vmt / vmt2 : 1.0;

	for (i=1; i <= num_out; i++) {
		vht_bin [i] *= vht;
		vmt_bin [i] *= vmt;
	}

	//---- output the smooth sample ----

	if (sample_flag) {
		if (random.Probability () < sample_size) {
			for (i=1; i <= num_bins; i++) {
				sample_file.Put_Field (i, spd_bin [i-1]);
			}
			for (bin_id=1; bin_id <= max_bin; bin_id++, i++) {
				vht = (bin_id <= num_out) ? vht_bin [bin_id] : 0.0;
				sample_file.Put_Field (i, vht);
			}
			if (!sample_file.Write ()) {
				Error ("Writing %s", sample_file.File_Type ());
			}
			num_sample++;
		}
	}
	return (num_out);
}

