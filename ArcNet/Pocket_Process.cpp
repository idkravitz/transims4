//*********************************************************
//	Pocket_Process.cpp - additional pocket lane processing
//*********************************************************

#include "ArcNet.hpp"

#include "Pocket_File.hpp"

//---------------------------------------------------------
//	Pocket_Processing
//---------------------------------------------------------

bool ArcNet::Pocket_Processing (Db_File *fh)
{
	int center;
	double length, offset, side;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Pocket_Data *pocket_ptr;

	Pocket_File *file = (Pocket_File *) fh;

	if (Network_Service::Pocket_Processing (file)) {

		arcview_pocket.Copy_Fields (file);

		pocket_ptr = pocket_data.New_Record ();

		link_ptr = link_data.Get (pocket_ptr->Link ());

		offset = UnRound (pocket_ptr->Offset ());
		length = UnRound (pocket_ptr->Length ());

		if (lanes_flag) {
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
			side = (2 * pocket_ptr->Lane () - center) * lane_width / 2.0;
		} else if (pocket_ptr->Lane () > 1) {
			side = pocket_side;
		} else {
			side = 0.0;
		}
		if (Link_Shape (link_ptr, pocket_ptr->Dir (), &arcview_pocket.points, offset, length, side)) {

			if (!arcview_pocket.Write_Record ()) {
				Error ("Writing ArcView Pocket Lane File");
			}
		}
	}
	return (false);
}
