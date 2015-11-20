//*********************************************************
//	WriteData.cpp - Write the Directional Data File
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	WriteData
//---------------------------------------------------------

void LinkData::Write_Data (void)
{
	double vol_ab, vol_ba, spd_ab, spd_ba;

	Link_Data *data_ptr;

	spd_ab = spd_ba = 0;

	Show_Message ("Writing %s -- Record", data_file.File_Type ());
	Set_Progress ();

	for (data_ptr = (Link_Data *) link_data.First_Key (); data_ptr != NULL; data_ptr = (Link_Data *) link_data.Next_Key ()) {
		Show_Progress ();

		if (data_ptr->num_ab == 0 && data_ptr->num_ba == 0) continue;

		if (data_ptr->num_ab > 0) {
			vol_ab = data_ptr->volume_ab / data_ptr->num_ab;
		} else {
			vol_ab = 0.0;
		}
		if (data_ptr->num_ba > 0) {
			vol_ba = data_ptr->volume_ba / data_ptr->num_ba;
		} else {
			vol_ba = 0.0;
		}
		if (speed_flag) {
			if (data_ptr->volume_ab > 0.0) {
				spd_ab = data_ptr->speed_ab / data_ptr->volume_ab;
			} else {
				spd_ab = 0.0;
			}
			if (data_ptr->volume_ba > 0) {
				spd_ba = data_ptr->speed_ba / data_ptr->volume_ba;
			} else {
				spd_ba = 0.0;
			}
		}
		data_file.Put_Field (1, data_ptr->link);
		data_file.Put_Field (2, data_ptr->anode);
		data_file.Put_Field (3, data_ptr->bnode);
		data_file.Put_Field (ab_vol_field, vol_ab);
		data_file.Put_Field (ba_vol_field, vol_ba);
		data_file.Put_Field (ab_spd_field, spd_ab);
		data_file.Put_Field (ba_spd_field, spd_ba);

		if (!data_file.Write ()) {
			Error ("Writing Link Data File");
		}
		nlink++;
	}
	End_Progress ();

	data_file.Close ();
}
