//*********************************************************
//	WriteLink.cpp - Write the TRANSIMS Link File
//*********************************************************

#include "TransimsNet.hpp"

#include "Link_File.hpp"
#include "Shape_File.hpp"
#include "Location_File.hpp"
#include "Parking_File.hpp"
#include "Access_File.hpp"
#include "Point_Array.hpp"
#include "Utility.hpp"

#include <math.h>

//---------------------------------------------------------
//	Write_Link
//---------------------------------------------------------

void TransimsNet::Write_Link (void)
{
	bool auto_flag, centroid_flag, save_flag;
	int median_field, thru_ab_field, thru_ba_field, loc_id, park_id, acc_id;
	int i, j, num, x1, y1, z1, x2, y2, z2, xa, ya, za, xb, yb, zb;
	int setbacka, setbackb, bearinga, bearingb, type, length, split_len, len_error, link_len;
	int node, zone, link, dir, anode, bnode, funcl, nsplit;
	double dx, dy, dz, xy_len, speed, factor, offset, len;
	char *str_rec, buffer [FIELD_BUFFER];

	XYZ_Point *pt_ptr;
	Node_List *anode_ptr, *bnode_ptr;
	Zone_Data *zone_ptr;
	Link_Nodes *link_ptr;
	Shape_Data *shape_ptr;
	Dir_Connect *ab_ptr, *ba_ptr;
	Area_Type *type_ptr;
	Connection *connect_ptr;
	Point_Data points;

	Link_File *link_file;
	Shape_File *shape_file;
	Location_File *location_file;
	Parking_File *parking_file;
	Access_File *access_file;

	char *ext_dir [] = {
		"External Origin", "External Destination"
	};

	link_file = (Link_File *) Network_Db_Base (NEW_LINK);

	median_field = link_file->Optional_Field ("TWOWAYTURN", "MEDIAN", "DIVIDED");
	thru_ab_field = link_file->Optional_Field ("THRUB", "THRU_AB");
	thru_ba_field = link_file->Optional_Field ("THRUA", "THRU_BA");

	if (shape_flag) {
		shape_file = (Shape_File *) Network_Db_Base (NEW_SHAPE);
	}
	location_file = (Location_File *) Network_Db_Base (NEW_ACTIVITY_LOCATION);
	parking_file = (Parking_File *) Network_Db_Base (NEW_PARKING);
	access_file = (Access_File *) Network_Db_Base (NEW_PROCESS_LINK);

	len_error = Round (1.0);	//---- one meter ----

	//---- round up the existing maximum id numbers ----

	if (update_flag) {
		if (mparking > 0) {
			mparking = (((mparking + 99) / 100) + 1) * 100;
		}
		if (mactivity > 0) {
			mactivity = (((mactivity + 99) / 100) + 1) * 100;
		}
		if (mprocess > 0) {
			mprocess = (((mprocess + 99) / 100) + 1) * 100;

		}
		if (mpocket > 0) {
			mpocket = (((mpocket + 99) / 100) + 1) * 100;
		}
	}

	Show_Message ("Writing %s - Record", link_file->File_Type ());
	Set_Progress ();

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();
		if (delete_flag && delete_links.In_Range (link_ptr->Link ())) continue;

		if (update_flag && !update_links.In_Range (link_ptr->Link ())) {
			if (shape_flag && link_ptr->Shape () > 0) {
				shape_ptr = shape_data [link_ptr->Shape ()];

				shape_file->Nest (false);
				shape_file->Link (link_ptr->Link ());

				num = shape_ptr->Points ();
				shape_file->Points (num);
				
				if (!shape_file->Write ()) goto shape_error;

				shape_file->Nest (true);

				for (i=1; i <= num; i++) {
					shape_ptr->Point (i, &x2, &y2, &z2);

					shape_file->X (UnRound (x2));
					shape_file->Y (UnRound (y2));
					shape_file->Z (UnRound (z2));

					if (!shape_file->Write ()) goto shape_error;
				}
			}
			continue;
		}

		//---- check for deleted links ----

		funcl = link_ptr->Type ();
		if (funcl == 0) continue;

		//---- update zone connector attributes ----

		if (funcl == EXTERNAL) {
			External_Station (link_ptr);

			if (link_ptr->Use () == 0) {
				if ((link_ptr->Anode () >= ext_zone && link_ptr->Anode () <= high_zone) || 
					(link_ptr->Bnode () >= ext_zone && link_ptr->Bnode () <= high_zone)) {
					Warning ("External Link %d was Not Connected to the Network", link_ptr->Link ());
				}
				continue;
			}
		}
		link = link_ptr->Link ();
		anode = link_ptr->Anode ();
		bnode = link_ptr->Bnode ();

		if ((anode <= high_zone || bnode <= high_zone) && funcl != EXTERNAL && !connect_flag && !replicate_flag) {
			Warning ("Link %d is Not a Zone Connector", link);
			continue;
		}

		//---- construct the node list / street name ----

		if (node_string.Is_Open ()) {
			str_rec = node_string.Record ();
			num = node_string.Max_Size ();

			node = link_ptr->First_Node ();

			if (node == 0) {
				str_fmt (str_rec, num, "%d = %d %d", link, anode, bnode);
			} else {
				str_fmt (str_rec, num, "%d = %d", link, anode);

				for (; node; node = link_ptr->Next_Node ()) {
					str_fmt (buffer, sizeof (buffer), " %d", node);
					str_cat (str_rec, num, buffer);
				}
				str_fmt (buffer, sizeof (buffer), " %d", bnode);
				str_cat (str_rec, num, buffer);
			}
			if (!node_string.Write ()) {
				Error ("Writing %s", node_string.File_Type ());
			}
		}

		//---- construct the mode string ----

		auto_flag = Use_Permission (link_ptr->Use (), CAR);

		//---- get the anode ----

		anode_ptr = node_data.Get (anode);
		setbacka = 0;

		if (anode_ptr == NULL) {
			Write (1, "Link Anode %d was Not Found in the Node List", anode);
			Show_Message ("Record");
			xa = ya  = za = 0;
		} else {
			xa = anode_ptr->X ();
			ya = anode_ptr->Y ();
			za = anode_ptr->Z ();
			if (anode_ptr->Type () == INTERSECTION || anode_ptr->Type () == INTERCHANGE) {
				setbacka = setback;
			}
		}

		//---- write shape file ----

		x1 = xa;
		y1 = ya;
		z1 = za;
		xy_len = 0.0;

		if (shape_flag && link_ptr->Shape () > 0) {
			shape_ptr = shape_data [link_ptr->Shape ()];

			shape_file->Nest (false);
			shape_file->Link (link_ptr->Link ());

			num = shape_ptr->Points ();
			shape_file->Points (num);
			
			if (!shape_file->Write ()) goto shape_error;
			nshapes++;
			nshape++;

			shape_file->Nest (true);

			for (i=1; i <= num; i++) {
				shape_ptr->Point (i, &x2, &y2, &z2);

				shape_file->X (UnRound (x2));
				shape_file->Y (UnRound (y2));
				shape_file->Z (UnRound (z2));

				if (!shape_file->Write ()) goto shape_error;
				nshape++;

				dx = x2 - x1;
				dy = y2 - y1;
				dz = z2 - z1;

				xy_len += sqrt (dx * dx + dy * dy + dz * dz);

				x1 = x2;
				y1 = y2;
				z1 = z2;
			}
		}

		//---- get the bnode ----

		bnode_ptr = node_data.Get (bnode);
		setbackb = 0;

		if (bnode_ptr == NULL) {
			Write (1, "Link Bnode %d was Not Found in the Node List", bnode);
			Show_Message ("Record");
			xb = yb = zb = 0;
		} else {
			xb = bnode_ptr->X ();
			yb = bnode_ptr->Y ();
			zb = bnode_ptr->Z ();
			if (bnode_ptr->Type () == INTERSECTION || bnode_ptr->Type () == INTERCHANGE) {
				setbackb = setback;
			}
		}
		dx = xb - x1;
		dy = yb - y1;
		dz = zb - z1;

		xy_len += sqrt (dx * dx + dy * dy + dz * dz);

		//---- check the link length -----

		length = link_ptr->Length ();

		if (length + len_error < xy_len) {
			nlength++;
			length = (int) (xy_len + 0.5);
		} else if (length > xy_len * max_length_factor) {
			nlength++;
			length = (int) (xy_len * max_length_factor + 0.5);
		}
		if (length + len_error < min_length) {
			nshort++;
			length = min_length;
		}
		link_ptr->Length (length);
		link_len = length - setbacka - setbackb;
		if (link_len < 0) link_len = 0;
		bearinga = bearingb = 0;

		//---- determine the thru link and pocket lanes in AB direction ----

		dir = link_ptr->AB_Dir ();

		if (dir > 0) {
			ab_ptr = dir_data [dir];

			if (funcl != EXTERNAL && link_ptr->Anode () != link_ptr->Bnode () &&
				!((connect_flag || replicate_flag) && link_ptr->Bnode () < ext_zone)) {
				Pocket_Lanes (ab_ptr, bnode_ptr, link_len);
			}
			bearinga = ab_ptr->In_Bearing ();
			bearingb = ab_ptr->Out_Bearing ();
		} else {
			ab_ptr = NULL;
		}

		//---- determine the thru link and pocket lanes in BA direction ----

		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			ba_ptr = dir_data [dir];

			if (funcl != EXTERNAL && link_ptr->Anode () != link_ptr->Bnode () &&
				!((connect_flag || replicate_flag) && link_ptr->Anode () < ext_zone)) {
				Pocket_Lanes (ba_ptr, anode_ptr, link_len);
			}
			if (ab_ptr == NULL) {
				bearinga = compass.Flip (ba_ptr->Out_Bearing ());
				bearingb = compass.Flip (ba_ptr->In_Bearing ());
			}
		} else {
			ba_ptr = NULL;
		}

		//---- write the link record ----

		len = UnRound (link_ptr->Length ());

		link_file->Link (link_ptr->Link ());
		link_file->Anode (link_ptr->Anode ());
		link_file->Bnode (link_ptr->Bnode ());
		link_file->Length (len);

		link_file->Type (Facility_Code ((Facility_Type) link_ptr->Type ()));
		link_file->Use (Use_Code ((Use_Type) link_ptr->Use ()));

		if (median_field) {
			if ((funcl < PRINCIPAL || funcl == RAMP) 
				&& link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {

				link_file->Put_Field (median_field, "T");
			} else {
				link_file->Put_Field (median_field, "F");
			}
		}
		//grade

		if (name_flag) {
			link_file->Street (link_ptr->Name ());
		}
		if (link_file->Notes_Flag ()) {
			if (link_ptr->Notes () != NULL) {
				link_file->Notes (link_ptr->Notes ());
			} else if (funcl == EXTERNAL) {
				link_file->Notes ("External Connector");
			} else {
				link_file->Notes ("Network Link");
			}
		}
		if (length - setbacka - setbackb < min_length) {
			setbacka = setbackb = 0;
		}
		link_file->Setback_A (UnRound (setbacka));
		link_file->Setback_B (UnRound (setbackb));
		link_file->Bearing_A (bearinga);
		link_file->Bearing_B (bearingb);

		if (ab_ptr) {
			link_file->Lanes_AB (ab_ptr->Thru ());
			link_file->Left_AB (ab_ptr->Left ());
			link_file->Right_AB (ab_ptr->Right ());

			speed = UnRound (ab_ptr->Speed ());

			if (update_flag && update_links.In_Range (link_ptr->Link ())) {
				factor = (double) link_ptr->Length () / ab_ptr->Time0 ();
				link_file->Fspd_AB (factor);
			} else {
				link_file->Fspd_AB (speed);
				speed = ((int) ((speed + cell_size / 2) / cell_size) + 1) * cell_size;
			}
			link_file->Speed_AB (speed);

			link_file->Cap_AB (ab_ptr->Capacity ());

			if (thru_ab_field) {
				connect_ptr = ab_ptr->To_Thru ();

				if (connect_ptr != NULL) {
					ab_ptr = dir_data [connect_ptr->Dir ()];	
					link_file->Put_Field (thru_ab_field, ab_ptr->Link ());
				} else {
					link_file->Put_Field (thru_ab_field, 0);
				}
			}
		} else {
			link_file->Lanes_AB (0);
			link_file->Left_AB (0);
			link_file->Right_AB (0);
			link_file->Fspd_AB (0);
			link_file->Speed_AB (0);
			link_file->Cap_AB (0);

			if (thru_ab_field) {
				link_file->Put_Field (thru_ab_field, 0);
			}
		}
		if (ba_ptr) {
			link_file->Lanes_BA (ba_ptr->Thru ());
			link_file->Left_BA (ba_ptr->Left ());
			link_file->Right_BA (ba_ptr->Right ());
			
			speed = UnRound (ba_ptr->Speed ());
		
			if (update_flag && update_links.In_Range (link_ptr->Link ())) {
				factor = (double) link_ptr->Length () / ba_ptr->Time0 ();
				link_file->Fspd_BA (factor);
			} else {
				link_file->Fspd_BA (speed);
				speed = ((int) ((speed + cell_size / 2) / cell_size) + 1) * cell_size;
			}
			link_file->Speed_BA (speed);

			link_file->Cap_BA (ba_ptr->Capacity ());

			if (thru_ba_field) {
				connect_ptr = ba_ptr->To_Thru ();

				if (connect_ptr != NULL) {
					ba_ptr = dir_data [connect_ptr->Dir ()];	
					link_file->Put_Field (thru_ba_field, ba_ptr->Link ());
				} else {
					link_file->Put_Field (thru_ba_field, 0);
				}
			}
		} else {
			link_file->Lanes_BA (0);
			link_file->Left_BA (0);
			link_file->Right_BA (0);
			link_file->Fspd_BA (0);
			link_file->Speed_BA (0);
			link_file->Cap_BA (0);

			if (thru_ba_field) {
				link_file->Put_Field (thru_ba_field, 0);
			}
		}
		if (!link_file->Write ()) {
			Error ("Writing New Link File");
		}
		nlink++;
		centroid_flag = save_flag = false;

		if (funcl == EXTERNAL) {
			nexternal++;

			if (!auto_flag && Use_Permission (link_ptr->Use (), RAIL)) continue;

			zone = anode;
			nsplit = 2;

		} else {

			//---- check for lane use records ----

			if (lane_use_flag) {
				Write_Lane_Use (link_ptr);
			}

			if (anode == bnode && xy_len == 0.0) continue;

			//---- skip auto access restrictions ----

			if (!access_flag [funcl] || !auto_flag) continue;

			//---- check for mpo replication -----

			if (replicate_flag) {

				if (anode >= ext_zone && bnode >= ext_zone) continue;

				centroid_flag = true;
				if (anode < ext_zone) {
					zone = anode;
				} else {
					zone = bnode;
				}
				nsplit = 2;

			} else {

				//---- get the area type ----

				if (zone_flag) {
					x1 = (xa + xb) / 2;
					y1 = (ya + yb) / 2;

					zone = Closest_Zone (x1, y1);

					zone_ptr = zone_data.Get (zone);

					if (zone_ptr == NULL) {
						Error ("Zone %d was Not Found in the Zone file", zone);
					}
					type = zone_ptr->Area_Type ();
				} else {
					type = 2;
				}
				type_ptr = area_type.Best (type);

				if (type_ptr == NULL || type_ptr->Split_Len () == 0) continue;

				split_len = type_ptr->Split_Len ();

				if (length < split_len) continue;

				nsplit = (length + split_len / 2) / split_len;

				if (nsplit < 2) nsplit = 2;
				if (nsplit > max_splits + 1) nsplit = max_splits + 1;
			}
		}

		//---- calculate the access points ----

		for (i=1; i < nsplit; i++) {

			node = bnode;

			if (funcl == EXTERNAL || centroid_flag) {
				factor = external_offset / xy_len;
				if (factor > 0.5) factor = 0.5;
			} else {
				factor = (double) i / (double) nsplit;
			}
			offset = len * (1.0 - factor);

			for (j=0; j < 2; j++) {
				if (funcl == EXTERNAL) {
					if (j) {
						if (link_ptr->BA_Dir () == 0) continue;
					} else {
						if (link_ptr->AB_Dir () == 0) continue;
					}
				}
				Link_Shape (link_ptr, j, &points, len - offset, 0.0, location_side);

				pt_ptr = points [1];

				dx = pt_ptr->x;
				dy = pt_ptr->y;
				dz = pt_ptr->z;

				if (j) {
					node = anode;
					offset = len - offset;
				}

				//---- add activity locations ----

				if (centroid_flag) {
					save_flag = (zone_centroid.Get_Index (zone) == 0);

					if (save_flag) {
						nactivity++;
						zone_centroid.Add (zone);
					}
					loc_id = zone;
				} else {
					nactivity++;
					loc_id = mactivity + nactivity;
					save_flag = true;
				}
				if (save_flag) {
					location_file->Location (loc_id);
					location_file->Link (link);
					location_file->Node (node);
					location_file->Offset (offset);
					location_file->X (dx);
					location_file->Y (dy);
					location_file->Z (dz);

					if (funcl == EXTERNAL || centroid_flag) {
						location_file->Zone (zone);
						location_file->Put_Field ("LAYER", "AUTO");
						if (centroid_flag) {
							location_file->Notes ("Zone_Centroid");
						} else {
							location_file->Notes (ext_dir [j]);
						}
					} else {
						location_file->Zone (Closest_Zone (Round (dx), Round (dy)));
						location_file->Put_Field ("LAYER", "WALK");
						location_file->Notes ("Land-Use Activity");
					}
					if (!location_file->Write ()) {
						Error ("Writing Activity Record %d", nactivity);
					}
				}

				//---- add parking lot ----

				nparking++;
				park_id = mparking + nparking;

				parking_file->ID (park_id);
				parking_file->Link (link);
				parking_file->Node (node);
				parking_file->Offset (offset);
				parking_file->Use ("ANY");
				parking_file->Space (0);
				parking_file->Start ("ALL");
				parking_file->End ("ALL");
				parking_file->Hourly (0);
				parking_file->Daily (0);

				parking_file->Put_Field ("GENERIC", "T");

				if (funcl == EXTERNAL) {
					parking_file->Type ("BNDRY");
					parking_file->Notes (ext_dir [j]);
				} else {
					parking_file->Type ("LOT");
					parking_file->Notes ("Parking Lot");
				}
				if (!parking_file->Write ()) {
					Error ("Writing Parking Record %d", nparking);
				}	

				//---- add process link ----

				nprocess++;
				acc_id = mprocess + nprocess;

				access_file->ID (acc_id);
				access_file->From_ID (loc_id);
				access_file->From_Type ("ACTIVITY");
				access_file->To_ID (park_id);
				access_file->To_Type ("PARKING");
				access_file->Time (30.0);
				access_file->Cost (0.0);

				if (centroid_flag) {
					access_file->Notes ("Zone Connector");
				} else if (funcl == EXTERNAL) {
					access_file->Notes (ext_dir [j]);
				} else {
					access_file->Notes ("Parking Access");
				}

				if (!access_file->Write ()) goto access_error;
	
				nprocess++;
				acc_id = mprocess + nprocess;

				access_file->ID (acc_id);
				access_file->From_ID (park_id);
				access_file->From_Type ("PARKING");
				access_file->To_ID (loc_id);
				access_file->To_Type ("ACTIVITY");
				access_file->Time (30.0);
				access_file->Cost (0.0);

				if (centroid_flag) {
					access_file->Notes ("Zone Connector");
				} else if (funcl == EXTERNAL) {
					access_file->Notes (ext_dir [j]);
				} else {
					access_file->Notes ("Parking Access");
				}
				if (!access_file->Write ()) goto access_error;
			}
		}
	}
	End_Progress ();
	return;

shape_error:
	Error ("Writing Shape Record for Link %d", link);

access_error:
	Error ("Writing Process Link Record %d", nprocess);
}
