//*********************************************************
//	Route_Nodes.cpp - Read Route Header and Nodes Files
//*********************************************************

#include "ArcNet.hpp"

#include "Integer_Array.hpp"

//---------------------------------------------------------
//	Route_Nodes
//---------------------------------------------------------

void ArcNet::Route_Nodes (void)
{
	int i, n, node, node0, route, nroutes, overlap, dir, route_field;
	double offset, length, side, side_offset;
	bool offset_flag, first;

	AB_Data *ab_ptr;
	Dir_Data *dir_ptr;
	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Route_Data *route_ptr;
	XYZ_Point point;

	offset_flag = (route_offset != 0.0);
	side = 0.0;

	//---- set the overlap count ----

	if (!overlap_flag) {
		for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
			link_ptr->Aoffset (0);
			link_ptr->Boffset (0);
		}
	}

	//---- write the route file ----

	Show_Message ("Writing Transit Routes -- Record");
	Set_Progress (100);
	nroutes = 0;

	route_field = route_header_db.Required_Field ("ROUTE", "LINE");

	route_header_db.Rewind ();

	while (route_header_db.Read_Record ()) {
		Show_Progress ();

		arcview_route.points.Reset ();
		arcview_route.Copy_Fields (&route_header_db);

		route_header_db.Get_Field (route_field, &route);

		route_ptr = route_data.Get (route);
		if (route_ptr == NULL) continue;

		//---- create the path vector ---

		node0 = 0;
		first = true;

		for (n=1; n <= route_ptr->Nodes (); n++) {
			node = route_ptr->Node (n);

			if (node0 == 0) {
				node0 = node;
				continue;
			}
			ab_ptr = ab_key.Get (abs(node0), abs(node));

			if (ab_ptr == NULL) {
				Warning ("No Link between Nodes %d and %d on Route %d", node0, node, route);
				node0 = node;

				node_ptr = node_data.Get (abs (node0));

				if (node_ptr != NULL) {
					point.x = UnRound (node_ptr->X ());
					point.y = UnRound (node_ptr->Y ());
					point.z = UnRound (node_ptr->Z ());
					
					if (!arcview_route.points.Add (&point)) {
						Error ("Insufficient Memory for Route Node Shape Points");
					}
				}
				continue;
			}
			dir_ptr = dir_data [ab_ptr->dir];
			dir = dir_ptr->Dir ();

			node0 = node;

			link_ptr = link_data.Get (dir_ptr->Link ());

			offset = length = UnRound (link_ptr->Length ());
			side_offset = route_offset;

			if (side_offset > length / 3.0) {
				side_offset = length / 3.0;
			}

			if (offset_flag) {
				if (!overlap_flag) {
					if (dir == 0) {
						overlap = link_ptr->Aoffset () + 1;
						link_ptr->Aoffset (overlap);
					} else {
						overlap = link_ptr->Boffset () + 1;
						link_ptr->Boffset (overlap);
					}
					side = side_offset = route_offset * overlap;

					if (side_offset > length / 3.0) {
						side_offset = length / 3.0;
					}
					offset -= side_offset;
					length -= 2.0 * side_offset;

				} else {
					offset -= side_offset;
					length -= 2.0 * side_offset;

					if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
						side = side_offset;
					} else {
						side = 0.0;
					}
				}
				i = 1;
			} else if (first) {
				i = 1;
				first = false;
			} else {
				i = 2;
			}

			if (dir == 0) {
				Link_Shape (link_ptr, 0, &points, side_offset, length, side);
			} else {
				Link_Shape (link_ptr, 1, &points, offset, length, side);
			}
			for (; i <= points.Num_Points (); i++) {
				if (!arcview_route.points.Add (points [i])) {
					Error ("Insufficient Memory for Route Node Shape Points");
				}
			}
		}
		if (!arcview_route.Write_Record ()) {
			Error ("Writing ArcView Route Node File");
		}
		nroutes++;
	}
	End_Progress ();

	Print (2, "Number of Transit Routes = %d", nroutes);

	arcview_route.Close ();
}
