//*********************************************************
//	Read_Link.cpp - read the link file
//*********************************************************

#include "Network_Service.hpp"

#include "Link_File.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Link
//---------------------------------------------------------

void Network_Service::Read_Link (void)
{
	int num;
	Db_File *file = Network_Db_File (LINK);

	//---- store the link data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) { 
		if (!Network_Data_Flag (NODE) && 
			(Network_File_Flag (NODE) || Network_Option_Flag (NODE))) {
			Error (file_error, "Node", file->File_ID ());
		}
	}
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Link_Processing (file)) {
			if (!link_data.Add ()) {
				Error ("Adding Record to the Link Data List");
			}
			Link_Data *link_ptr = link_data.Record ();

			if (link_ptr->AB_Dir () > 0) {
				if (!dir_data.Add ()) goto dir_error;
				if (link_ptr->AB_Dir () != dir_data.Record_Index ()) {
					goto num_error;
				}
				if (AB_Key_Flag ()) {
					AB_Data ab;

					if (Renumber ()) {
						ab.anode = node_data [link_ptr->Anode ()]->Node ();
						ab.bnode = node_data [link_ptr->Bnode ()]->Node ();
					} else {
						ab.anode = link_ptr->Anode ();
						ab.bnode = link_ptr->Bnode ();
					}
					ab.dir = link_ptr->AB_Dir ();

					if (!ab_key.Add (&ab)) goto ab_error;
				}
			}
			if (link_ptr->BA_Dir () > 0) {
				if (!dir_data.Add ()) goto dir_error;
				if (link_ptr->BA_Dir () != dir_data.Record_Index ()) {
					goto num_error;
				}
				if (AB_Key_Flag ()) {
					AB_Data ab;

					if (Renumber ()) {
						ab.anode = node_data [link_ptr->Bnode ()]->Node ();
						ab.bnode = node_data [link_ptr->Anode ()]->Node ();
					} else {
						ab.anode = link_ptr->Bnode ();
						ab.bnode = link_ptr->Anode ();
					}
					ab.dir = link_ptr->BA_Dir ();

					if (!ab_key.Add (&ab)) goto ab_error;
				}
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	num = link_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Link Data Records = %d", num);
	}
	if (dir_data.Num_Records () > 0) {
		Print (1, "Number of Directional Links = %d", dir_data.Num_Records ());
	}
	link_data.Optimize ();
	dir_data.Optimize ();

	if (AB_Key_Flag ()) {
		ab_key.Optimize ();
	}
	if (num > 0) Network_Data_True (LINK);
	return;

dir_error:
	Error ("Adding Record to the Link Direction List");

num_error:
	Error ("Direction Numbering");

ab_error:
	Error ("Adding A->B Link Key");
	return;
}

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool Network_Service::Link_Processing (Db_File *fh)
{
	bool node_flag, shape_flag;
	int lvalue, link, dir, xa, ya, xb, yb, lane_cap, ab_in, ab_out;
	double dvalue, len, dx, dy;
	char *str_ptr;

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	Link_File *file = (Link_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	shape_flag = Network_Data_Flag (SHAPE);

	//---- reserve memory ----

	if (link_data.Max_Records () == 0) {
		if (!link_data.Max_Records (file->Num_Records ())) goto mem_error;

		if (!dir_data.Max_Records (2 * file->Num_Records ())) goto mem_error;

		if (AB_Key_Flag ()) {
			if (!ab_key.Max_Records (2 * file->Num_Records ())) goto mem_error;
		}
	}

	//---- read and save the data ----

	dx = dy = 0;

	link_ptr = link_data.New_Record ();
	if (link_ptr == NULL) goto mem_error;

	dir_ptr = dir_data.New_Record (true, 2);
	//dir_ptr = dir_data.New_Record (false, 2);
	if (dir_ptr == NULL) goto mem_error;

	dir = dir_data.Num_Records () + 1;

	lvalue = file->Link ();

	if (lvalue == 0) {
		lvalue = link_data.Num_Records () + 1;
	}
	link_ptr->Link (lvalue);

	link = Renumber () ? (link_data.Num_Records () + 1) : lvalue;

	lvalue = file->Anode ();

	if (lvalue == 0) return (false);

	if (node_flag) {
		node_ptr = node_data.Get (lvalue);

		if (node_ptr == NULL) goto node_error;

		link_ptr->Anode (Renumber () ? node_data.Record_Index () : lvalue);

		xa = node_ptr->X ();
		ya = node_ptr->Y ();
	} else {
		link_ptr->Anode (lvalue);
		xa = ya = 0;
	}

	lvalue = file->Bnode ();
	
	if (node_flag) {
		node_ptr = node_data.Get (lvalue);

		if (node_ptr == NULL) goto node_error;

		link_ptr->Bnode (Renumber () ? node_data.Record_Index () : lvalue);

		xb = node_ptr->X ();
		yb = node_ptr->Y ();
	} else {
		link_ptr->Bnode (lvalue);
		xb = yb = 0;
	}

	len = file->Length ();
	link_ptr->Length (Round (len));

	link_ptr->Aoffset (Round (file->Setback_A ()));
	link_ptr->Boffset (Round (file->Setback_B ()));

	if (link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset () < 0) {
		return (Check_Message ("Link %d Length %.2lf and Setbacks %.2lf, %.2lf are Incompatible",
			link_ptr->Link (), len, file->Setback_A (), file->Setback_B ()));
	}

	//---- facility type ----

	link_ptr->Type (Facility_Code (file->Type ()));

	if (link_ptr->Type () == FREEWAY) {
		lane_cap = 2000;
	} else if (link_ptr->Type () == EXPRESSWAY || link_ptr->Type () == PRINCIPAL) {
		lane_cap = 1400;
	} else {
		lane_cap = 800;
	}

	//---- use permission ----

	str_ptr = file->Use ();

	if (*str_ptr == '\0') {
		str_ptr = "ANY";
	}
	link_ptr->Use (Use_Code (str_ptr));

	//---- shape index ----

	if (shape_flag) {
		link_ptr->Shape (shape_data.Get_Index (link_ptr->Link ()));
	} else {
		link_ptr->Shape (0);
	}

	//---- calculate link bearings ----

	ab_in = file->Bearing_A ();
	ab_out = file->Bearing_B ();

	if (file->Bearing_Flag () && (ab_in != 0 || ab_out != 0)) {
		int num_points = compass.Num_Points ();

		//---- adjust the points to the user-specified resolution ----

		if (num_points != 360 && num_points > 0) {
			ab_in = (ab_in * num_points + 180) / 360;
			ab_out = (ab_out * num_points + 180) / 360;
		}

	} else {

		if (link_ptr->Shape () > 0 && 
			(Bearing_Offset () + MAX (link_ptr->Aoffset (), link_ptr->Boffset ())) < link_ptr->Length ()) {

			int i, num, offset, len1, len2, xy_len, x1, y1, x2, y2;

			Shape_Data *shape_ptr = shape_data [link_ptr->Shape ()];
			num = shape_ptr->Points ();

			//---- from the A node ---

			offset = Bearing_Offset () + link_ptr->Aoffset ();

			len1 = xy_len = 0;
			x1 = xa;
			y1 = ya;

			for (i=1; i <= num; i++) {
				shape_ptr->Point (i, &x2, &y2);

				dx = x2 - x1;
				dy = y2 - y1;

				xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
				len2 = len1 + xy_len;

				if (len2 > offset) break;

				len1 = len2;
				x1 = x2;
				y1 = y2;
			}
			if (i > num) {
				dx = xb - x1;
				dy = yb - y1;

				xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
				len2 = len1 + xy_len;
			}
			offset -= len1;
			if (xy_len == 0) xy_len = 1;

			dx = x1 + (dx * offset / xy_len) - xa;
			dy = y1 + (dy * offset / xy_len) - ya;

			ab_in = compass.Direction (dx, dy);
			
			//---- from the B node ----

			offset = Bearing_Offset () + link_ptr->Boffset ();

			len1 = xy_len = 0;
			x1 = xb;
			y1 = yb;

			for (i=num; i >= 1; i--) {
				shape_ptr->Point (i, &x2, &y2);

				dx = x2 - x1;
				dy = y2 - y1;

				xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
				len2 = len1 + xy_len;

				if (len2 > offset) break;

				len1 = len2;
				x1 = x2;
				y1 = y2;
			}
			if (i < 1) {
				dx = xa - x1;
				dy = ya - y1;

				xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
				len2 = len1 + xy_len;
			}
			offset -= len1;
			if (xy_len == 0) xy_len = 1;

			dx = x1 + (dx * offset / xy_len) - xb;
			dy = y1 + (dy * offset / xy_len) - yb;

			ab_out = compass.Direction (-dx, -dy);

		} else {
			dx = xb - xa;
			dy = yb - ya;

			ab_in = ab_out = compass.Direction (dx, dy);
		}
	}

	//---- A->B direction ----

	lvalue = file->Lanes_AB ();

	if (lvalue > 0) {
		link_ptr->AB_Dir (dir);

		dir_ptr->Link (link);
		dir_ptr->Dir (0);

		dir_ptr->Thru (lvalue);
		dir_ptr->Left (file->Left_AB ());
		dir_ptr->Right (file->Right_AB ());

		dvalue = file->Fspd_AB ();

		if (dvalue == 0.0) {
			dvalue = file->Speed_AB ();

			if (dvalue == 0.0) {
				dvalue = 15.0;
			}
		}
		dir_ptr->Time0 (Round (len / dvalue));

		dir_ptr->Speed (Round (file->Speed_AB ()));

		if (dir_ptr->Speed () == 0.0) {
			dir_ptr->Speed (Round (dvalue));
		}
		lvalue = file->Cap_AB ();

		if (lvalue == 0) lvalue = dir_ptr->Thru () * lane_cap;

		dir_ptr->Capacity (lvalue);

		dir_ptr->In_Bearing (ab_in);
		dir_ptr->Out_Bearing (ab_out);

		dir_ptr->Cost_List (0);
		dir_ptr->TOD_List (0);

		dir_ptr = (Dir_Data *) (((char *) dir_ptr) + dir_data.Data_Size ());
		dir++;
	} else {
		link_ptr->AB_Dir (0);
	}

	//---- B->A direction ----

	lvalue = file->Lanes_BA ();

	if (lvalue > 0) {
		link_ptr->BA_Dir (dir);

		dir_ptr->Link (link);
		dir_ptr->Dir (1);

		dir_ptr->Thru (lvalue);
		dir_ptr->Left (file->Left_BA ());
		dir_ptr->Right (file->Right_BA ());

		dvalue = file->Fspd_BA ();

		if (dvalue == 0.0) {
			dvalue = file->Speed_BA ();

			if (dvalue == 0.0) {
				dvalue = 15.0;
			}
		}
		dir_ptr->Time0 (Round (len / dvalue));

		dir_ptr->Speed (Round (file->Speed_BA ()));

		if (dir_ptr->Speed () == 0.0) {
			dir_ptr->Speed (Round (dvalue));
		}
		lvalue = file->Cap_BA ();

		if (lvalue == 0) lvalue = dir_ptr->Thru () * lane_cap;

		dir_ptr->Capacity (lvalue);

		dir_ptr->In_Bearing (compass.Flip (ab_out));
		dir_ptr->Out_Bearing (compass.Flip (ab_in));

		dir_ptr->TOD_List (0);
		dir_ptr->Cost_List (0);

	} else {
		link_ptr->BA_Dir (0);
	}
	return ((link_ptr->AB_Dir () > 0 || link_ptr->BA_Dir () > 0));

node_error:
	return (Check_Message ("Link %d Node %d was Not Found in the Node file", link_ptr->Link (), lvalue));

mem_error:
	Error ("Insuffienct Memory for Link Data");
	return (false);
}
