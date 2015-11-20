//*********************************************************
//	Connect_Process.cpp - read the lane connectivity file
//*********************************************************

#include "ArcNet.hpp"

#include "Connect_File.hpp"

#include "RoundedConnectivity.hpp"

//---------------------------------------------------------
//	Connectivity_Processing
//---------------------------------------------------------

bool ArcNet::Connectivity_Processing (Db_File *fh)
{
	int i, center, in, out, num_in, num_out, num, num_connect, in_lane, out_lane, lane, pocket;
	double length, side, setback;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Connect_Data *connect_ptr;

	Connect_File *file = (Connect_File *) fh;

	if (Network_Service::Connectivity_Processing (file)) {

		arcview_connect.Copy_Fields (file);

		arcview_connect.parts.Reset ();
		arcview_connect.points.Reset ();

		connect_ptr = connect_data.New_Record ();

		num_in = connect_ptr->In_Lane_High () - connect_ptr->In_Lane_Low () + 1;
		num_out = connect_ptr->Out_Lane_High () - connect_ptr->Out_Lane_Low () + 1;

		num_connect = MAX (num_in, num_out);

		in = connect_ptr->In_Lane_Low ();
		out = connect_ptr->Out_Lane_Low ();

		if (num_in > num_out) { 	//---- too many input lanes ----
			lane = num_in - num_out;
			in_lane = in;

			dir_ptr = dir_data.Get (connect_ptr->In_Link_Dir ());

			//---- remove left pocket lanes ----

			pocket = dir_ptr->Left ();

			while (in_lane <= pocket && lane > 0) {
				in_lane++;
				lane--;
				out--;
			}

			//---- remove right pocket lanes ----

			if (lane > 0) {
				pocket += dir_ptr->Thru ();
				in_lane = connect_ptr->In_Lane_High ();

				while (in_lane > pocket && lane > 0) {
					in_lane--;
					lane--;
				}
			}

			//---- force outside lanes to merge ----

			while (lane > 0) {

				//---- force left lane to merge ----

				out--;
				lane--;

				//---- force right lane to merge ----

				if (lane > 0) {
					lane--;
				}
			}

		} else if (num_in < num_out) {		//---- too many output lanes ----
			lane = num_in - num_out;
			out_lane = out;

			dir_ptr = dir_data.Get (connect_ptr->Out_Link_Dir ());

			//---- remove left pocket lanes ----

			pocket = dir_ptr->Left ();

			while (out_lane <= pocket && lane < 0) {
				out_lane++;
				lane++;
				in--;
			}

			//---- remove right pocket lanes ----

			if (lane < 0) {
				pocket += dir_ptr->Thru ();
				out_lane = connect_ptr->Out_Lane_High ();

				while (out_lane > pocket && lane < 0) {
					out_lane--;
					lane++;
				}
			}

			//---- outside lanes diverge ----

			while (lane < 0) {

				//---- left lane diverge ----

				in--;
				lane++;

				//---- right lane diverge ----

				if (lane < 0) {
					lane++;
				}
			}
		}

		//---- draw each connection ----

		for (num=0; num < num_connect; num++, in++, out++) {
			if (in < connect_ptr->In_Lane_Low ()) {
				in_lane = connect_ptr->In_Lane_Low ();
			} else if (in > connect_ptr->In_Lane_High ()) {
				in_lane = connect_ptr->In_Lane_High ();
			} else {
				in_lane = in;
			}
			if (out < connect_ptr->Out_Lane_Low ()) {
				out_lane = connect_ptr->Out_Lane_Low ();
			} else if (out > connect_ptr->Out_Lane_High ()) {
				out_lane = connect_ptr->Out_Lane_High ();
			} else {
				out_lane = out;
			}
			if (!arcview_connect.parts.Add (arcview_connect.points.Num_Points ())) goto mem_error;

			//---- approach leg ----

			link_ptr = link_data.Get (connect_ptr->In_Link ());

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
			side = (2 * in_lane - center) * lane_width / 2.0;

			if (connect_ptr->In_Dir () == 0) {
				length = UnRound (link_ptr->Length ());
				setback = UnRound (link_ptr->Boffset ());
				if ( rounded_connectivity && setback_override_flag ) setback = setback_distance;

				Link_Shape (link_ptr, 0, &points, length - setback - 10.0, 10.0, side);
			} else {
				setback = UnRound (link_ptr->Aoffset ());
				if ( rounded_connectivity && setback_override_flag ) setback = setback_distance;

				Link_Shape (link_ptr, 1, &points, setback + 10.0, 10.0, side);
			}
			for (i=1; i <= points.Num_Points () - 1; i++) {
				if (!arcview_connect.points.Add (points [i])) goto mem_error;
			}

			// collect the last two incoming points for intersection interpolation
			int last_pt = points.Num_Points ();
			double x1 = points[last_pt-1]->x;
			double y1 = points[last_pt-1]->y;
			double x2 = points[last_pt]->x;
			double y2 = points[last_pt]->y;

			//---- departure leg ----

			link_ptr = link_data.Get (connect_ptr->Out_Link ());

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
			side = (2 * out_lane - center) * lane_width / 2.0;

			if (connect_ptr->Out_Dir () == 0) {
				setback = UnRound (link_ptr->Aoffset ());
				if ( rounded_connectivity && setback_override_flag ) setback = setback_distance;

				Link_Shape (link_ptr, 0, &points, setback, 10.0, side);
			} else {
				length = UnRound (link_ptr->Length ());
				setback = UnRound (link_ptr->Boffset ());
				if ( rounded_connectivity && setback_override_flag ) setback = setback_distance;

				Link_Shape (link_ptr, 1, &points, length - setback, 10.0, side);
			}

			// collect the first two outbound points for intersection interpolation
			double x3 = points[1]->x;
			double y3 = points[1]->y;
			double x4 = points[2]->x;
			double y4 = points[2]->y;

			if ( rounded_connectivity ) {
				if (!RoundedConnectivity (x1, y1, x2, y2, x3, y3, x4, y4, &arcview_connect)) goto mem_error;
			} else {
				XYZ_Point pt; pt.x = x2; pt.y = y2;
				if (!arcview_connect.points.Add (&pt)) goto mem_error;
				pt.x = x3; pt.y = y3;
				if (!arcview_connect.points.Add (&pt)) goto mem_error;
			}

			for (i=2; i <= points.Num_Points (); i++) {
				if (!arcview_connect.points.Add (points [i])) goto mem_error;
			}
		}
		if (!arcview_connect.Write_Record ()) {
			Error ("Writing ArcView Lane Connectivity File");
		}
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Lane Connectivity Shape Points");
	return (false);
}
