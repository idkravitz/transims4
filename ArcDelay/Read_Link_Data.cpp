//*********************************************************
//	Read_Link_Data.cpp - read the link data file
//*********************************************************

#include "ArcDelay.hpp"

#include "In_Polygon.hpp"
#include "Smooth_Shape.hpp"

//---------------------------------------------------------
//	Read_Link_Data
//---------------------------------------------------------

void ArcDelay::Read_Link_Data (void)
{
	int i, center, link, lane, num_lanes, num_rec;
	double length, side, start, width;
	bool offset_flag;

	XYZ_Point point, *ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Dir_Data *dir_ptr;

	offset_flag = (link_offset != 0.0);

	Show_Message ("Reading %s -- Record", arcview_link_data.File_Type ());
	Set_Progress (1000);
	num_rec = 0;

	//---- process each link ----

	while (link_data_file.Read ()) {
		Show_Progress ();

		//---- get the link ----

		link = link_data_file.Link ();

		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) continue;

		//---- check the subarea polygon ----

		if (subarea_flag) {
			node_ptr = node_data.Get (link_ptr->Anode ());

			if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
				node_ptr = node_data.Get (link_ptr->Bnode ());

				if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
					continue;
				}
			}
		}

		//---- copy the data fields ----

		arcview_link_data.Copy_Fields (&link_data_file);

		arcview_link_data.parts.Reset ();
		arcview_link_data.points.Reset ();

		//---- bandwidth processing ----

		if (width_flag) {
			arcview_link_data.Get_Field (width_field, &width);
			if (width == 0.0) continue;

			if (width > 0) {
				width = width / width_factor;
			} else {
				width = -width / width_factor;
			}
			if (width < min_width) {
				width = min_width;
			} else if (width > max_width) {
				width = max_width;
			}
			width = -width;

			length = UnRound (link_ptr->Length ());

			//---- get the centerline points ----

			Link_Shape (link_ptr, 0, &points, 0.0, length, 0.0);

			ptr = points.First ();
			point = *ptr;
					
			if (!arcview_link_data.parts.Add (0)) goto mem_error;

			for (; ptr; ptr = points.Next ()) {
				if (!arcview_link_data.points.Add (ptr)) goto point_error;
			}

			//---- get the outside points of the band ----

			Link_Shape (link_ptr, 1, &points, length, length, width);

			if (max_angle > 0 && min_length > 0) {
				Smooth_Shape (&points, max_angle, min_length);
			}

			for (ptr = points.First (); ptr; ptr = points.Next ()) {
				if (!arcview_link_data.points.Add (ptr)) goto point_error;
			}

			//---- close the polygon ----

			if (!arcview_link_data.points.Add (&point)) goto point_error;

		} else {

			//---- draw lanes ----

			if (lanes_flag) {
				if (link_ptr->AB_Dir () > 0) {
					dir_ptr = dir_data [link_ptr->AB_Dir ()];
					if (dir_ptr == NULL) continue;

					length = UnRound (link_ptr->Length ());
					start = UnRound (link_ptr->Aoffset ());
					length -= start + UnRound (link_ptr->Boffset ());

					lane = dir_ptr->Left ();
					num_lanes = dir_ptr->Thru () + lane;

					//---- find the center point ----

					if (center_flag && link_ptr->BA_Dir () == 0) {
						center = num_lanes + dir_ptr->Right () + 1;
					} else {
						center = 1;
					}

					for (++lane; lane <= num_lanes; lane++) {
						side = (2 * lane - center) * lane_width / 2.0;

						Link_Shape (link_ptr, 0, &points, start, length, side);

						if (!arcview_link_data.parts.Add (arcview_link_data.points.Num_Points ())) goto mem_error;

						for (i=1; i <= points.Num_Points (); i++) {
							if (!arcview_link_data.points.Add (points [i])) goto mem_error;
						}
					}
				}
				if (link_ptr->BA_Dir () > 0) {
					dir_ptr = dir_data [link_ptr->BA_Dir ()];
					if (dir_ptr == NULL) continue;

					length = UnRound (link_ptr->Length ());
					start = length - UnRound (link_ptr->Boffset ());
					length = start - UnRound (link_ptr->Aoffset ());

					lane = dir_ptr->Left ();
					num_lanes = dir_ptr->Thru () + lane;

					//---- find the center point ----

					if (center_flag && link_ptr->AB_Dir () == 0) {
						center = num_lanes + dir_ptr->Right () + 1;
					} else {
						center = 1;
					}

					for (++lane; lane <= num_lanes; lane++) {
						side = (2 * lane - center) * lane_width / 2.0;

						Link_Shape (link_ptr, 1, &points, start, length, side);

						if (!arcview_link_data.parts.Add (arcview_link_data.points.Num_Points ())) goto mem_error;

						for (i=1; i <= points.Num_Points (); i++) {
							if (!arcview_link_data.points.Add (points [i])) goto mem_error;
						}
					}
				}

			} else {

				if (offset_flag) {

					//---- draw link in AB direction ----

					if (link_ptr->AB_Dir () > 0) {
						if (center_flag && link_ptr->BA_Dir () == 0) {
							side = 0.0;
						} else {
							side = link_offset;
						}
						Link_Shape (link_ptr, 0, &points, 0.0, -1.0, side);

						if (!arcview_link_data.parts.Add (0)) goto mem_error;

						for (i=1; i <= points.Num_Points (); i++) {
							if (!arcview_link_data.points.Add (points [i])) goto mem_error;
						}
					}

					//---- draw link in BA direciton ----

					if (link_ptr->BA_Dir () > 0) {
						if (center_flag && link_ptr->AB_Dir () == 0) {
							side = 0.0;
						} else {
							side = link_offset;
						}
						Link_Shape (link_ptr, 1, &points, -1.0, -1.0, side);

						if (!arcview_link_data.parts.Add (arcview_link_data.points.Num_Points ())) goto mem_error;

						for (i=1; i <= points.Num_Points (); i++) {
							if (!arcview_link_data.points.Add (points [i])) goto mem_error;
						}
					}

				} else {
					
					//---- draw centerline -----

					if (!arcview_link_data.parts.Add (0)) goto mem_error;

					Link_Shape (link_ptr, 0, &arcview_link_data.points);
				}
			}
		}

		//---- save the link record ----

		if (!arcview_link_data.Write_Record ()) {
			Error ("Writing %s", arcview_link_data.File_Type ());
		}
		num_rec++;
	}
	End_Progress ();
	arcview_link_data.Close ();

	Print (2, "Number of Arcview Link Data Records = %d", num_rec);
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_link_data.points.Num_Points ());

mem_error:
	Error ("Insufficient Memory for Link Shape Points");
}
