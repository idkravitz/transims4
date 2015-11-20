//*********************************************************
//	Read_Phasing.cpp - read the phasing plan file
//*********************************************************

#include "Network_Service.hpp"

#include "Phasing_File.hpp"

//---------------------------------------------------------
//	Read_Phasing
//---------------------------------------------------------

void Network_Service::Read_Phasing (void)
{
	Db_File *file = Network_Db_File (PHASING_PLAN);

	//---- store the phasing plan data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (NODE) && Network_Option_Flag (NODE)) {
			Error (file_error, "Node", file->File_ID ());
		}
		if (!Network_File_Flag (LINK) && Network_Option_Flag (LINK)) {
			Error (file_error, "Link", file->File_ID ());
		}
		if (!Network_File_Flag (TIMING_PLAN) && Network_Option_Flag (TIMING_PLAN)) {
			Error (file_error, "Timing Plan", file->File_ID ());
		}
	}
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Phasing_Processing (file)) {
			if (!phasing_data.Add ()) {
				Error ("Adding Record to the Phasing Plan List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	phasing_data.Max_Records (phasing_data.Num_Records ());

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = phasing_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Phasing Plan Data Records = %d", num);
	}
	if (num > 0) Network_Data_True (PHASING_PLAN);
}

//---------------------------------------------------------
//	Phasing_Processing
//---------------------------------------------------------

bool Network_Service::Phasing_Processing (Db_File *fh)
{
	bool node_flag, link_flag, timing_flag, detector_flag;
	int lvalue, index, node_num, num;
	char *svalue, *ptr;

	Link_Data *link_ptr = NULL;
	Phasing_Data *phasing_ptr = NULL;
	Timing_Data timing_rec;

	Phasing_File *file = (Phasing_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);
	timing_flag = Network_Data_Flag (TIMING_PLAN);
	detector_flag = Network_Data_Flag (DETECTOR);

	//---- reserve memory ----

	if (phasing_data.Max_Records () == 0) {
		if (!phasing_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	phasing_ptr = phasing_data.New_Record ();
	if (phasing_ptr == NULL) goto mem_error;
	phasing_ptr->Reset ();

	node_num = file->Node ();

	if (node_num == 0) return (false);

	//---- store the node ----

	if (node_flag) {
		num = node_data.Get_Index (node_num);

		if (num == 0) {
			return (Check_Message ("Phasing Plan Node %d was Not Found in the Node file", node_num));
		}
		if (!Renumber ()) {
			num = node_num;
		}
	} else {
		num = node_num;
	}
	phasing_ptr->Node (num);

	//---- check the timing plan ----

	if (timing_flag) {
		timing_rec.Timing (file->Timing ());
		timing_rec.Phase (file->Phase ());

		if (timing_data.Get_Index (&timing_rec) == 0) {
			return (Check_Message ("Timing Plan %d-%d was Not Found in the Timing Plan file", file->Timing (), file->Phase ()));
		}
	}
	phasing_ptr->Timing (file->Timing ());
	phasing_ptr->Phase (file->Phase ());

	//---- check/convert the in-link number ----
	
	lvalue = file->In_Link ();
	if (lvalue <= 0) goto link_error;

	if (link_flag) {
		index = link_data.Get_Index (lvalue);
		if (index == 0) goto link_error;

		link_ptr = link_data [index];

		if (!Renumber ()) {
			index = lvalue;
		}
		phasing_ptr->In_Link (index);
		
		if (link_ptr->Anode () == num) {
			phasing_ptr->In_Dir (1);
			index = link_ptr->BA_Dir ();
		} else if (link_ptr->Bnode () == num) {
			phasing_ptr->In_Dir (0);
			index = link_ptr->AB_Dir ();
		} else {
			index = 0;
		}
		if (index == 0) goto link_error;
	} else {
		phasing_ptr->In_Link (lvalue);
	}

	//---- check/convert the out-link number ----
	
	lvalue = file->Out_Link ();
	if (lvalue <= 0) goto link_error;

	if (link_flag) {
        index = link_data.Get_Index (lvalue);
		if (index == 0) goto link_error;

		link_ptr = link_data [index];

		if (!Renumber ()) {
			index = lvalue;
		}
		phasing_ptr->Out_Link (index);

		if (link_ptr->Bnode () == num) {
			phasing_ptr->Out_Dir (1);
			index = link_ptr->BA_Dir ();
        } else if (link_ptr->Anode () == num) {
			phasing_ptr->Out_Dir (0);
			index = link_ptr->AB_Dir ();
		} else {
			index = 0;
		}
		if (index == 0) goto link_error;
	} else {
		phasing_ptr->Out_Link (lvalue);
	}

	//---- convert the protection code ----

	phasing_ptr->Protection (Protection_Code (file->Protection ()));

	//---- process the detectors ----

	svalue = file->Detectors ();

	if (*svalue == '\0' || atoi (svalue) == 0) {
		phasing_ptr->Num_Detectors (0);
		return (true);
	}
	num = 1;
	ptr = svalue;

	while ((ptr = strchr (ptr, '/')) != NULL) {
		num++;
		ptr++;
	}
	if (!phasing_ptr->Num_Detectors (num)) {
		Error ("Insufficient Memory for %d Detectors on Phasing Plan %d", num, Progress_Count ());
	}
	num = 0;

	while (svalue != NULL) {
		ptr = strchr (svalue, '/');

		if (ptr != NULL) {
			*ptr++ = '\0';
		}
		lvalue = atoi (svalue);

		if (lvalue <= 0) break;
		num++;

		if (detector_flag) {
			index = detector_data.Get_Index (lvalue);

			if (index == 0) {
				return (Check_Message ("Phasing Plan Detector %d was Not Found in the Detector file", lvalue));
			}
			if (!Renumber ()) {
				index = lvalue;
			}
			phasing_ptr->Detector (num, index);
		} else {
			phasing_ptr->Detector (num, lvalue);
		}
		svalue = ptr;
	}
	if (phasing_ptr->Num_Detectors () != num) {
		return (Check_Message ("Phasing Plan %d Detector Numbers are Not Valid", Progress_Count ()));
	}
	return (true);

link_error:
	return (Check_Message ("Phasing Plan Node %d Link %d was Not Found", node_num, lvalue));

mem_error:
	Error ("Insufficient Memory for Phasing Plan Data");
	return (false);
}
