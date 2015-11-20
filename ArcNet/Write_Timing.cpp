//*********************************************************
//	Write_Timing - write a timing plan file
//*********************************************************

#include "ArcNet.hpp"

#include "RoundedConnectivity.hpp"

//---------------------------------------------------------
//	Write_Timing
//---------------------------------------------------------

void ArcNet::Write_Timing (void)
{
	int i, timing_field, phase_field, timing, phase;
	double length, setback;

	Link_Data *link_ptr;
	Phasing_Data *phasing_ptr;
	
	Show_Message ("Writing Timing Plan -- Record");
	Set_Progress (1000);

	timing_field = timing_db.Required_Field ("TIMING", "PLAN");
	phase_field = timing_db.Required_Field ("PHASE");

	timing_db.Rewind ();

	while (timing_db.Read_Record ()) {
		Show_Progress ();

		arcview_timing.Copy_Fields (&timing_db);

		timing_db.Get_Field (timing_field, &timing);
		timing_db.Get_Field (phase_field, &phase);

		if (select_time) {
			if (timing_list.Get_Index (timing) == 0) continue;
		}
		arcview_timing.parts.Reset ();
		arcview_timing.points.Reset ();

		for (phasing_ptr = phasing_data.First (); phasing_ptr; phasing_ptr = phasing_data.Next ()) {

			if (phasing_ptr->Timing () != timing || phasing_ptr->Phase () != phase) continue;
					
			if (!arcview_timing.parts.Add (arcview_timing.points.Num_Points ())) goto mem_error;

			//---- approach leg ----

			link_ptr = link_data.Get (phasing_ptr->In_Link ());

			if (phasing_ptr->In_Dir () == 0) {
				length = UnRound (link_ptr->Length ());
				setback = UnRound (link_ptr->Boffset ());
				if ( rounded_connectivity && setback_override_flag ) setback = setback_distance;

				Link_Shape (link_ptr, 0, &points, length - setback - 10.0, 10.0, lane_width);
			} else {
				setback = UnRound (link_ptr->Aoffset ());
				if ( rounded_connectivity && setback_override_flag ) setback = setback_distance;

				Link_Shape (link_ptr, 1, &points, setback + 10.0, 10.0, lane_width);
			}
			for (i=1; i <= points.Num_Points () - 1 ; i++) {
				if (!arcview_timing.points.Add (points [i])) goto mem_error;
			}

			// collect the last two incoming points for intersection interpolation
			int last_pt = points.Num_Points ();
			double x1 = points[last_pt-1]->x;
			double y1 = points[last_pt-1]->y;
			double x2 = points[last_pt]->x;
			double y2 = points[last_pt]->y;

			//---- departure leg ----

			link_ptr = link_data.Get (phasing_ptr->Out_Link ());

			if (phasing_ptr->Out_Dir () == 0) {
				setback = UnRound (link_ptr->Aoffset ());
				if ( rounded_connectivity && setback_override_flag ) setback = setback_distance;

				Link_Shape (link_ptr, 0, &points, setback, 10.0, lane_width);
			} else {
				length = UnRound (link_ptr->Length ());
				setback = UnRound (link_ptr->Boffset ());
				if ( rounded_connectivity && setback_override_flag ) setback = setback_distance;

				Link_Shape (link_ptr, 1, &points, length - setback, 10.0, lane_width);
			}

			// collect the first two outbound points for intersection interpolation
			double x3 = points[1]->x;
			double y3 = points[1]->y;
			double x4 = points[2]->x;
			double y4 = points[2]->y;

			if ( rounded_connectivity ) {
				if (!RoundedConnectivity (x1, y1, x2, y2, x3, y3, x4, y4, &arcview_timing)) goto mem_error;
			} else {
				XYZ_Point pt; pt.x = x2; pt.y = y2;
				if (!arcview_timing.points.Add (&pt)) goto mem_error;
				pt.x = x3; pt.y = y3;
				if (!arcview_timing.points.Add (&pt)) goto mem_error;
			}

			for (i=2; i <= points.Num_Points (); i++) {
				if (!arcview_timing.points.Add (points [i])) goto mem_error;
			}
		}
		if (arcview_timing.points.Num_Points () == 0) {
			Warning ("Phase %d was Not Found for Time Plan %d", phase, timing);
		} else {
			if (!arcview_timing.Write_Record ()) {
				Error ("Writing ArcView Timing Plan File");
			}
		}
	}
	End_Progress ();

	arcview_timing.Close ();
	return;

mem_error:
	Error ("Insufficient Memory for Timing Plan Shape Points");
	return;
}
