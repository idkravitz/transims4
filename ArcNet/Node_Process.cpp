//*********************************************************
//	Node_Process.cpp - additional node processing
//*********************************************************

#include "ArcNet.hpp"

#include "Node_File.hpp"

//---------------------------------------------------------
//	Node_Processing
//---------------------------------------------------------

bool ArcNet::Node_Processing (Db_File *fh)
{
	Node_File *file = (Node_File *) fh;
	XYZ_Point point;

	if (First_Record ()) {
		arcview_node.Num_Points (1);
	}

	if (Network_Service::Node_Processing (file)) {
		arcview_node.Copy_Fields (file);

		point.x = file->X ();
		point.y = file->Y ();
		point.z = file->Z ();

		arcview_node.points.Set (1, &point);

		if (!arcview_node.Write_Record ()) {
			Error ("Writing ArcView Node File");
		}
		return (true);
	}
	return (false);
}
