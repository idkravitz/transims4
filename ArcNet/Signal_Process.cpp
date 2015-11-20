//*********************************************************
//	Signal_Process.cpp - additional signal processing
//*********************************************************

#include "ArcNet.hpp"

#include "Signal_File.hpp"

//---------------------------------------------------------
//	Signal_Processing
//---------------------------------------------------------

bool ArcNet::Signal_Processing (Db_File *fh)
{
	XYZ_Point point;
	Node_Data *node_ptr;
	Signal_Data *signal_ptr;

	Signal_File *file = (Signal_File *) fh;

	if (First_Record ()) {
		arcview_signal.Num_Points (1);
	}

	if (Network_Service::Signal_Processing (file)) {
		arcview_signal.Copy_Fields (file);

		signal_ptr = signal_data.New_Record ();

		if (select_time) {
			if (!time_range.In_Range (signal_ptr->Start ())) return (false);

			if (!timing_list.Add (signal_ptr->Timing ())) {
				Error ("Adding to Timing List");
			}
		}
		node_ptr = node_data.Get (signal_ptr->Node ());

		point.x = UnRound (node_ptr->X ());
		point.y = UnRound (node_ptr->Y ());
		point.z = UnRound (node_ptr->Z ());
		
		arcview_signal.points.Set (1, &point);

		if (!arcview_signal.Write_Record ()) {
			Error ("Writing ArcView Signalized Node File");
		}
		if (coordinator_flag) return (true);
	}
	return (false);
}
