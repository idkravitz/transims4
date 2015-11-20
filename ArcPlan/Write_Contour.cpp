//*********************************************************
//	Write_Contour - draw the time/distance contour polygons
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Contour
//---------------------------------------------------------

void ArcPlan::Write_Contour (Offset_Data *org_ptr)
{
	int time_contour, time_field, distance_contour, distance_field;
	int i, j, dir, dir_rec, num_path, *path, anode, lnk, tod, period;
	int cum_time_in, cum_time_out, ttime, diff, in_link_dir, time_in;
	double factor, dist, from_a, length, len, offset, end, cum_dist_in, cum_dist_out, dist_in;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Connect_Time *connect_ptr;
	AB_Data *ab_ptr;

	time_contour = arcview_time.Field_Number ("CONTOUR");
	time_field = arcview_time.Field_Number ("TIME");
	distance_contour = arcview_distance.Field_Number ("CONTOUR");
	distance_field = arcview_distance.Field_Number ("DISTANCE");

	//---- get the path list ----

	path = plan_file.Path (&num_path);

	if (!type_flag) num_path--;
	if (num_path == 0) return;

	anode = in_link_dir = cum_time_in = cum_time_out = 0;
	cum_dist_in = cum_dist_out = 0;
	tod = Round (plan_file.Time ());

	//---- process each link on the path ----

	for (i=1; i <= num_path; i++, path++) {

		if (type_flag) {		//---- node-based plans ----
			if (i == 1) {
				lnk = org_ptr->Link ();

				link_ptr = link_data.Get (lnk);
				if (link_ptr == NULL) goto link_error;

				if (link_ptr->Bnode () == *path) {
					dir = 0;
				} else if (link_ptr->Anode () == *path) {
					dir = 1;
				} else {
					goto link_error;
				}
			} else {
				ab_ptr = ab_key.Get (anode, *path);

				if (ab_ptr == NULL) {
					Warning ("A Link between %d and %d was Not Found", anode, *path);
					return;
				}
				ttime_ptr = ttime_data [ab_ptr->dir];

				lnk = ttime_ptr->Link ();
				dir = ttime_ptr->Dir ();
			}
			anode = *path;

		} else {		//---- link-based plans ----

			if (*path < 0) {
				lnk = -(*path);
				dir = 1;
			} else {
				lnk = *path;
				dir = 0;
			}
		}

		//---- get the link data ----

		link_ptr = link_data.Get (lnk);
		if (link_ptr == NULL) goto link_error;

		length = UnRound (link_ptr->Length ());

		if (i == 1) {
			from_a = org_ptr->Offset ();
		} else if (dir == 0) {
			from_a = 0;
		} else {
			from_a = length;
		}
		if (dir == 0) {
			dir_rec = link_ptr->AB_Dir ();
			dist = length - from_a;
		} else {
			dir_rec = link_ptr->BA_Dir ();
			dist = from_a;
		}
		if (dir_rec == 0) goto link_error;

		ttime_ptr = ttime_data [dir_rec];
		period = ttime_data.Period (Resolve (tod));

		ttime = ttime_ptr->TTime (period);

		if (i == 1) {
			factor = dist / length;
			if (factor < 0.001) factor = 0.001;

			ttime = (int) (ttime * factor + 0.5);
		}

		//---- add turning movement delay ----

		if (turn_flag) {
			connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());
			in_link_dir = ttime_ptr->Link_Dir ();

			if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
				ttime += connect_ptr->TTime (period);
				if (ttime < 1) ttime = 1;
			}
		}
		tod += ttime;

		cum_time_out = cum_time_in + ttime;
		cum_dist_out = cum_dist_in + dist;

		//---- process the link ----

		if (!link_flag [dir_rec]) {

			//---- check each time increment ----

			time_in = cum_time_in;
			offset = from_a;
			len = dist;

			for (j = time_points.First_Key (); j > 0; j = time_points.Next_Key ()) {

				ttime = Round (j);
				if (ttime < time_in) continue;

				if (ttime > cum_time_out) {

					Link_Shape (link_ptr, dir, &(arcview_time.points), offset, len);
					time_in = cum_time_out;

				} else {

					diff = cum_time_out - time_in;
					if (diff <= 0) continue;

					factor = (double) (ttime - time_in) / diff;
					end = len * factor;
					len -= end;

					Link_Shape (link_ptr, dir, &(arcview_time.points), offset, end);
					time_in = ttime;

					if (dir == 0) {
						offset += end;
					} else {
						offset -= end;
					}
				}

				//---- save the link record ----

				if (arcview_time.points.Num_Points () == 0) continue;

				arcview_time.Put_Field (time_contour, j);
				if (cum_time_out < ttime) {
					arcview_time.Put_Field (time_field, Resolve (cum_time_out));
				} else {
					arcview_time.Put_Field (time_field, j);
				}

				//---- write the shape record ----

				if (!arcview_time.Write_Record ()) {
					Error ("Writing %s", arcview_time.File_Type ());
				}
				num_time++;
				if (time_in == cum_time_out) break;
			}

			//---- check each distance increment ----

			dist_in = cum_dist_in;
			offset = from_a;
			len = dist;

			for (j = distance_points.First_Key (); j > 0; j = distance_points.Next_Key ()) {

				if (j < dist_in) continue;

				if (j > cum_dist_out) {

					Link_Shape (link_ptr, dir, &(arcview_distance.points), offset, len);
					dist_in = cum_dist_out;

				} else {

					factor = cum_dist_out - dist_in;
					if (factor <= 0) continue;

					factor = (double) (j - dist_in) / factor;
					end = len * factor;
					len -= end;

					Link_Shape (link_ptr, dir, &(arcview_distance.points), offset, end);
					dist_in = j;

					if (dir == 0) {
						offset += end;
					} else {
						offset -= end;
					}
				}

				//---- save the link record ----

				if (arcview_distance.points.Num_Points () == 0) continue;

				arcview_distance.Put_Field (distance_contour, j);
				if (cum_dist_out < j) {
					arcview_distance.Put_Field (distance_field, cum_dist_out);
				} else {
					arcview_distance.Put_Field (distance_field, j);
				}

				//---- write the shape record ----

				if (!arcview_distance.Write_Record ()) {
					Error ("Writing %s", arcview_distance.File_Type ());
				}
				num_distance++;
				if (dist_in == cum_dist_out) break;
			}
			link_flag [dir_rec] = true;
		}
		cum_time_in = cum_time_out;
		cum_dist_in = cum_dist_out;
	}
	return;

link_error:
	Warning ("Traveler %d Link %d was Not Available", plan_file.Traveler (), lnk);
}
