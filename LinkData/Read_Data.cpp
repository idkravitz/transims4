//*********************************************************
//	ReadData.cpp - Read the Directional Data File
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	ReadData
//---------------------------------------------------------

void LinkData::Read_Data (void)
{
	int link, anode, bnode, dir;
	double volume, speed;

	Link_Equiv equiv_rec, *equiv_ptr;
	Link_Data *data_ptr;

	//---- read the directional link file ----

	Show_Message ("Reading %s -- Record", dir_file.File_Type ());
	Set_Progress ();

	while (dir_file.Read ()) {
		Show_Progress ();

		if (output_flag) {
			new_dir_file.Copy_Fields (&dir_file);
			new_dir_file.Put_Field (1, 0);
			new_dir_file.Put_Field (2, 0);
		}
		dir_file.Get_Field (from_field, &anode);
		dir_file.Get_Field (to_field, &bnode);
		dir_file.Get_Field (vol_field, &volume);

		if (speed_flag) {
			dir_file.Get_Field (spd_field, &speed);
			speed *= volume;
		}

		//---- find the link ID ----

		dir = 0;
		equiv_rec.anode = anode;
		equiv_rec.bnode = bnode;

		equiv_ptr = (Link_Equiv *) link_equiv.Get (&equiv_rec);

		if (equiv_ptr == NULL) {
			dir = 1;
			equiv_rec.anode = bnode;
			equiv_rec.bnode = anode;

			equiv_ptr = (Link_Equiv *) link_equiv.Get (&equiv_rec);

			if (equiv_ptr == NULL) {
				Write (1, "Link %d-%d was Not Found in the Link Node Equivalence File ", anode, bnode);

				if (output_flag) new_dir_file.Write ();
				continue;
			}
		}
		if (output_flag) {
			new_dir_file.Put_Field (1, equiv_ptr->link);
			new_dir_file.Put_Field (2, dir);
			new_dir_file.Write ();
		}

		//---- locate the link record ----

		link = equiv_ptr->link;

		data_ptr = (Link_Data *) link_data.Get (&link); 

		//---- update the link record ----

		if (dir) {
			data_ptr->volume_ba += volume;
			data_ptr->speed_ba += speed;
			data_ptr->num_ba++;
		} else {
			data_ptr->volume_ab += volume;
			data_ptr->speed_ab += speed;
			data_ptr->num_ab++;
		}
	}
	End_Progress ();
	ndir = Progress_Count ();

	dir_file.Close ();
}
