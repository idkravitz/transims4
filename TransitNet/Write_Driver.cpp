//*********************************************************
//	Write_Driver.cpp - write the transit driver file
//*********************************************************

#include "TransitNet.hpp"

#include "Driver_File.hpp"

//---------------------------------------------------------
//	Write_Driver
//---------------------------------------------------------

void TransitNet::Write_Driver (void)
{
	int i, link, dir;
	bool new_flag;

	Line_Data *line_ptr;
	Link_Data *link_ptr;
	Driver_Data *driver_ptr;

	Driver_File *driver_file = (Driver_File *) Network_Db_Base (NEW_TRANSIT_DRIVER);

	//---- write the driver file ----

	Show_Message ("Writing %s -- Record", driver_file->File_Type ());
	Set_Progress (1000);

	for (driver_ptr = driver_data.First_Key (); driver_ptr; driver_ptr = driver_data.Next_Key ()) {
		Show_Progress ();

		new_flag = (edit_list.Get_Index (driver_ptr->Route ()) != 0);

		driver_file->Route (driver_ptr->Route ());
		driver_file->Links (driver_ptr->Links ());
		driver_file->Veh_Type (driver_ptr->Type ());
		driver_file->Sub_Type (driver_ptr->Sub_Type ());

		if (driver_file->Notes_Flag ()) {
			line_ptr = line_data.Get (driver_ptr->Route ());

			if (line_ptr != NULL) {
				driver_file->Notes ((char *) line_ptr->Name ());
			}
		}
		if (!driver_file->Write (false)) goto driver_error;
		ndriver++;
		if (new_flag) driver_edit++;

		for (i=1; i <= driver_ptr->Links (); i++) {
			link = driver_ptr->Link (i);
			dir = driver_ptr->Dir (i);

			driver_file->Link (link);

			if (driver_file->LinkDir_Type () == LINK_SIGN) {
				if (dir == 1) driver_file->Link (-link);
			} else if (driver_file->LinkDir_Type () == LINK_DIR) {
				driver_file->Dir (dir);
			} else {
				link_ptr = link_data.Get (link);

				if (dir == 0) {
					driver_file->Dir (link_ptr->Bnode ());
				} else {
					driver_file->Dir (link_ptr->Anode ());
				}
			}
			if (!driver_file->Write (true)) goto driver_error;
			ndriver++;
			if (new_flag) driver_edit++;
		}
	}
	End_Progress ();

	driver_file->Close ();
	return;

driver_error:
	Error ("Writing %s", driver_file->File_Type ());
}
