//*********************************************************
//	Performance_Process.cpp - read the link delay file
//*********************************************************

#include "ArcDelay.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Performance_Processing
//---------------------------------------------------------

bool ArcDelay::Performance_Processing (Db_File *fh)
{
	//---- check the subarea polygon ----

	if (subarea_flag) {
		Delay_File *file = (Delay_File *) fh;

		if (!file->Nest ()) {
			int link;
			Node_Data *node_ptr;
			Link_Data *link_ptr;

			link = file->Link ();
			if (link < 0) link = -link;

			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) return (false);

			node_ptr = node_data.Get (link_ptr->Anode ());

			if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
				node_ptr = node_data.Get (link_ptr->Bnode ());

				if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
					return (false);
				}
			}
		}
	}
	return (Demand_Service::Performance_Processing (fh));
}
