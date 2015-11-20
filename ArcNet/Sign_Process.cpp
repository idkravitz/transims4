//*********************************************************
//	Sign_Process.cpp - additional sign processing
//*********************************************************

#include "ArcNet.hpp"

#include "Sign_File.hpp"

//---------------------------------------------------------
//	Sign_Processing
//---------------------------------------------------------

bool ArcNet::Sign_Processing (Db_File *fh)
{
	int dir;
	double offset;

	Link_Data *link_ptr;
	Sign_Data *sign_ptr;

	Sign_File *file = (Sign_File *) fh;

	if (First_Record ()) {
		arcview_sign.Num_Points (1);
	}

	if (Network_Service::Sign_Processing (file)) {
		arcview_sign.Copy_Fields (file);

		sign_ptr = sign_data.New_Record ();

		link_ptr = link_data.Get (sign_ptr->Link ());

		dir = sign_ptr->Dir ();

		if (dir == 1) {
			offset = sign_setback;
		} else {
			offset = UnRound (link_ptr->Length ()) - sign_setback;
		}

		Link_Shape (link_ptr, dir, &points, offset, 0.0, sign_side);
		
		arcview_sign.points.Set (1, points [1]);

		if (!arcview_sign.Write_Record ()) {
			Error ("Writing ArcView Unsignalized Node File");
		}
	}
	return (false);
}
