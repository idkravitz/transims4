//*********************************************************
//	Read_Link.cpp - convert the gis link data
//*********************************************************

#include "GISNet.hpp"

#include "Point_Array.hpp"
#include "Smooth_Shape.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Link
//---------------------------------------------------------

void GISNet::Read_Link (void)
{
	int i, link, node, num, x, y, z, num_flip;
	double length, dx, dy, dz, x1, y1, x2, y2, z1, z2;
	XYZ_Point *pt_ptr;

	Node_Data *node_ptr;
	Point_Data points;

	points.Z_Flag (gis_file.Z_Flag ());

	//---- read each gis link ----

	Show_Message ("Reading %s -- Record", gis_file.File_Type ());
	Set_Progress (1000);
	num_flip = 0;

	while (gis_file.Read_Record ()) {
		Show_Progress ();
		
		if (gis_file.Num_Points () < 2) continue;

		//---- copy fields and execute user program ----

		link_file->Link (0);
		link_file->Anode (0);
		link_file->Bnode (0);
		link_file->Length (0.0);

		link_file->Copy_Fields (&gis_file);

		if (convert_flag) {
			if (!convert.Execute ()) continue;
		}

		if (link_file->Link () == 0) {
			link_file->Link (first_link++);
		} else if (link_file->Link () >= first_link) {
			first_link = link_file->Link () + 1;
		}

		//---- copy points into a point array ----

		num_in += num = gis_file.Num_Points ();
		points.Reset ();
		points.Max_Points (num);

		for (pt_ptr = gis_file.points.First (); pt_ptr; pt_ptr = gis_file.points.Next ()) {
			pt_ptr->x = UnRound (Round (pt_ptr->x));
			pt_ptr->y = UnRound (Round (pt_ptr->y));
			pt_ptr->z = UnRound (Round (pt_ptr->z));

			points.Add (pt_ptr);
		}

		//---- get the anode coordinates ----

		pt_ptr = points.First ();

		x = Round (pt_ptr->x);
		y = Round (pt_ptr->y);
		z = Round (pt_ptr->z);

		node = link_file->Anode ();

		if (node == 0) {
			for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
				if (abs (node_ptr->X () - x) <= diff && abs (node_ptr->Y () - y) <= diff &&
					abs (node_ptr->Z () - z) <= diff) {
					break;
				}
			}
			if (node_ptr == NULL) {
				node = first_node++;
			} else {
				node = node_ptr->Node ();
			}
			link_file->Anode (node);
		} else {
			if (node >= first_node) first_node = node + 1;
		}
		node_ptr = node_data.Get (node);

		if (node_ptr == NULL) {
			if (node_flag) {
				Warning ("Node %d was Not Found in the Node File", node);
			}
			node_ptr = node_data.New_Record (true);

			node_ptr->Node (node);
			node_ptr->X (x);
			node_ptr->Y (y);
			node_ptr->Z (z);

			if (!node_data.Add ()) goto node_error;
			nnode++;
		} else {
			if (abs (node_ptr->X () - x) > diff || abs (node_ptr->Y () - y) > diff) {

				//---- consider flipping the points ----

				pt_ptr = points.Last ();

				x = Round (pt_ptr->x);
				y = Round (pt_ptr->y);
				z = Round (pt_ptr->z);

				if (abs (node_ptr->X () - x) > diff || abs (node_ptr->Y () - y) > diff) {
					Warning ("Node %d is at a Different Coordinate Location", node);
				} else {
					points.Flip_Points ();
					num_flip++;
				}
				pt_ptr = points.First ();
			}
			pt_ptr->x = UnRound (node_ptr->X ());
			pt_ptr->y = UnRound (node_ptr->Y ());
			pt_ptr->z = UnRound (node_ptr->Z ());

			points.Set ();
		}

		//--- set the bnode point ----

		pt_ptr = points.Last ();

		x = Round (pt_ptr->x);
		y = Round (pt_ptr->y);
		z = Round (pt_ptr->z);

		node = link_file->Bnode ();

		if (node == 0) {
			for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
				if (abs (node_ptr->X () - x) <= diff && abs (node_ptr->Y () - y) <= diff &&
					abs (node_ptr->Z () - z) <= diff) {
					break;
				}
			}
			if (node_ptr == NULL) {
				node = first_node++;
			} else {
				node = node_ptr->Node ();
			}
			link_file->Bnode (node);
		} else {
			if (node >= first_node) first_node = node + 1;
		}
		node_ptr = node_data.Get (node);

		if (node_ptr == NULL) {
			if (node_flag) {
				Warning ("Node %d was Not Found in the Node File", node);
			}
			node_ptr = node_data.New_Record (true);

			node_ptr->Node (node);
			node_ptr->X (x);
			node_ptr->Y (y);
			node_ptr->Z (z);

			if (!node_data.Add ()) goto node_error;
			nnode++;
		} else {
			if (abs (node_ptr->X () - x) > diff || abs (node_ptr->Y () - y) > diff) {
				Warning ("Node %d is at a Different Coordinate Location", node);
			}
			pt_ptr->x = UnRound (node_ptr->X ());
			pt_ptr->y = UnRound (node_ptr->Y ());
			pt_ptr->z = UnRound (node_ptr->Z ());

			points.Set ();
		}

		//---- smooth the link shape -----

		if (max_angle > 0 && min_length > 0) {
			Smooth_Shape (&points, max_angle, min_length);
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
						link = link_file->Link ();

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

		//---- save the link data ----

		if (update_flag || link_file->Length () == 0.0) {
			link_file->Length (UnRound (Round (length)));
		}
		if (!link_file->Write ()) {
			Error ("Writing Network Link File");
		}
		nlink++;
	}
	End_Progress ();
	gis_file.Close ();

	Print (2, "Number of %s Records = %d", gis_file.File_Type (), Progress_Count ());
	Print (1, "Number of Input Shape Points = %d", num_in);

	Break_Check (5);
	link_file->Close ();
	Write (2, "Number of %s Records = %d", link_file->File_Type (), nlink);

	//---- save the node records ----

	nnode = 0;

	for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
		node_file->Node (node_ptr->Node ());
		node_file->X (UnRound (node_ptr->X ()));
		node_file->Y (UnRound (node_ptr->Y ()));
		node_file->Z (UnRound (node_ptr->Z ()));

		if (!node_file->Write ()) {
			Error ("Writing Node File");
		}
		nnode++;
	}
	node_file->Close ();
	Write (1, "Number of %s Records = %d", node_file->File_Type (), nnode);

	if (shape_file != NULL) {
		shape_file->Close ();
		Write (1, "Number of %s Records = %d", shape_file->File_Type (), nrec);
		Write (1, "Number of Links with Shape Points = %d", nshape);
	}
	if (num_flip > 0) {
		Write (1, "Number of Reoriented Link Shapes = %d", num_flip);
	}
	return;

node_error:
	Error ("Adding Node Record");

shape_error:
	Error ("Writing Shape File");
}
