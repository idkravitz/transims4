//*********************************************************
//	Sum_Volume - add the path to link volumes
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Sum_Volume
//---------------------------------------------------------

void ArcPlan::Sum_Volume (Offset_Data *org_ptr, Offset_Data *des_ptr)
{
	int i, dir, dir_rec, num_path, *path, anode, lnk;

	Link_Data *link_ptr;
	AB_Data *ab_ptr;

	//---- get the path list ----

	anode = 0;
	path = plan_file.Path (&num_path);

	if (type_flag && num_path == 0) return;

	for (i=1; i <= num_path; i++, path++) {
		if (type_flag) {
			if (i == 1) {
				lnk = org_ptr->Link ();
				link_ptr = link_data.Get (lnk);

				if (link_ptr == NULL) goto link_error;

				if (link_ptr->Bnode () == *path) {
					dir_rec = link_ptr->AB_Dir ();
				} else if (link_ptr->Anode () == *path) {
					dir_rec = link_ptr->BA_Dir ();
				} else {
					goto link_error;
				}
			} else {
				ab_ptr = ab_key.Get (anode, *path);

				if (ab_ptr == NULL) {
					Warning ("A Link between %d and %d was Not Found", anode, *path);
					return;
				}
				dir_rec = ab_ptr->dir;
			}
			anode = *path;
		} else {
			if (*path < 0) {
				lnk = -(*path);
				dir = 1;
			} else {
				lnk = *path;
				dir = 0;
			}
			link_ptr = link_data.Get (lnk);

			if (link_ptr == NULL) goto link_error;

			if (dir == 0) {
				dir_rec = link_ptr->AB_Dir ();
			} else {
				dir_rec = link_ptr->BA_Dir ();
			}
		}
		if (dir_rec == 0) goto link_error;

		width_data [dir_rec]++;
	}

	//---- set the destination offset ----

	if (type_flag && anode > 0) {
		lnk = des_ptr->Link ();

		link_ptr = link_data.Get (lnk);
		if (link_ptr == NULL) goto link_error;
	
		if (link_ptr->Anode () == anode) {
			dir_rec = link_ptr->AB_Dir ();
		} else if (link_ptr->Bnode () == anode) {
			dir_rec = link_ptr->BA_Dir ();
		} else {
			goto link_error;
		}
		if (dir_rec == 0) goto link_error;

		width_data [dir_rec]++;
	}
	return;

link_error:
	Warning ("Traveler %d Link %d was Not Available", plan_file.Traveler (), lnk);
}
