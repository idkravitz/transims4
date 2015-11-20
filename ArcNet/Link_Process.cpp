//*********************************************************
//	Link_Process.cpp - additonal link processing
//*********************************************************

#include "ArcNet.hpp"

#include "Link_File.hpp"

#include <math.h>

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool ArcNet::Link_Processing (Db_File *fh)
{
	static bool offset_flag;

	int i, lane, center, num_lanes;
	double side, start, length;

	Link_Data *link_ptr;

	Link_File *file = (Link_File *) fh;

	if (First_Record ()) {
		offset_flag = (link_offset != 0.0);
	}

	if (Network_Service::Link_Processing (file)) {
		arcview_link.Copy_Fields (file);

		arcview_link.parts.Reset ();
		arcview_link.points.Reset ();

		link_ptr = link_data.New_Record ();

		if (lanes_flag) {

			//---- draw lanes in AB direction ----

			if (link_ptr->AB_Dir () > 0) {
				start = UnRound (link_ptr->Aoffset ());
				length = UnRound (link_ptr->Length ());
				length -= start + UnRound (link_ptr->Boffset ());

				if (guideway_flag) {
					lane = 0;
					num_lanes = file->Left_AB () + file->Lanes_AB () + file->Right_AB ();

					guideway_file.Put_Field (1, link_ptr->Link ());
					guideway_file.Put_Field (2, "B");
				} else {
					lane = file->Left_AB ();
					num_lanes = file->Lanes_AB () + lane;
				}
				if (center_flag && link_ptr->BA_Dir () == 0) {
					if (guideway_flag) {
						center = num_lanes + 1;
					} else {
						center = num_lanes + file->Right_AB () + 1;
					}
				} else {
					center = 1;
				}
				for (++lane; lane <= num_lanes; lane++) {
					side = (2 * lane - center) * lane_width / 2.0;

					Link_Shape (link_ptr, 0, &points, start, length, side);

					if (guideway_flag) {
						guideway_file.points.Reset ();

						for (i=1; i <= points.Num_Points (); i++) {
							if (!guideway_file.points.Add (points [i])) goto mem_error;
						}
						if (!guideway_file.Write_Record ()) {
							Error ("Writing Visalizer Guideway File");
						}
						if (lane <= file->Left_AB () || (lane > file->Left_AB () + file->Lanes_AB ())) continue;
					}
					if (!arcview_link.parts.Add (arcview_link.points.Num_Points ())) goto mem_error;

					if (arrow_flag) Add_Arrow (&points);

					for (i=1; i <= points.Num_Points (); i++) {
						if (!arcview_link.points.Add (points [i])) goto mem_error;
					}
				}
			} 

			//---- draw lanes in BA direction ----

			if (link_ptr->BA_Dir () > 0) {
				length = UnRound (link_ptr->Length ());
				start = length - UnRound (link_ptr->Boffset ());
				length = start - UnRound (link_ptr->Aoffset ());

				if (guideway_flag) {
					lane = 0;
					num_lanes = file->Left_BA () + file->Lanes_BA () + file->Right_BA ();

					guideway_file.Put_Field (1, link_ptr->Link ());
					guideway_file.Put_Field (2, "A");
				} else {
					lane = file->Left_BA ();
					num_lanes = file->Lanes_BA () + lane;
				}

				if (center_flag && link_ptr->AB_Dir () == 0) {
					if (guideway_flag) {
						center = num_lanes + 1;
					} else {
						center = num_lanes + file->Right_BA () + 1;
					}
				} else {
					center = 1;
				}
				for (++lane; lane <= num_lanes; lane++) {
					side = (2 * lane - center) * lane_width / 2.0;

					Link_Shape (link_ptr, 1, &points, start, length, side);

					if (guideway_flag) {
						guideway_file.points.Reset ();

						for (i=1; i <= points.Num_Points (); i++) {
							if (!guideway_file.points.Add (points [i])) goto mem_error;
						}
						if (!guideway_file.Write_Record ()) {
							Error ("Writing Visalizer Guideway File");
						}
						if (lane <= file->Left_BA () || (lane > file->Left_BA () + file->Lanes_BA ())) continue;
					}
					if (!arcview_link.parts.Add (arcview_link.points.Num_Points ())) goto mem_error;

					if (arrow_flag) Add_Arrow (&points);

					for (i=1; i <= points.Num_Points (); i++) {
						if (!arcview_link.points.Add (points [i])) goto mem_error;
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

					if (!arcview_link.parts.Add (0)) goto mem_error;

					if (arrow_flag) Add_Arrow (&points);

					for (i=1; i <= points.Num_Points (); i++) {
						if (!arcview_link.points.Add (points [i])) goto mem_error;
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

					if (!arcview_link.parts.Add (arcview_link.points.Num_Points ())) goto mem_error;

					if (arrow_flag) Add_Arrow (&points);

					for (i=1; i <= points.Num_Points (); i++) {
						if (!arcview_link.points.Add (points [i])) goto mem_error;
					}
				}

			} else {
				
				//---- draw centerline -----

				if (!arcview_link.parts.Add (0)) goto mem_error;

				if (direction_flag) {
					Link_Shape (link_ptr, 0, &arcview_link.points);

					if (arrow_flag) Add_Arrow (&arcview_link.points);

				} else if (link_ptr->AB_Dir () == 0) {
					Link_Shape (link_ptr, 1, &arcview_link.points);

					if (arrow_flag) Add_Arrow (&arcview_link.points);

				} else {
					Link_Shape (link_ptr, 0, &arcview_link.points);

					if (arrow_flag && link_ptr->BA_Dir () == 0) {
						Add_Arrow (&arcview_link.points);
					}
				}
			}
		}
		if (!arcview_link.Write_Record ()) {
			Error ("Writing ArcView Link File");
		}
		return (true);
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Link Shape Points");
	return (false);
}

//		link_rec.length = length * length_units;
//
//		if (link_rec.length == 0.0) {
//			link_rec.length = 1.0;
//		}

