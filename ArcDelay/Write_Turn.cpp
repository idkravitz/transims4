//*********************************************************
//	Write_Turn.cpp - write the arcview turn file
//*********************************************************

#include "ArcDelay.hpp"

//---------------------------------------------------------
//	Write_Turn
//---------------------------------------------------------

void ArcDelay::Write_Turn (void)
{
	int i, j, in_dir, vol, period, nperiods, end, time, green, yellow, red;
	double length, setback, delay;
	char buffer [FIELD_BUFFER], string [STRING_BUFFER];
	bool read_flag;

	Link_Data *link_ptr;
	Connect_Time *connect_ptr;
	Range_Data *range_ptr;
	Signal_Time *signal_ptr, signal_rec;
	Phasing_Data *phase_ptr;

	signal_rec.green = 0;
	signal_rec.yellow = 0;
	signal_rec.red = 0;

	//---- process each time period ----

	read_flag = false;
	nperiods = time_period.Num_Ranges ();

	for (period = 1; period <= nperiods; period++) {

		range_ptr = time_period [period];
		end = range_ptr->High () + 1;

		str_cpy (string, sizeof (string), time_period.Range_Format (period));

		for (i=j=0; string [i] != '\0'; i++) {
			if (string [i] == ':') continue;
			if (string [i] == '.' && string [i+1] == '.') {
				buffer [j++] = '_';
				i++;
			} else {
				buffer [j++] = string [i];
			}
		}
		buffer [j] = '\0';

		//---- create a new shape file ----

		str_fmt (string, sizeof (string), "%s_%s.shp", turnname, buffer);
		Print (1);

		if (!arcview_turn.Open (string)) {
			File_Error ("Opening ArcView Turn File", arcview_turn.Shape_Filename ());
		}
		arcview_turn.Write_Header ();	

		Show_Message ("Writing %s %s -- Record", arcview_turn.File_Type (), time_period.Format_Time (end));
		Set_Progress (1000);

		//---- gather phasing times for this time period ----

		if (event_flag) {
			signal_time.Reset ();

			while (read_flag || event_file.Read ()) {
				read_flag = false;
				time = time_period.Step (event_file.Time ());

				if (time < range_ptr->Low ()) continue;
				if (time > range_ptr->High ()) break;

				if (event_file.Event () != 0) continue;

				signal_rec.time = time;
				signal_rec.node = event_file.Node ();
				signal_rec.plan_phase = (event_file.Plan () << 8);
				signal_rec.plan_phase += (event_file.Phase () & 0xFF);

				str_cpy (buffer, sizeof (buffer), event_file.Status ());
				if (str_cmp (buffer, "Yellow") == 0) {
					signal_rec.type = 1;
				} else if (str_cmp (buffer, "Red") == 0) {
					signal_rec.type = 2;
				} else {
					signal_rec.type = 0;
				}
				signal_ptr = (Signal_Time *) signal_time.Get (&signal_rec);

				if (signal_ptr == NULL) {
					if (!signal_time.Add (&signal_rec)) {
						Error ("Adding Signal Timing Record");
					}
				} else {
					time = signal_rec.time - signal_ptr->time;

					if (signal_ptr->type == 1) {
						signal_ptr->yellow += time;
					} else if (signal_ptr->type == 2) {
						signal_ptr->red += time;
					} else {
						signal_ptr->green += time;
					}
					signal_ptr->type = signal_rec.type;
					signal_ptr->time = signal_rec.time;
				}
			}
			read_flag = true;
		}

		//---- process each connection ----

		for (connect_ptr = connect_time.First_Key (); connect_ptr; connect_ptr = connect_time.Next_Key ()) {
			Show_Progress ();

			if (connect_ptr->Periods () == 0) continue;

			link_ptr = link_data.Get (connect_ptr->In_Link ());

			in_dir = connect_ptr->In_Dir ();

			//---- save the link direction data ----

			if (delay_file->LinkDir_Type () == LINK_SIGN) {
				if (in_dir == 1) {
					delay_file->Link (-link_ptr->Link ());
				} else {
					delay_file->Link (link_ptr->Link ());
				}
			} else {
				delay_file->Link (link_ptr->Link ());	

				if (delay_file->LinkDir_Type () == LINK_NODE) {
					delay_file->Dir ((in_dir == 1) ? link_ptr->Bnode () : link_ptr->Anode ());
				} else {
					delay_file->Dir (in_dir);
				}
			}

			//---- get the connection delay ----

			vol = connect_ptr->Volume (period);
			delay = UnRound (connect_ptr->TTime (period));

			if (vol > 0) {
				delay /= vol;
			}
			delay_file->Out_Link (connect_ptr->Out_Link ());
			delay_file->Out_Dir (connect_ptr->Out_Dir ());
			delay_file->Out_Turn (vol);
			delay_file->Out_Time (delay);

			//---- save the connection shape ----

			delay_file->Nest (false);
			arcview_turn.Copy_Fields (delay_file);

			delay_file->Nest (true);
			arcview_turn.Copy_Fields (delay_file);

			if (event_flag) {
				green = yellow = red = 0;
				for (phase_ptr = phasing_data.First (); phase_ptr; phase_ptr = phasing_data.Next ()) {
					if (connect_ptr->In_Link_Dir () == phase_ptr->In_Link_Dir () &&
						connect_ptr->Out_Link_Dir () == phase_ptr->Out_Link_Dir ()) {

						signal_rec.node = phase_ptr->Node ();
						signal_rec.plan_phase = (phase_ptr->Timing () << 8);
						signal_rec.plan_phase += (phase_ptr->Phase () & 0xFF);

						signal_ptr = (Signal_Time *) signal_time.Get (&signal_rec);

						if (signal_ptr != NULL) {
							green += signal_ptr->green;
							yellow += signal_ptr->yellow;
							red += signal_ptr->red;
						}
					}
				}
				time = green + yellow + red;
				arcview_turn.Put_Field (green_field, green);
				arcview_turn.Put_Field (yellow_field, yellow);
				arcview_turn.Put_Field (red_field, red);
				if (time > 0) {
					time = (green * 100 + time / 2) / time;
				}
				arcview_turn.Put_Field (split_field, time);
			}

			//---- approach leg ----

			arcview_turn.points.Reset ();

			if (connect_ptr->In_Dir () == 0) {
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

			link_ptr = link_data.Get (connect_ptr->Out_Link ());

			if (connect_ptr->Out_Dir () == 0) {
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

			//---- save the turn record ----

			if (!arcview_turn.Write_Record ()) {
				Error ("Writing %s", arcview_turn.File_Type ());
			}
		}
		End_Progress ();
		arcview_turn.Close ();
	}
	return;

mem_error:
	Error ("Insufficient Memory for Turn Shape Points");
	return;
}
