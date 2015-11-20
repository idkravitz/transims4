//*********************************************************
//	Read_Details.cpp - read the link details files
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Details
//---------------------------------------------------------

void TransimsNet::Read_Details (void)
{
	int link, type, control, use;
	char *str_ptr;

	Link_Detail_Data detail_rec;
	Time_Range *period;

	//---- read the link detail file ----

	Show_Message ("Reading %s - Record", detail_file.File_Type ());
	Set_Progress ();

	while (detail_file.Read ()) {
		Show_Progress ();

		link = detail_file.Link ();
		if (link_data.Get (link) == NULL) {
			Warning ("Detail Link %d was Not Found", link);
			continue;
		}
		detail_rec.Link (link);
		detail_rec.Dir (detail_file.Dir ());

		str_ptr = detail_file.Control ();

		if (*str_ptr == '\0') {
			type = 0;
			control = 0;
		} else if (*str_ptr == 'N') {
			type = 1;
			control = 0;
		} else if (*str_ptr == 'T' || *str_ptr == 'A' || *str_ptr == 't' || *str_ptr == 'a') {
			type = 2;
			control = Signal_Code (str_ptr);
		} else {
			type = 3;
			control = Sign_Code (str_ptr);
		}
		detail_rec.Type (type);
		detail_rec.Control (control);

		detail_rec.Left (detail_file.Left ());
		detail_rec.Left_Thru (detail_file.Left_Thru ());
		detail_rec.Thru (detail_file.Thru ());
		detail_rec.Right_Thru (detail_file.Right_Thru ());
		detail_rec.Right (detail_file.Right ());
		
		str_ptr = detail_file.Use_Type ();

		if (*str_ptr != '\0' && str_cmp (str_ptr, "NONE")) {
			use = Use_Code (str_ptr);
		} else {
			use = 0;
		}
		detail_rec.Use (use);
		detail_rec.Lanes (detail_file.Use_Lanes ());

		str_ptr = detail_file.Use_Period ();

		if (*str_ptr != '\0') {
			period = new Time_Range (1, "24_HOUR_CLOCK");
			period->Add_Ranges (str_ptr);
		} else {
			period = NULL;
		}
		detail_rec.Period (period);
		
		if (!detail_data.Add (&detail_rec)) {
			Error ("Adding Link Detail Data");
		}
	}
	End_Progress ();
	
	Print (2, "Number of Link Detail Records = %d", Progress_Count ());
}
