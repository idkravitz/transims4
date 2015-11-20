//*********************************************************
//	New_Link.cpp - write the new link records
//*********************************************************

#include "VISSIMNet.hpp"

#include "Link_File.hpp"
#include "Shape_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Link
//---------------------------------------------------------

void VISSIMNet::New_Link (void)
{
	int i, j, node, link, length;
	char *name;
	double factor, offset, len;

	Link_Splits *split_ptr;
	Link_Name *link_ptr, *new_link_ptr;
	Dir_List *dir_ab_ptr, *dir_ba_ptr, *new_dir_ptr;
	Link_File *link_file;
	Shape_Data *shape_ptr, *new_shape_ptr;
	Shape_File *shape_file;
	XYZ_Point *point;
	XYZ *pt;

	//---- copy and add links ----

	for (split_ptr = (Link_Splits *) link_splits.First_Key (); split_ptr; split_ptr = (Link_Splits *) link_splits.Next_Key ()) {
		link_ptr = link_data [split_ptr->record];

		dir_ab_ptr = dir_data [link_ptr->AB_Dir ()];
		dir_ba_ptr = dir_data [link_ptr->BA_Dir ()];
		offset = 0.0;

		for (i=0; i <= split_ptr->num_splits; i++) {
			new_link_ptr = new_link_data.New_Record (true);

			memcpy (new_link_ptr, link_ptr, sizeof (Link_Data));
			new_link_ptr->Name (link_ptr->Name ());

			new_link_ptr->Link (link_ptr->Link () * MAX_SPLITS + i);

			if (i < split_ptr->num_splits) {
				new_link_ptr->Bnode (split_ptr->node [i]);
				length = split_ptr->offset [i];
				new_link_ptr->Boffset (0);
			} else {
				length = new_link_ptr->Length ();
			}
			if (i > 0) {
				new_link_ptr->Anode (split_ptr->node [i-1]);
				length -= split_ptr->offset [i-1];
				new_link_ptr->Aoffset (0);
				offset = UnRound (split_ptr->offset [i-1]);
			}
			len = UnRound (length);
			new_link_ptr->Length (length);
			factor = (double) length / link_ptr->Length ();

			//---- split the link shape ----

			if (link_ptr->Shape () > 0) {
				new_shape_ptr = new_shape_data.New_Record (true);

				new_shape_ptr->Link (new_link_ptr->Link ());

				if (split_ptr->num_splits == 0) {
					shape_ptr = shape_data [link_ptr->Shape ()];

					new_shape_ptr->Max_Points (shape_ptr->Points ());

					for (pt = shape_ptr->First_Point (); pt; pt = shape_ptr->Next_Point ()) {
						if (!new_shape_ptr->Point (pt)) goto shape_error;
					}
				} else {
					Link_Shape (link_ptr, 0, &points, offset, len);

					if (points.Num_Points () > 2) {
						new_shape_ptr->Max_Points (points.Num_Points () - 2);

						for (j=2; j < points.Num_Points (); j++) {
							point = points [j];
							if (!new_shape_ptr->Point (Round (point->x), Round (point->y))) goto shape_error;
						}
					}
				}
				if (new_shape_ptr->Points () > 0) {
					if (!new_shape_data.Add ()) goto shape_error;

					new_link_ptr->Shape (new_shape_data.Record_Index ());
				} else {
					new_link_ptr->Shape (0);
				}
			} else {
				new_link_ptr->Shape (0);
			}

			//---- process each direction ----

			if (dir_ab_ptr != NULL) {
				new_dir_ptr = new_dir_data.New_Record (true);

				memcpy (new_dir_ptr, dir_ab_ptr, sizeof (Dir_Data));

				new_dir_ptr->Link (new_link_ptr->Link ());
				new_dir_ptr->Time0 ((int) (new_dir_ptr->Time0 () * factor + 0.5));

				if (!new_dir_data.Add ()) {
					Error ("Adding Link Direction Record");
				}
				new_link_ptr->AB_Dir (new_dir_data.Num_Records ());
			}

			if (dir_ba_ptr != NULL) {
				new_dir_ptr = new_dir_data.New_Record (true);

				memcpy (new_dir_ptr, dir_ba_ptr, sizeof (Dir_Data));

				new_dir_ptr->Link (new_link_ptr->Link ());
				new_dir_ptr->Time0 ((int) (new_dir_ptr->Time0 () * factor + 0.5));
				if (!new_dir_data.Add ()) {
					Error ("Adding Link Direction Record");
				}
				new_link_ptr->BA_Dir (new_dir_data.Num_Records ());
			}
			if (!new_link_data.Add ()) {
				Error ("Adding Link Record");
			}
		}
	}

	//---- add parking connection links ----

	new_link_ptr = (Link_Name *) new_link_data.Last_Key ();
	link = ((new_link_ptr->Link () + 1) / 100 + 1) * 100;

	for (split_ptr = (Link_Splits *) link_splits.First_Key (); split_ptr; split_ptr = (Link_Splits *) link_splits.Next_Key ()) {
		if (split_ptr->num_splits == 0) continue;

		for (i=0; i < split_ptr->num_splits; i++) {
			for (j=0; j < 2; j++) {
				if (j == 0) {
					node = split_ptr->park_ab [i];
					if (node <= 0) continue;
					split_ptr->link_ab [i] = link;
				} else {
					node = split_ptr->park_ba [i];
					if (node <= 0) continue;
					split_ptr->link_ba [i] = link;
				}
				new_link_ptr = new_link_data.New_Record (true);

				new_link_ptr->Link (link++);
				new_link_ptr->Anode (split_ptr->node [i]);
				new_link_ptr->Bnode (node);
				new_link_ptr->Length (Round (PARKING_OFFSET));
				new_link_ptr->Use (ANY);
				new_link_ptr->Type (LOCAL);
				new_link_ptr->Name ("Parking");

				new_dir_ptr = new_dir_data.New_Record (true);

				new_dir_ptr->Link (new_link_ptr->Link ());
				new_dir_ptr->Dir (0);
				new_dir_ptr->Thru (1);
				new_dir_ptr->Speed (Round (PARKING_SPEED));
				new_dir_ptr->Capacity (300);
				new_dir_ptr->Time0 (Round (PARKING_OFFSET/PARKING_SPEED));

				if (!new_dir_data.Add ()) {
					Error ("Adding Parking Link");
				}
				new_link_ptr->AB_Dir (new_dir_data.Num_Records ());

				new_dir_ptr = new_dir_data.New_Record (true);

				new_dir_ptr->Link (new_link_ptr->Link ());
				new_dir_ptr->Dir (1);
				new_dir_ptr->Thru (1);
				new_dir_ptr->Speed (Round (PARKING_SPEED));
				new_dir_ptr->Capacity (300);
				new_dir_ptr->Time0 (Round (PARKING_OFFSET/PARKING_SPEED));

				if (!new_dir_data.Add ()) {
					Error ("Adding Parking Link");
				}
				new_link_ptr->BA_Dir (new_dir_data.Num_Records ());

				if (!new_link_data.Add ()) {
					Error ("Adding Parking Link");
				}
			}
		}
	}

	//---- write the new link file ----

	if (!Network_File_Flag (NEW_LINK)) return;

	link_file = (Link_File *) Network_Db_Base (NEW_LINK);

	for (link_ptr = new_link_data.First (); link_ptr; link_ptr = new_link_data.Next ()) {
		link_file->Link (link_ptr->Link ());

		name = link_ptr->Name ();
		link_file->Street ((name != NULL) ? name : (char *) "");

		link_file->Anode (link_ptr->Anode ());
		link_file->Bnode (link_ptr->Bnode ());
		link_file->Length (UnRound (link_ptr->Length ()));
		link_file->Setback_A (UnRound (link_ptr->Aoffset ()));
		link_file->Setback_B (UnRound (link_ptr->Boffset ()));

		link_file->Use (Use_Code (link_ptr->Use ()));
		link_file->Type (Facility_Code ((Facility_Type) link_ptr->Type ()));

		if (link_ptr->AB_Dir () > 0) {
			dir_ab_ptr = new_dir_data [link_ptr->AB_Dir ()];

			link_file->Bearing_A (dir_ab_ptr->In_Bearing ());
			link_file->Bearing_B (dir_ab_ptr->Out_Bearing ());

			link_file->Lanes_AB (dir_ab_ptr->Thru ());
			link_file->Left_AB (dir_ab_ptr->Left ());
			link_file->Right_AB (dir_ab_ptr->Right ());
			link_file->Speed_AB (UnRound (dir_ab_ptr->Speed ()));
			link_file->Fspd_AB ((double) link_ptr->Length () / dir_ab_ptr->Time0 ());
			link_file->Cap_AB (dir_ab_ptr->Capacity ());
		} else {
			link_file->Bearing_A (0);
			link_file->Bearing_B (0);
			link_file->Lanes_AB (0);
			link_file->Left_AB (0);
			link_file->Right_AB (0);
			link_file->Speed_AB (0);
			link_file->Fspd_AB (0);
			link_file->Cap_AB (0);
		}

		if (link_ptr->BA_Dir () > 0) {
			dir_ba_ptr = new_dir_data [link_ptr->BA_Dir ()];

			link_file->Lanes_BA (dir_ba_ptr->Thru ());
			link_file->Left_BA (dir_ba_ptr->Left ());
			link_file->Right_BA (dir_ba_ptr->Right ());
			link_file->Speed_BA (UnRound (dir_ba_ptr->Speed ()));
			link_file->Fspd_BA ((double) link_ptr->Length () / dir_ba_ptr->Time0 ());
			link_file->Cap_BA (dir_ba_ptr->Capacity ());
		} else {
			link_file->Lanes_BA (0);
			link_file->Left_BA (0);
			link_file->Right_BA (0);
			link_file->Speed_BA (0);
			link_file->Fspd_BA (0);
			link_file->Cap_BA (0);
		}
		if (!link_file->Write ()) {
			Error ("Writing %s", link_file->File_Type ());
		}
	}
	link_file->Close ();

	Print (2, "Number of New Link Records = %d", new_link_data.Num_Records ());

	//---- write the new shape file ----

	if (!Network_File_Flag (NEW_SHAPE)) return;

	shape_file = (Shape_File *) Network_Db_Base (NEW_SHAPE);
	j = 0;

	for (shape_ptr = new_shape_data.First (); shape_ptr; shape_ptr = new_shape_data.Next ()) {
		shape_file->Link (shape_ptr->Link ());
		shape_file->Points (shape_ptr->Points ());

		if (!shape_file->Write (false)) {
			Error ("Writing %s", shape_file->File_Type ());
		}
		j++;

		for (pt = shape_ptr->First_Point (); pt; pt = shape_ptr->Next_Point ()) {
			shape_file->X (UnRound (pt->x));
			shape_file->Y (UnRound (pt->y));
			shape_file->Z (UnRound (pt->z));

			if (!shape_file->Write (true)) {
				Error ("Writing %s", shape_file->File_Type ());
			}
			j++;
		}		
	}
	shape_file->Close ();

	Print (2, "Number of New Shape Records = %d", j);
	return;

shape_error:
	Error ("Adding Shape Point");
}
