//*********************************************************
//	Read_Link_Dir.cpp - read the link direction file
//*********************************************************

#include "ArcDelay.hpp"

#include "In_Polygon.hpp"
#include "Smooth_Shape.hpp"

//---------------------------------------------------------
//	Read_Link_Dir
//---------------------------------------------------------

void ArcDelay::Read_Link_Dir (void)
{
	int i, center, link, dir, lane, num_lanes, num_rec;
	double length, side, start, width, offset;
	bool offset_flag;

	XYZ_Point point, *ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Node_Data *node_ptr;

	offset_flag = (link_offset != 0.0);

	Show_Message ("Reading %s -- Record", arcview_link_dir.File_Type ());
	Set_Progress (1000);
	num_rec = 0;

	//---- process each link ----

	while (link_dir_file.Read ()) {
		Show_Progress ();

		//---- get the link direction ----

		link = link_dir_file.Link ();

		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) continue;

		dir = link_dir_file.Dir ();

		if (dir == 1) {
			dir_ptr = dir_data [link_ptr->BA_Dir ()];
		} else {
			dir_ptr = dir_data [link_ptr->AB_Dir ()];
		}
		if (dir_ptr == NULL) continue;

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

		arcview_link_dir.Copy_Fields (&link_dir_file);

		if (index_flag) {
			arcview_link_dir.Put_Field (1, link * 2 + dir);
		}

		arcview_link_dir.parts.Reset ();
		arcview_link_dir.points.Reset ();

		//---- bandwidth processing ----

		if (width_flag) {
			arcview_link_dir.Get_Field (width_field, &width);
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

			if (dir == 0) {
				offset = 0.0;
			} else {
				offset = length;
			}

			//---- get the centerline points ----

			Link_Shape (link_ptr, dir, &points, offset, length, 0.0);

			ptr = points.First ();
			point = *ptr;
					
			if (!arcview_link_dir.parts.Add (0)) goto mem_error;

			for (; ptr; ptr = points.Next ()) {
				if (!arcview_link_dir.points.Add (ptr)) goto point_error;
			}

			//---- get the outside points of the band ----

			offset = length - offset;
			dir = 1 - dir;

			Link_Shape (link_ptr, dir, &points, offset, length, width);

			if (max_angle > 0 && min_length > 0) {
				Smooth_Shape (&points, max_angle, min_length);
			}

			for (ptr = points.First (); ptr; ptr = points.Next ()) {
				if (!arcview_link_dir.points.Add (ptr)) goto point_error;
			}

			//---- close the polygon ----

			if (!arcview_link_dir.points.Add (&point)) goto point_error;

		} else {

			//---- find the center point ----

			if (center_flag) {
				if (link_ptr->BA_Dir () == 0 || link_ptr->AB_Dir () == 0) {
					center = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right () + 1;
				} else {
					center = 1;
				}
			} else {
				center = 1;
			}

			//---- draw lanes ----

			if (lanes_flag) {
				length = UnRound (link_ptr->Length ());

				if (dir == 0) {
					start = UnRound (link_ptr->Aoffset ());
					length -= start + UnRound (link_ptr->Boffset ());
				} else {
					start = length - UnRound (link_ptr->Boffset ());
					length = start - UnRound (link_ptr->Aoffset ());
				}
				lane = dir_ptr->Left ();
				num_lanes = dir_ptr->Thru () + lane;

				for (++lane; lane <= num_lanes; lane++) {
					side = (2 * lane - center) * lane_width / 2.0;

					Link_Shape (link_ptr, dir, &points, start, length, side);

					if (!arcview_link_dir.parts.Add (arcview_link_dir.points.Num_Points ())) goto mem_error;

					for (i=1; i <= points.Num_Points (); i++) {
						if (!arcview_link_dir.points.Add (points [i])) goto mem_error;
					}
				}

			} else {

				if (offset_flag) {
					start = (dir == 0) ? 0.0 : -1.0;

					if (center > 1) {
						side = 0.0;
					} else {
						side = link_offset;
					}
					Link_Shape (link_ptr, dir, &points, start, -1.0, side);

					if (!arcview_link_dir.parts.Add (0)) goto mem_error;

					for (i=1; i <= points.Num_Points (); i++) {
						if (!arcview_link_dir.points.Add (points [i])) goto mem_error;
					}

				} else {
					
					//---- draw centerline -----

					if (!arcview_link_dir.parts.Add (0)) goto mem_error;

					Link_Shape (link_ptr, 0, &arcview_link_dir.points);
				}
			}
		}

		//---- save the link record ----

		if (!arcview_link_dir.Write_Record ()) {
			Error ("Writing %s", arcview_link_dir.File_Type ());
		}
		num_rec++;
	}
	End_Progress ();
	arcview_link_dir.Close ();

	Print (2, "Number of Arcview Link Direction Records = %d", num_rec);
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_link_dir.points.Num_Points ());

mem_error:
	Error ("Insufficient Memory for Link Shape Points");
}
