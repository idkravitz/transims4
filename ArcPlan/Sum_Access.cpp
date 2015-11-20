//*********************************************************
//	Sum_Access - summarize the travel time and distance
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Sum_Access
//---------------------------------------------------------

void ArcPlan::Sum_Access (Offset_Data *org_ptr, Offset_Data *des_ptr, int *time, int *distance)
{
	int i, dir, dir_rec, num_path, *path, anode, lnk, tod, period;
	int cum_time, ttime, in_link_dir;
	double factor, dist, from_a, length, cum_dist;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Connect_Time *connect_ptr;
	AB_Data *ab_ptr;

	//---- get the path list ----

	path = plan_file.Path (&num_path);

	if (!type_flag) num_path--;
	if (num_path == 0) return;

	anode = in_link_dir = cum_time = 0;
	cum_dist = 0;
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

		cum_time += ttime;
		cum_dist += dist;
	}
	*time += Resolve (cum_time);
	*distance += (int) (cum_dist + 0.5);
	return;

link_error:
	Warning ("Traveler %d Link %d was Not Available", plan_file.Traveler (), lnk);
}
