//*********************************************************
//	Write_Link.cpp - convert the link data
//*********************************************************

#include "TPPlusNet.hpp"

#include "Link_File.hpp"

#include <math.h>

//---------------------------------------------------------
//	Write_Link
//---------------------------------------------------------

void TPPlusNet::Write_Link (void)
{
	int nlink, dir;

	Link_Name *link_ptr;
	Dir_Data *dir_ptr;

	Link_File *new_link;

	new_link = (Link_File *) Network_Db_Base (NEW_LINK);

	nlink = 0;

	//---- read each gis link ----

	Show_Message ("Writing %s -- Record", new_link->File_Type ());
	Set_Progress ();

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		if (new_link->Name_Flag ()) {
			new_link->Street (link_ptr->Name ());
		}
		new_link->Link (link_ptr->Link ());
		new_link->Anode (link_ptr->Anode ());
		new_link->Bnode (link_ptr->Bnode ());
		new_link->Length (UnRound (link_ptr->Length ()));
		new_link->Setback_A (UnRound (link_ptr->Aoffset ()));
		new_link->Setback_B (UnRound (link_ptr->Boffset ()));
		new_link->Bearing_A (0);
		new_link->Bearing_B (0);
		new_link->Use (Use_Code (link_ptr->Use ()));
		new_link->Type (Facility_Code ((Facility_Type) link_ptr->Type ()));

		//---- ab direction ----

		dir = link_ptr->AB_Dir ();
		
		if (dir > 0) {
			dir_ptr = dir_data [dir];

			new_link->Lanes_AB (dir_ptr->Thru ());
			new_link->Left_AB (dir_ptr->Left ());
			new_link->Right_AB (dir_ptr->Right ());
			new_link->Speed_AB (UnRound (dir_ptr->Speed ()));
			new_link->Fspd_AB (UnRound (dir_ptr->Time0 ()));
			new_link->Cap_AB (dir_ptr->Capacity ());
			new_link->Bearing_A (dir_ptr->In_Bearing ());
			new_link->Bearing_B (dir_ptr->Out_Bearing ());
		} else {
			new_link->Lanes_AB (0);
			new_link->Left_AB (0);
			new_link->Right_AB (0);
			new_link->Speed_AB (0);
			new_link->Fspd_AB (0);
			new_link->Cap_AB (0);
		}

		//---- ba direction ----

		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			new_link->Lanes_BA (dir_ptr->Thru ());
			new_link->Left_BA (dir_ptr->Left ());
			new_link->Right_BA (dir_ptr->Right ());
			new_link->Speed_BA (UnRound (dir_ptr->Speed ()));
			new_link->Fspd_BA (UnRound (dir_ptr->Time0 ()));
			new_link->Cap_BA (dir_ptr->Capacity ());
		} else {
			new_link->Lanes_BA (0);
			new_link->Left_BA (0);
			new_link->Right_BA (0);
			new_link->Speed_BA (0);
			new_link->Fspd_BA (0);
			new_link->Cap_BA (0);
		}

		//---- save the new link file ----

		if (!new_link->Write ()) {
			Error ("Writing Network Link File");
		}
		nlink++;
	}
	End_Progress ();
	new_link->Close ();

	Print (1, "Number of %s Records = %d", new_link->File_Type (), nlink);
}
