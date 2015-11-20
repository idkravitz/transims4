//*********************************************************
//	Write_Coordinator.cpp - additional signal processing
//*********************************************************

#include "SubareaNet.hpp"

#include "Coordinator_File.hpp"

//---------------------------------------------------------
//	Write_Coordinator
//---------------------------------------------------------

void SubareaNet::Write_Coordinator (void)
{
	int id, id_field;

	Signal_Data *signal_ptr;

	Coordinator_File *coord_file = (Coordinator_File *) Network_Db_Base (NEW_SIGNAL_COORDINATOR);

	Show_Message ("Writing Subarea Signal Coordinator -- Record");
	Set_Progress (1000);

	id_field = coordinator_db.Required_Field ("ID");

	coordinator_db.Rewind ();

	while (coordinator_db.Read_Record ()) {
		Show_Progress ();

		coordinator_db.Get_Field (id_field, &id);

		for (signal_ptr = signal_data.First (); signal_ptr; signal_ptr = signal_data.Next ()) {
			if (signal_ptr->Coordinator () == id) {
				coord_file->Copy_Fields (&coordinator_db);

				if (!coord_file->Write ()) {
					Error ("Writing %s", coord_file->File_Type ());
				}
				ncoord++;
				break;
			}
		}
	}
	End_Progress ();
}
