//*********************************************************
//	Write_Driver - write a transit driver file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Write_Driver
//---------------------------------------------------------

void ArcNet::Write_Driver (void)
{
	int i, j, dir, overlap;
	int route_field, links_field, type_field, subtype_field, notes_field;
	double offset, length, side, side_offset;
	bool offset_flag, first;

	Link_Data *link_ptr;
	Driver_Data *driver_ptr;
	Db_Base *file;
	
	file = Network_Db_Base (TRANSIT_DRIVER);

	route_field = file->Required_Field ("ROUTE");
	links_field = file->Required_Field ("LINKS", "NLINKS", "NUM_LINKS");
	type_field = file->Optional_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	subtype_field = file->Optional_Field ("SUBTYPE", "SUB_TYPE");
	notes_field = file->Optional_Field ("NOTES");

	Show_Message ("Writing Transit Driver -- Record");
	Set_Progress (1000);

	offset_flag = (route_offset != 0.0);

	//---- set the overlap count ----

	if (!overlap_flag) {
		for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
			link_ptr->Aoffset (0);
			link_ptr->Boffset (0);
		}
	}

	//---- process each transit route ----

	for (driver_ptr = driver_data.First (); driver_ptr; driver_ptr = driver_data.Next ()) {
		Show_Progress ();

		arcview_route.Put_Field (route_field, driver_ptr->Route ());
		arcview_route.Put_Field (type_field, driver_ptr->Type ());
		arcview_route.Put_Field (subtype_field, driver_ptr->Sub_Type ());
		arcview_route.Put_Field (links_field, driver_ptr->Links ());
		//arcview_route.Put_Field (notes_field, (char *) driver_ptr->Notes ());

		arcview_route.points.Reset ();
		first = true;

		for (i=1; i <= driver_ptr->Links (); i++) {

			link_ptr = link_data.Get (driver_ptr->Link (i));
			dir = driver_ptr->Dir (i);

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
				j = 1;
			} else if (first) {
				j = 1;
				first = false;
			} else {
				j = 2;
			}
			if (dir == 0) {
				Link_Shape (link_ptr, 0, &points, side_offset, length, side);
			} else {
				Link_Shape (link_ptr, 1, &points, offset, length, side);
			}
			for (; j <= points.Num_Points (); j++) {
				if (!arcview_route.points.Add (points [j])) {
					Error ("Insufficient Memory for Transit Driver Shape Points");
				}
			}
		}

		if (!arcview_route.Write_Record ()) {
			Error ("Writing ArcView Transit Driver File");
		}
	}
	End_Progress ();

	arcview_route.Close ();
}
