//*********************************************************
//	Write_Coordinator.cpp - write the signal coordinator data
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Write_Coordinator
//---------------------------------------------------------

void ArcNet::Write_Coordinator (void)
{
	int id, id_field;

	XYZ_Point point;
	Node_Data *node_ptr;
	Signal_Data *signal_ptr;

	Show_Message ("Writing Signal Coordinator -- Record");
	Set_Progress (1000);

	id_field = coordinator_db.Required_Field ("ID");

	coordinator_db.Rewind ();

	while (coordinator_db.Read_Record ()) {
		Show_Progress ();

		arcview_coordinator.Copy_Fields (&coordinator_db);

		coordinator_db.Get_Field (id_field, &id);

		arcview_coordinator.points.Reset ();

		for (signal_ptr = signal_data.First (); signal_ptr; signal_ptr = signal_data.Next ()) {
			if (signal_ptr->Coordinator () == id) {
				node_ptr = node_data.Get (signal_ptr->Node ());

				point.x = UnRound (node_ptr->X ());
				point.y = UnRound (node_ptr->Y ());
				point.z = UnRound (node_ptr->Z ());
				
				if (!arcview_coordinator.points.Add (&point)) {
					Error ("Insufficient Memory for Signal Coordinator Points");
				}
			}
		}
		if (arcview_coordinator.points.Num_Points () > 0) {
			if (!arcview_coordinator.Write_Record ()) {
				Error ("Writing ArcView Signal Coordinator File");
			}
		}
	}
	End_Progress ();

	arcview_coordinator.Close ();
}
