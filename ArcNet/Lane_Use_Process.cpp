//*********************************************************
//	Lane_Use_Process.cpp - additional lane use processing
//*********************************************************

#include "ArcNet.hpp"

#include "Lane_Use_File.hpp"

//---------------------------------------------------------
//	Lane_Use_Processing
//---------------------------------------------------------

bool ArcNet::Lane_Use_Processing (Db_File *fh)
{
	int i, dir, center, lane, num_lanes;
	double length, offset, start, end, side;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Lane_Use_Data *lane_use_ptr;

	Lane_Use_File *file = (Lane_Use_File *) fh;

	if (Network_Service::Lane_Use_Processing (file)) {
		arcview_lane_use.Copy_Fields (file);

		lane_use_ptr = lane_use_data.New_Record ();

		if (select_time) {
			if (!time_range.Span_Range (lane_use_ptr->Start (), lane_use_ptr->End ())) return (false);
		}

		link_ptr = link_data.Get (lane_use_ptr->Link ());
		dir = lane_use_ptr->Dir ();

		offset = UnRound (lane_use_ptr->Offset ());
		length = UnRound (lane_use_ptr->Length ());

		if (offset == 0.0 && length == 0.0) {
			offset = 0.0;
			length = UnRound (link_ptr->Length ());
		}
		start = offset;
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
			lane = lane_use_ptr->Lane ();

			if (lane == 0) {
				if (dir == 0) {
					dir_ptr = dir_data [link_ptr->AB_Dir ()];
				} else {
					dir_ptr = dir_data [link_ptr->BA_Dir ()];
				}
				lane = dir_ptr->Left ();
				num_lanes = lane + dir_ptr->Thru ();
			
				arcview_lane_use.parts.Reset ();
				arcview_lane_use.points.Reset ();

				for (++lane; lane <= num_lanes; lane++) {
					side = (2 * lane - center) * lane_width / 2.0;

					Link_Shape (link_ptr, dir, &points, offset, length, side);

					if (!arcview_lane_use.parts.Add (arcview_lane_use.points.Num_Points ())) goto mem_error;

					for (i=1; i <= points.Num_Points (); i++) {
						if (!arcview_lane_use.points.Add (points [i])) goto mem_error;
					}
				}
				if (!arcview_lane_use.Write_Record ()) goto write_error;
				return (false);

			} else {
				arcview_lane_use.Num_Parts (1);

				side = (2 * lane - center) * lane_width / 2.0;
			}
		} else {
			side = 0.0;
		}
		if (Link_Shape (link_ptr, dir, &arcview_lane_use.points, offset, length, side)) {

			if (!arcview_lane_use.Write_Record ()) goto write_error;
		}
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Lane Use Shape Points");
	return (false);

write_error:
	Error ("Writing ArcView Lane Use File");
	return (false);
}

