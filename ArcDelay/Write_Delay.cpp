//*********************************************************
//	Write_Delay.cpp - write the arcview delay file
//*********************************************************

#include "ArcDelay.hpp"

#include "Smooth_Shape.hpp"

//---------------------------------------------------------
//	Write_Delay
//---------------------------------------------------------

void ArcDelay::Write_Delay (void)
{
	int i, lane, center, num_lanes, dir, dir1, end, period, nperiods, volume;
	double ttime, density, speed, length, side, start, width, offset;
	char start_time [FIELD_BUFFER], end_time [FIELD_BUFFER];
	char buffer [STRING_BUFFER];
	bool version3_flag, offset_flag;

	XYZ_Point point, *ptr;
	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Range_Data *range_ptr;

	version3_flag = (delay_file->Dbase_Format () == VERSION3);
	offset_flag = (link_offset != 0.0);

	//---- process each time period ----

	nperiods = time_period.Num_Ranges ();

	for (period = 1; period <= nperiods; period++) {

		range_ptr = time_period [period];
		end = range_ptr->High () + 1;

		str_cpy (start_time, sizeof (start_time), time_period.Format_Step (range_ptr->Low ()));
		str_cpy (end_time, sizeof (end_time), time_period.Format_Step (end));

		//---- create a new shape file ----

		str_fmt (buffer, sizeof (buffer), "%s_%s.shp", shapename, time_period.Range_Label (period));
		Print (1);

		if (!arcview_delay.Open (buffer)) {
			File_Error ("Opening ArcView Delay File", arcview_delay.Shape_Filename ());
		}
		arcview_delay.Write_Header ();

		Show_Message ("Writing %s %s -- Link", arcview_delay.File_Type (), end_time);
		Set_Progress (1000);

		//---- process each link ----

		for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {
			Show_Progress ();

			//---- process each direction ----

			for (dir = 0; dir < 2; dir++) {

				if (dir == 1) {
					perf_ptr = performance_data [link_ptr->BA_Dir ()];
				} else {
					perf_ptr = performance_data [link_ptr->AB_Dir ()];
				}
				if (perf_ptr == NULL || perf_ptr->Periods () == 0) continue;

				//---- save the link direction data ----

				if (delay_file->LinkDir_Type () == LINK_SIGN) {
					if (dir == 1) {
						delay_file->Link (-link_ptr->Link ());
					} else {
						delay_file->Link (link_ptr->Link ());
					}
				} else {
					delay_file->Link (link_ptr->Link ());	

					if (delay_file->LinkDir_Type () == LINK_NODE) {
						delay_file->Dir ((dir == 1) ? link_ptr->Bnode () : link_ptr->Anode ());
					} else {
						delay_file->Dir (dir);
					}
				}

				//---- calculate the performance ----

				volume = perf_ptr->Volume (period);
				ttime = perf_ptr->TTime (period);
				speed = perf_ptr->Avg_Speed (period);
				density = perf_ptr->Density (period);

				//---- save the Version 3 fields ----

				if (version3_flag) {

					delay_file->Time (end);
					delay_file->Lane (perf_ptr->Left () + 1);
					delay_file->Turn (0);

					delay_file->Volume (volume);
					delay_file->TTime (volume * ttime);
					delay_file->TTime2 (volume * ttime * ttime);

					delay_file->Vehicles ((int) (density + 0.5));
					delay_file->Speed (density * speed);
					delay_file->Speed2 (density * speed * speed);

				} else {

					delay_file->Start_Time (start_time);
					delay_file->End_Time (end_time);

					delay_file->Volume (volume);
					delay_file->Enter (perf_ptr->Enter (period));
					delay_file->Exit (perf_ptr->Exit (period));

					delay_file->Speed (speed);
					delay_file->TTime (ttime);
					delay_file->Delay (perf_ptr->Delay (period));

					delay_file->Density (density);
					delay_file->Max_Density (perf_ptr->Max_Density (period));

					delay_file->Time_Ratio (perf_ptr->Time_Ratio (period));
					delay_file->Avg_Queue (perf_ptr->Queue (period));
					delay_file->Max_Queue (perf_ptr->Max_Queue (period));
					delay_file->Cycle_Failure (perf_ptr->Failure (period));

					delay_file->VMT (perf_ptr->Distance (period));
					delay_file->VHT (perf_ptr->Time (period));
				}

				//---- save the link shape points ----

				delay_file->Num_Connect (0);

				arcview_delay.Copy_Fields (delay_file);

				if (index_flag) {
					arcview_delay.Put_Field (1, link_ptr->Link () * 2 + dir);
				}
				arcview_delay.parts.Reset ();
				arcview_delay.points.Reset ();

				//---- bandwidth processing ----

				if (width_flag && !link_dir_flag) {
					arcview_delay.Get_Field (width_field, &width);
					if (width == 0.0) continue;
					if (width < 0) width = -width;

					if (width < min_value) continue;
					width = width / width_factor;

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
					
					if (!arcview_delay.parts.Add (0)) goto mem_error;

					for (; ptr; ptr = points.Next ()) {
						if (!arcview_delay.points.Add (ptr)) goto point_error;
					}

					//---- get the outside points of the band ----

					offset = length - offset;
					dir1 = 1 - dir;

					Link_Shape (link_ptr, dir1, &points, offset, length, width);

					if (max_angle > 0 && min_length > 0) {
						Smooth_Shape (&points, max_angle, min_length);
					}

					for (ptr = points.First (); ptr; ptr = points.Next ()) {
						if (!arcview_delay.points.Add (ptr)) goto point_error;
					}

					//---- close the polygon ----

					if (!arcview_delay.points.Add (&point)) goto point_error;

				} else {

					//---- find the center point ----

					if (center_flag) {
						if (link_ptr->BA_Dir () == 0 || link_ptr->AB_Dir () == 0) {
							center = perf_ptr->Left () + perf_ptr->Thru () + perf_ptr->Right () + 1;
						} else {
							center = 1;
						}
					} else {
						center = 1;
					}
					length = UnRound (link_ptr->Length ());

					//---- draw lanes ----

					if (lanes_flag) {

						if (dir == 0) {
							start = UnRound (link_ptr->Aoffset ());

							length -= start + UnRound (link_ptr->Boffset ());
						} else {
							start = length - UnRound (link_ptr->Boffset ());
							length = start - UnRound (link_ptr->Aoffset ());
						}
						lane = perf_ptr->Left ();
						num_lanes = perf_ptr->Thru () + lane;

						for (++lane; lane <= num_lanes; lane++) {
							side = (2 * lane - center) * lane_width / 2.0;

							Link_Shape (link_ptr, dir, &points, start, length, side);

							if (!arcview_delay.parts.Add (arcview_delay.points.Num_Points ())) goto mem_error;

							for (i=1; i <= points.Num_Points (); i++) {
								if (!arcview_delay.points.Add (points [i])) goto mem_error;
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

							if (!arcview_delay.parts.Add (0)) goto mem_error;

							for (i=1; i <= points.Num_Points (); i++) {
								if (!arcview_delay.points.Add (points [i])) goto mem_error;
							}

						} else {
							
							//---- draw centerline -----

							if (!arcview_delay.parts.Add (0)) goto mem_error;

							Link_Shape (link_ptr, 0, &arcview_delay.points);
						}
					}
				}

				//---- save the link record ----

				if (!arcview_delay.Write_Record ()) {
					Error ("Writing ArcView Delay File");
				}
			}
		}
		End_Progress ();
		arcview_delay.Close ();
	}
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_link_dir.points.Num_Points ());

mem_error:
	Error ("Insufficient Memory for Link Shape Points");
}
