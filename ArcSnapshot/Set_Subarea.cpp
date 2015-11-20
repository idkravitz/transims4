//*********************************************************
//	Set_Subarea.cpp - mark links within the subarea polygon
//*********************************************************

#include "ArcSnapshot.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Set_Subarea
//---------------------------------------------------------

void ArcSnapshot::Set_Subarea (void)
{
	Link_Data *link_ptr;		
	Node_Data *node_ptr;
	
	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		node_ptr = node_data.Get (link_ptr->Anode ());

		if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
			node_ptr = node_data.Get (link_ptr->Bnode ());

			if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
				link_ptr->Use (0);
			}
		}
	}
}
