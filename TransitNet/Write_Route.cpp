//*********************************************************
//	Write_Route.cpp - write transit route and schedule files
//*********************************************************

#include "TransitNet.hpp"

#include "Line_File.hpp"
#include "Schedule_File.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void TransitNet::Write_Route (void)
{
	int i, j, stop, time;
	bool new_flag;

	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Link_Data *link_ptr;

	Line_File *line_file = (Line_File *) Network_Db_Base (NEW_TRANSIT_ROUTE);
	Schedule_File *schedule_file = (Schedule_File *) Network_Db_Base (NEW_TRANSIT_SCHEDULE);

	//---- write the route and schedule file ----

	Show_Message ("Writing %s -- Record", line_file->File_Type ());
	Set_Progress (1000);

	for (line_ptr = line_data.First_Key (); line_ptr; line_ptr = line_data.Next_Key ()) {
		Show_Progress ();

		new_flag = (edit_list.Get_Index (line_ptr->Route ()) != 0);

		//---- write the route file ----

		line_file->Route (line_ptr->Route ());
		line_file->Stops (line_ptr->Stops ());
		line_file->Mode (Transit_Code ((Transit_Type) line_ptr->Mode ()));
		line_file->Name ((char *) line_ptr->Name ());
		line_file->Notes ((char *) line_ptr->Notes ());

		if (!line_file->Write (false)) goto line_error;
		nroute++;
		if (new_flag) route_edit++;

		for (i=1; i <= line_ptr->Stops (); i++) {

			stop = line_ptr->Stop (i);
			line_file->Stop (stop);

			stop_ptr = stop_data.Get (stop);

			if (line_file->LinkDir_Type () == LINK_NODE) {
				line_file->Link (stop_ptr->Link ());

				link_ptr = link_data.Get (stop_ptr->Link ());

				if (stop_ptr->Dir () > 0) {
					line_file->Node (link_ptr->Anode ());
				} else {
					line_file->Node (link_ptr->Bnode ());
				}
			} else if (line_file->LinkDir_Type () == LINK_DIR) {
				line_file->Link (stop_ptr->Link ());
				line_file->Node (stop_ptr->Dir ());
			} else {
				if (stop_ptr->Dir () > 0) {
					line_file->Link (-(stop_ptr->Link ())); 
				} else {
					line_file->Link (stop_ptr->Link ());
				}
				line_file->Node (0);
			}
			line_file->Zone (line_ptr->Zone (i));
			line_file->Time_Flag (line_ptr->Time_Flag (i));

			if (!line_file->Write (true)) goto line_error;
			nroute++;
			if (new_flag) route_edit++;
		}

		//---- write the schedule file ----

		schedule_file->Route (line_ptr->Route ());

		for (i=1; i <= line_ptr->Runs (); i++) {
			for (j=1; j <= line_ptr->Stops (); j++) {

				stop = line_ptr->Stop (j);
				time = line_ptr->Schedule (i, j);

				schedule_file->Time (Resolve (time));
				schedule_file->Stop (stop);

				if (!schedule_file->Write ()) {
					Error ("Writing %s", schedule_file->File_Type ());
				}
				nschedule++;
				if (new_flag) schedule_edit++;
			}
		}
	}
	End_Progress ();

	line_file->Close ();
	schedule_file->Close ();
	return;

line_error:
	Error ("Writing %s", line_file->File_Type ());
}

