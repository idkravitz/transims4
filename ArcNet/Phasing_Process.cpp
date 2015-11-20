//*********************************************************
//	Phasing_Process.cpp - read the phasing plan file
//*********************************************************

#include "ArcNet.hpp"

#include "Phasing_File.hpp"

#include "RoundedConnectivity.hpp"

//---------------------------------------------------------
//	Phasing_Processing
//---------------------------------------------------------

bool ArcNet::Phasing_Processing (Db_File *fh)
{
	int i;
	double length, setback;

	Link_Data *link_ptr;
	Phasing_Data *phasing_ptr;

	Phasing_File *file = (Phasing_File *) fh;

	if (Network_Service::Phasing_Processing (file)) {

		arcview_phasing.Copy_Fields (file);
		arcview_phasing.points.Reset ();

		phasing_ptr = phasing_data.New_Record ();

		if (select_time) {
			if (timing_list.Get_Index (phasing_ptr->Timing ()) == 0) return (false);
		}

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
			if (!arcview_phasing.points.Add (points [i])) goto mem_error;
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
			if (!RoundedConnectivity (x1, y1, x2, y2, x3, y3, x4, y4, &arcview_phasing)) goto mem_error;
		} else {
			XYZ_Point pt; pt.x = x2; pt.y = y2;
			if (!arcview_phasing.points.Add (&pt)) goto mem_error;
			pt.x = x3; pt.y = y3;
			if (!arcview_phasing.points.Add (&pt)) goto mem_error;
		}

		for (i=2; i <= points.Num_Points (); i++) {
			if (!arcview_phasing.points.Add (points [i])) goto mem_error;
		}

		if (!arcview_phasing.Write_Record ()) {
			Error ("Writing ArcView Phasing Plan File");
		}
	}
	return (timing_flag);

mem_error:
	Error ("Insufficient Memory for Phasing Plan Shape Points");
	return (false);
}
