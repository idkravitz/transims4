//*********************************************************
//	Toll_Process.cpp - additional toll processing
//*********************************************************

#include "ArcNet.hpp"

#include "Toll_File.hpp"

//---------------------------------------------------------
//	Toll_Processing
//---------------------------------------------------------

bool ArcNet::Toll_Processing (Db_File *fh)
{
	int i, dir, center, lane, num_lanes;
	double length, offset, start, end, side;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Toll_Data *toll_ptr;

	Toll_File *file = (Toll_File *) fh;

	if (Network_Service::Toll_Processing (file)) {
		arcview_toll.Copy_Fields (file);

		toll_ptr = toll_data.New_Record ();

		if (select_time) {
			if (!time_range.Span_Range (toll_ptr->Start (), toll_ptr->End ())) return (false);
		}
		link_ptr = link_data.Get (toll_ptr->Link ());
		dir = toll_ptr->Dir ();

		start = offset = 0.0;
		length = UnRound (link_ptr->Length ());
		end = start + length;

		if (dir == 0) {
			side = UnRound (link_ptr->Aoffset ());
			if (start < side) start = side;

			side = UnRound (link_ptr->Length () - link_ptr->Boffset ());
			if (end > side) end = side;

			length = end - start;
			offset = start;
		} else {
			side = UnRound (link_ptr->Boffset ());
			if (start < side) start = side;

			side = UnRound (link_ptr->Length () - link_ptr->Aoffset ());
			if (end > side) end = side;

			length = end - start;

			offset = UnRound (link_ptr->Length ()) - start;
		}

		if (lanes_flag) {
			if (center_flag) {
				if (link_ptr->BA_Dir () == 0) {
					dir_ptr = dir_data [link_ptr->AB_Dir ()];
					center = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right () + 1;
				} else if (link_ptr->AB_Dir () == 0) {
					dir_ptr = dir_data [link_ptr->BA_Dir ()];
					center = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right () + 1;
				} else {
					center = 1;
				}
			} else {
				center = 1;
			}

			if (dir == 0) {
				dir_ptr = dir_data [link_ptr->AB_Dir ()];
			} else {
				dir_ptr = dir_data [link_ptr->BA_Dir ()];
			}
			lane = dir_ptr->Left ();
			num_lanes = lane + dir_ptr->Thru ();

			arcview_toll.parts.Reset ();
			arcview_toll.points.Reset ();

			for (++lane; lane <= num_lanes; lane++) {
				side = (2 * lane - center) * lane_width / 2.0;

				Link_Shape (link_ptr, dir, &points, offset, length, side);

				if (!arcview_toll.parts.Add (arcview_toll.points.Num_Points ())) goto mem_error;

				for (i=1; i <= points.Num_Points (); i++) {
					if (!arcview_toll.points.Add (points [i])) goto mem_error;
				}
			}

		} else {

			Link_Shape (link_ptr, dir, &arcview_lane_use.points, offset, length, 0.0);

		}
		if (!arcview_toll.Write_Record ()) {
			Error ("Writing ArcView Toll File");
		}
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Toll Shape Points");
	return (false);
}

