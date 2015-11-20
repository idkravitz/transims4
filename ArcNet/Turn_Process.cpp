//*********************************************************
//	Turn_Process.cpp - read the turn prohibition file
//*********************************************************

#include "ArcNet.hpp"

#include "Turn_File.hpp"

//---------------------------------------------------------
//	Turn_Processing
//---------------------------------------------------------

bool ArcNet::Turn_Processing (Db_File *fh)
{
	int i;
	double length, setback;

	Link_Data *link_ptr;
	Turn_Data *turn_ptr;

	Turn_File *file = (Turn_File *) fh;

	if (Network_Service::Turn_Processing (file)) {

		arcview_turn.Copy_Fields (file);
		arcview_turn.points.Reset ();

		turn_ptr = turn_data.New_Record ();

		if (select_time) {
			if (!time_range.Span_Range (turn_ptr->Start (), turn_ptr->End ())) return (false);
		}

		//---- approach leg ----

		link_ptr = link_data.Get (turn_ptr->In_Link ());

		if (turn_ptr->In_Dir () == 0) {
			length = UnRound (link_ptr->Length ());
			setback = UnRound (link_ptr->Boffset ());

			Link_Shape (link_ptr, 0, &points, length - setback - 10.0, 10.0, lane_width);
		} else {
			setback = UnRound (link_ptr->Aoffset ());

			Link_Shape (link_ptr, 1, &points, setback + 10.0, 10.0, lane_width);
		}
		for (i=1; i <= points.Num_Points (); i++) {
			if (!arcview_turn.points.Add (points [i])) goto mem_error;
		}

		//---- departure leg ----

		link_ptr = link_data.Get (turn_ptr->Out_Link ());

		if (turn_ptr->Out_Dir () == 0) {
			setback = UnRound (link_ptr->Aoffset ());

			Link_Shape (link_ptr, 0, &points, setback, 10.0, lane_width);
		} else {
			length = UnRound (link_ptr->Length ());
			setback = UnRound (link_ptr->Boffset ());

			Link_Shape (link_ptr, 1, &points, length - setback, 10.0, lane_width);
		}
		for (i=1; i <= points.Num_Points (); i++) {
			if (!arcview_turn.points.Add (points [i])) goto mem_error;
		}
		if (!arcview_turn.Write_Record ()) {
			Error ("Writing ArcView Turn Prohibition File");
		}
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Turn Prohibition Shape Points");
	return (false);
}
