//*********************************************************
//	Read_Snapshot.cpp - reads the snapshot file
//*********************************************************

#include "ArcSnapshot.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Snapshot
//---------------------------------------------------------

void ArcSnapshot::Read_Snapshot (void)
{
	int i, time, last_time, period, index, type, pass;
	double size;
	char buffer [STRING_BUFFER];

	Db_Header *file;

	if (occupancy_flag) {
		file = &occupancy_file;
	} else {
		file = &snapshot_file;
	}

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (10000);
	Print (1);

	//---- read each snapshot record ----

	last_time = -1;
	period = type = 0;

	while (file->Read ()) {
		Show_Progress ();

		//---- get the time data ----

		if (occupancy_flag) {
			time = time_period.Step (occupancy_file.End ());
		} else {
			
			//---- check the vehicle type ----

			type = snapshot_file.VehType ();

			if (veh_type_flag && !veh_types.In_Range (type)) continue;

			time = time_period.Step (snapshot_file.Time ());
		}
		if (time > max_time) break;
		if (!time_period.In_Range (time) || !time_period.At_Increment (time)) continue;

		if (sum_flag && time != last_time) {
			if (last_time >= 0 && period) {
				Write_Summary (period);
			}
			period = time_period.Period (time);

			for (i=0; i < max_type; i++) {
				if (vehicles [i] != NULL) {
					memset (vehicles [i], '\0', num_links * sizeof (int));
					memset (persons [i], '\0', num_links * sizeof (int));
				}
			}
		}

		//---- open a new file ----

		if (snapshot_flag && time != last_time) {
			if (last_time >= 0) {
				arcview_snapshot.Close ();
			}

			//---- create a new shape file ----

			str_fmt (buffer, sizeof (buffer), "%s_%s.shp", shapename, time_period.Time_Label (time, pad_flag));

			if (!arcview_snapshot.Open (buffer)) {
				File_Error ("Opening ArcView Snapshot File", arcview_snapshot.Shape_Filename ());
			}
			arcview_snapshot.Write_Header ();
		}
		last_time = time;
		pass = snapshot_file.Passengers () + 1;

		if (sum_flag) {
			index = link_data.Get_Index (snapshot_file.Link ());

			if (index > 0) {
				vehicles [0] [index]++;
				persons [0] [index] += pass;

				if (type < max_type) {
					vehicles [type] [index]++;
					persons [type] [index] += pass;
				}
			}
		}
		if (circle_flag) {
			size = sqrt (pass / PI);
			arcview_snapshot.Put_Field (circle_field, size);
		}
		if (square_flag) {
			size = sqrt ((double) pass);
			arcview_snapshot.Put_Field (square_field, size);
		}
		arcview_snapshot.Copy_Fields (file);

		Draw_Vehicle ();
	}
	End_Progress ();

	file->Close ();

	if (snapshot_flag && last_time >= 0) {
		arcview_snapshot.Close ();
	}
	Print (2, "Number of Snapshot Records Read = %d", Progress_Count ());
	Print (1, "Number of Snapshot Records Saved = %d", nsaved);

	if (sum_flag) {
		Print (2, "Number of Link Summary Records Saved = %d", num_sum);
	}
}

