//*********************************************************
//	Read_Link.cpp - convert the link data
//*********************************************************

#include "TPPlusNet.hpp"

#include "Link_File.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Link
//---------------------------------------------------------

void TPPlusNet::Read_Link (void)
{
	int i, link, link_id, anode, bnode, xa, ya, xb, yb, index, lanes, num, nrec, nshape, ft, at;
	double length, x1, y1, x2, y2, z1, z2, dx, dy, dz;
	XYZ_Point *pt_ptr;

	Db_Header *file;
	Link_File *new_link;

	Node_Data *node_ptr;

	Link_Name link_rec, *link_ptr;
	Dir_Data dir_rec, *dir_ptr;
	AB_Data ab_rec, *ab_ptr;
	Point_Data points;

	new_link = (Link_File *) Network_Db_Base (NEW_LINK);

	if (link_shape_flag) {
		file = &link_shape_file;
		points.Z_Flag (link_shape_file.Z_Flag ());
	} else {
		file = &link_file;
	}
	link_id = 1;
	nrec = nshape = 0;

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (1000);

	while (((link_shape_flag) ? link_shape_file.Read_Record () : link_file.Read_Record ())) {
		Show_Progress ();

		//---- copy fields and execute user program ----

		new_link->Link (0);
		new_link->Length (0.0);

		new_link->Copy_Fields (file);

		if (convert_flag) {
			if (detail_flag) {
				detail_file.Reset_Record ();
			}
			if (!convert.Execute ()) continue;
		}

		//--- check the anode ----

		anode = new_link->Anode ();

		if (anode == 0) {
			Error ("A-node Number was Not Found");
		}
		if (anode < ext_zone) continue;

		node_ptr = node_data.Get (anode);

		if (node_ptr == NULL) {
			Warning ("A-node %d was Not Found in the Node File", anode);
			continue;
		}
		xa = node_ptr->X ();
		ya = node_ptr->Y ();

		//--- check the bnode ----

		bnode = new_link->Bnode ();

		if (bnode == 0) {
			Error ("B-node Number was Not Found");
		}
		if (bnode < ext_zone) continue;

		node_ptr = node_data.Get (bnode);

		if (node_ptr == NULL) {
			Warning ("B-node %d was Not Found in the Node File", bnode);
			continue;
		}
		xb = node_ptr->X ();
		yb = node_ptr->Y ();

		//---- check for twoway links ----

		ab_rec.anode = bnode;
		ab_rec.bnode = anode;

		ab_ptr = ab_key.Get (&ab_rec);
		
		if (ab_ptr != NULL) {

			//---- update existing record ----

			dir_ptr = dir_data [ab_ptr->dir];

			if (dir_ptr == NULL) {
				Error ("Retrieving Direction Data");
			}
			link_ptr = link_data.Get (dir_ptr->Link ());

			if (link_ptr == NULL) {
				Error ("Retrieving Link Data");
			}

			//---- translate the data fields ----

			dir_rec.Link (link_ptr->Link ());
			dir_rec.Dir (1);
			dir_rec.Thru (new_link->Lanes_AB ());
			dir_rec.Left (new_link->Left_AB ());
			dir_rec.Right (new_link->Right_AB ());

			if (dir_rec.Left () < 0 || dir_rec.Left () > 3) {
				Warning ("Number of Left Pocket Lanes = %d is Out of Range (0..3)", dir_rec.Left ());
			}
			if (dir_rec.Right () < 0 || dir_rec.Right () > 3) {
				Warning ("Number of Right Pocket Lanes = %d is Out of Range (0..3)", dir_rec.Right ());
			}
			if (spdcap_flag) {
				lanes = new_link->Lanes_AB ();

				if (fac_fld > 0 && at_fld > 0) {
					file->Get_Field (at_fld, &at);
					file->Get_Field (fac_fld, &ft);

					if (at < 0 || at >= MAX_AT) goto area_type_error;
					if (ft < 0 || ft >= MAX_FUNCL) goto facility_error;

					dir_rec.Speed (spd_map [at] [ft] [lanes]);
					dir_rec.Time0 (spd_map [at] [ft] [lanes]);
					dir_rec.Capacity (cap_map [at] [ft] [lanes]);
				} else {
					index = (int) new_link->Speed_AB ();
					if (index < 0 || index > 99) goto speed_error;
					dir_rec.Speed (spd_map [index % 10] [index / 10] [lanes]);

					index = (int) new_link->Fspd_AB ();
					if (index < 0 || index > 99) goto speed_error;
					dir_rec.Time0 (spd_map [index % 10] [index / 10] [lanes]);

					index = new_link->Cap_AB ();
					if (index < 0 || index > 99) goto capacity_error;
					dir_rec.Capacity (cap_map [index % 10] [index / 10] [lanes]);
				}

			} else {
				dir_rec.Speed (Round (new_link->Speed_AB ()));
				dir_rec.Time0 (Round (new_link->Fspd_AB ()));
				dir_rec.Capacity (new_link->Cap_AB ());
			}

			//---- check the speeds ----

			if (dir_rec.Speed () < 1) {
				Warning ("Link %d Speed %.1lf is Out of Range", link_ptr->Link (), UnRound (dir_rec.Speed ()));
			}
			if (dir_rec.Time0 () < 1) {
				Warning ("Link %d Speed %.1lf is Out of Range", link_ptr->Link (), UnRound (dir_rec.Time0 ()));
			}

			//---- save the link record ----

			if (!dir_data.Add (&dir_rec)) goto dir_error;

			link_ptr->BA_Dir (dir_data.Record_Index ());

			//---- write the link detail record ----

			if (convert_flag && detail_flag) {
				if (detail_file.Link () != 0) {
					detail_file.Link (link_ptr->Link ());
					detail_file.Dir (1);

					if (!detail_file.Write ()) {
						Error ("Writing Link Detail File");
					}
				}
			}

		} else {

			//---- create a new record ----

			if (new_link->Link () == 0) {
				new_link->Link (link_id++);
			} else if (new_link->Link () >= link_id) {
				link_id = new_link->Link () + 1;
			}

			//---- process link shapes ----

			if (link_shape_flag) {

				//---- copy points into a point array ----

				num = link_shape_file.Num_Points ();
				points.Reset ();
				points.Max_Points (num);

				for (pt_ptr = link_shape_file.points.First (); pt_ptr; pt_ptr = link_shape_file.points.Next ()) {
					pt_ptr->x = UnRound (Round (pt_ptr->x));
					pt_ptr->y = UnRound (Round (pt_ptr->y));
					pt_ptr->z = UnRound (Round (pt_ptr->z));

					points.Add (pt_ptr);
				}

				//---- process the point data ----

				length = 0.0;
				x1 = y1 = z1 = 0.0;
				num = points.Num_Points ();

				for (i=1; i <= num; i++) {
					pt_ptr = points [i];

					x2 = pt_ptr->x;
					y2 = pt_ptr->y;
					z2 = pt_ptr->z;

					if (i > 1) {
						dx = x2 - x1;
						dy = y2 - y1;
						dz = z2 - z1;

						length += sqrt (dx * dx + dy * dy + dz * dz);

						//---- write the shape file ----

						if (i < num && shape_file != NULL) {

							//---- write the shape header ----

							if (i == 2) {
								link = new_link->Link ();

								if (link < 1) {
									Error ("Link Number was Not Defined in the Link File");
								}
								shape_file->Link (link);
								shape_file->Points (num - 2);

								if (!shape_file->Write (false)) goto shape_error;
								nshape++;
								nrec++;
							}
							shape_file->X (x2);
							shape_file->Y (y2);
							shape_file->Z (z2);

							if (!shape_file->Write (true)) goto shape_error;
							nrec++;
						}
					}
					x1 = x2; 
					y1 = y2;
					z1 = z2;
				}
			} else {

				//---- check the link length ----

				dx = xb - xa;
				dy = yb - ya;

				length = sqrt (dx * dx + dy * dy);
			}
			if (new_link->Length () == 0.0) {
				new_link->Length (UnRound ((int) (length + 0.5)));
			}

			//---- translate the data fields ----

			if (new_link->Name_Flag ()) {
				link_rec.Name (new_link->Street ());
			}
			link_rec.Link (new_link->Link ());
			link_rec.Anode (anode);
			link_rec.Bnode (bnode);
			link_rec.Length (Round (new_link->Length ()));
			link_rec.Aoffset (Round (new_link->Setback_A ()));
			link_rec.Boffset (Round (new_link->Setback_B ()));
			link_rec.Use (Use_Code (new_link->Use ()));
			link_rec.Type (Facility_Code (new_link->Type ()));
			link_rec.BA_Dir (0);

			dir_rec.Link (new_link->Link ());
			dir_rec.Dir (0);
			dir_rec.Thru (new_link->Lanes_AB ());
			dir_rec.Left (new_link->Left_AB ());
			dir_rec.Right (new_link->Right_AB ());

			if (dir_rec.Left () < 0 || dir_rec.Left () > 3) {
				Warning ("Number of Left Pocket Lanes = %d is Out of Range (0..3)", dir_rec.Left ());
			}
			if (dir_rec.Right () < 0 || dir_rec.Right () > 3) {
				Warning ("Number of Right Pocket Lanes = %d is Out of Range (0..3)", dir_rec.Right ());
			}
			if (spdcap_flag) {
				lanes = new_link->Lanes_AB ();

				if (fac_fld > 0 && at_fld > 0) {
					file->Get_Field (at_fld, &at);
					file->Get_Field (fac_fld, &ft);

					if (at < 0 || at >= MAX_AT) goto area_type_error;
					if (ft < 0 || ft >= MAX_FUNCL) goto facility_error;

					dir_rec.Speed (spd_map [at] [ft] [lanes]);
					dir_rec.Time0 (spd_map [at] [ft] [lanes]);
					dir_rec.Capacity (cap_map [at] [ft] [lanes]);
				} else {
					index = (int) new_link->Speed_AB ();
					if (index < 0 || index > 99) goto speed_error;
					dir_rec.Speed (spd_map [index % 10] [index / 10] [lanes]);

					index = (int) new_link->Fspd_AB ();
					if (index < 0 || index > 99) goto speed_error;
					dir_rec.Time0 (spd_map [index % 10] [index / 10] [lanes]);

					index = new_link->Cap_AB ();
					if (index < 0 || index > 99) goto capacity_error;
					dir_rec.Capacity (cap_map [index % 10] [index / 10] [lanes]);
				}
			} else {
				dir_rec.Speed (Round (new_link->Speed_AB ()));
				dir_rec.Time0 (Round (new_link->Fspd_AB ()));
				dir_rec.Capacity (new_link->Cap_AB ());
			}

			//---- check the speeds ----

			if (dir_rec.Speed () < 1) {
				Warning ("Link %d Speed %.1lf is Out of Range", link_rec.Link (), UnRound (dir_rec.Speed ()));
			}
			if (dir_rec.Time0 () < 1) {
				Warning ("Link %d Speed %.1lf is Out of Range", link_rec.Link (), UnRound (dir_rec.Time0 ()));
			}

			//---- save the link record ----

			if (!dir_data.Add (&dir_rec)) goto dir_error;

			ab_rec.anode = anode;
			ab_rec.bnode = bnode;
			ab_rec.dir = dir_data.Record_Index ();

			if (!ab_key.Add (&ab_rec)) {
				Error ("Adding Directional Index");
			}
			link_rec.AB_Dir (ab_rec.dir);

			if (!link_data.Add (&link_rec)) {
				Error ("Adding Link Data");
			}

			//---- write the link detail record ----

			if (convert_flag && detail_flag) {
				if (detail_file.Link () != 0) {
					detail_file.Link (link_rec.Link ());
					detail_file.Dir (0);

					if (!detail_file.Write ()) {
						Error ("Writing Link Detail File");
					}
				}
			}
		}
	}
	End_Progress ();
	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	if (link_shape_flag) {
		link_shape_file.Close ();

		if (shape_file != NULL) {
			shape_file->Close ();
			Write (1, "Number of %s Records = %d", shape_file->File_Type (), nrec);
			Write (1, "Number of Links with Shape Points = %d", nshape);
		}
	} else {
		link_file.File_Access (READ);
		link_file.Close ();
	}
	return;

speed_error:
	Error ("Speed Class %d is Out of Range (0..99)", index);
	return;

capacity_error:
	Error ("Capacity Class %d is Out of Range (0..99)", index);
	return;

facility_error:
	Error ("Facility Index %d is Out of Range (0..%d)", ft, MAX_FUNCL - 1);
	return;

area_type_error:
	Error ("Area Type Index %d is Out of Range (0..%d)", at, MAX_AT - 1);
	return;

dir_error:
	Error ("Adding Directional Data");

shape_error:
	Error ("Writing Shape File");
}
