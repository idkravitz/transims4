//*********************************************************
//	Detector_Process.cpp - additional detector processing
//*********************************************************

#include "ArcNet.hpp"

#include "Detector_File.hpp"

//---------------------------------------------------------
//	Detector_Processing
//---------------------------------------------------------

bool ArcNet::Detector_Processing (Db_File *fh)
{
	int i, lane, center;
	double length, offset, side;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Detector_Data *detector_ptr;

	Detector_File *file = (Detector_File *) fh;

	if (Network_Service::Detector_Processing (file)) {
		arcview_detector.Copy_Fields (file);

		detector_ptr = detector_data.New_Record ();

		link_ptr = link_data.Get (detector_ptr->Link ());

		offset = UnRound (detector_ptr->Offset ());
		length = UnRound (detector_ptr->Length ());

		if (detector_ptr->Dir () == 1) {
			offset += length;
		} else {
			offset -= length;
		}
		if (lanes_flag) {
			arcview_detector.parts.Reset ();
			arcview_detector.points.Reset ();

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
			for (lane = detector_ptr->Low_Lane (); lane <= detector_ptr->High_Lane (); lane++) {
				side = (2 * lane - center) * lane_width / 2.0;

				Link_Shape (link_ptr, detector_ptr->Dir (), &points, offset, length, side);

				if (!arcview_detector.parts.Add (arcview_detector.points.Num_Points ())) goto mem_error;

				for (i=1; i <= points.Num_Points (); i++) {
					if (!arcview_detector.points.Add (points [i])) goto mem_error;
				}
			}
		} else {
			Link_Shape (link_ptr, detector_ptr->Dir (), &arcview_detector.points, offset, length, 0.0);
		}

		if (!arcview_detector.Write_Record ()) {
			Error ("Writing ArcView Detector File");
		}
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Detector Shape Points");
	return (false);
}
