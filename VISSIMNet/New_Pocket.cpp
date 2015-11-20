//*********************************************************
//	New_Pocket.cpp - write the new pocket lane records
//*********************************************************

#include "VISSIMNet.hpp"

#include "Pocket_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Pocket
//---------------------------------------------------------

void VISSIMNet::New_Pocket (void)
{
	int link;

	Link_Splits *split_ptr;
	Link_Name *link_ptr;
	Pocket_Data *pocket_ptr, *new_pocket_ptr;
	Pocket_File *pocket_file;

	//---- copy and renumber the pockets ----

	for (pocket_ptr = pocket_data.First_Key (); pocket_ptr; pocket_ptr = pocket_data.Next_Key ()) {

		new_pocket_ptr = new_pocket_data.New_Record (true);

		memcpy (new_pocket_ptr, pocket_ptr, sizeof (Pocket_Data));

		if (pocket_ptr->Dir () == 1) {
			new_pocket_ptr->Link (pocket_ptr->Link () * MAX_SPLITS);
		} else {
			link = pocket_ptr->Link ();

			split_ptr = (Link_Splits *) link_splits.Get (&link);

			new_pocket_ptr->Link (pocket_ptr->Link () * MAX_SPLITS + split_ptr->num_splits);
		}

		//---- fix the offset and pocket length ----

		link_ptr = new_link_data.Get (new_pocket_ptr->Link ());

		if (new_pocket_ptr->Length () > link_ptr->Length ()) {
			new_pocket_ptr->Length (link_ptr->Length () - 1);
		}
		if (new_pocket_ptr->Type () == TURN_LANE) {
			if (new_pocket_ptr->Dir () == 0) {
				new_pocket_ptr->Offset (new_pocket_ptr->Length () + link_ptr->Boffset ());
			} else {
				new_pocket_ptr->Offset (new_pocket_ptr->Length () + link_ptr->Aoffset ());
			}
		} else if (new_pocket_ptr->Type () == MERGE_LANE) {
			if (new_pocket_ptr->Dir () == 0) {
				new_pocket_ptr->Offset (link_ptr->Length () - link_ptr->Aoffset ());
			} else {
				new_pocket_ptr->Offset (link_ptr->Length () - link_ptr->Boffset ());
			}
		}
		if (new_pocket_ptr->Dir () == 0) {		//---- offset from A node ----
			new_pocket_ptr->Offset (link_ptr->Length () - new_pocket_ptr->Offset ());
		}

		if (!new_pocket_data.Add ()) {
			Error ("Adding New Pocket Data");
		}
	}

	//---- write the new pocket file ----

	if (!Network_File_Flag (NEW_POCKET_LANE)) return;

	pocket_file = (Pocket_File *) Network_Db_Base (NEW_POCKET_LANE);

	for (pocket_ptr = new_pocket_data.First (); pocket_ptr; pocket_ptr = new_pocket_data.Next ()) {
		link_ptr = new_link_data.Get (pocket_ptr->Link ());

		pocket_file->ID (pocket_ptr->ID ());

		if (pocket_ptr->Dir () == 0) {
			pocket_file->Node (link_ptr->Bnode ());
		} else {
			pocket_file->Node (link_ptr->Anode ());
		}
		pocket_file->Link (pocket_ptr->Link ());
		pocket_file->Lane (pocket_ptr->Lane ());
		pocket_file->Type (Pocket_Code ((Pocket_Type) pocket_ptr->Type ()));
		pocket_file->Offset (0.0);
		pocket_file->Length (UnRound (pocket_ptr->Length ()));

		if (!pocket_file->Write ()) {
			Error ("Writing %s", pocket_file->File_Type ());
		}
	}
	pocket_file->Close ();

	Print (2, "Number of New Pocket Records = %d", new_pocket_data.Num_Records ());
}
