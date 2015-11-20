//*********************************************************
//	Trip_Length_Data.cpp - trip length data class
//*********************************************************

#include "Trip_Length_Data.hpp"

//---------------------------------------------------------
//	Trip_Length_Data constructor
//---------------------------------------------------------

Trip_Length_Data::Trip_Length_Data (void) : Class2_Index (0, 0)
{
	Count (0);
}

//---------------------------------------------------------
//	Trip_Length_Array constructor
//---------------------------------------------------------

Trip_Length_Array::Trip_Length_Array (int max_records) : 
	Class2_Array (sizeof (Trip_Length_Data), max_records), Static_Service ()
{
	increment = 0;
	output_flag = false;
}

//---- Add Trip ----

bool Trip_Length_Array::Add_Trip (int mode, int purpose1, int purpose2, int length, int count)
{
	Trip_Length_Data *data_ptr, data_rec;

	if (increment <= 0 || length < 0) return (false);

	data_rec.Mode (mode);
	if (purpose1 > purpose2) {
		data_rec.Purpose1 (purpose2);
		data_rec.Purpose2 (purpose1);
	} else {
		data_rec.Purpose1 (purpose1);
		data_rec.Purpose2 (purpose2);
	}
	length /= increment;
	if (length >= 0xFFFF) length = 0xFFFE;
	data_rec.Increment (length);

	data_ptr = (Trip_Length_Data *) Get (&data_rec);

	if (data_ptr == NULL) {
		data_ptr = New_Record (true);
		if (data_ptr == NULL) return (false);

		data_rec.Count (0);

		if (!Add (&data_rec)) return (false);
	}
	data_ptr->Add_Count (count);
	return (true);
}

//---- Open_Trip_Length_File ----

bool Trip_Length_Array::Open_Trip_Length_File (char *filename, char *label)
{
	if (filename != NULL) {
		output_flag = true;

		exe->Print (1);
		if (label == NULL) {
			length_file.File_Type ("New Trip Length File");
		} else {
			length_file.File_Type (label);
		}
		length_file.File_Access (Db_Code::CREATE);

		if (!length_file.Open (exe->Project_Filename (filename, exe->Extension ()))) {
			exe->File_Error ("Creating New Trip Length File", length_file.Filename ());
			return (false);
		}
	}
	return (true);
}

//---- Write_Trip_Length_File ----

void Trip_Length_Array::Write_Trip_Length_File (void)
{
	if (!output_flag) return;

	int num_out;
	Trip_Length_Data *data_ptr;

	FILE *file;

	exe->Show_Message ("Writing %s -- Record", length_file.File_Type ());
	exe->Set_Progress ();

	file = length_file.File ();

	//---- write the header ----

	fprintf (file, "MODE\tPURPOSE1\tPURPOSE2\tLENGTH\tTRIPS\n");

	//---- write each record ----
	
	num_out = 0;

	for (data_ptr = First_Key (); data_ptr; data_ptr = Next_Key ()) {
		exe->Show_Progress ();

		fprintf (file, "%d\t%d\t%d\t%d\t%d\n", data_ptr->Mode (), data_ptr->Purpose1 (),
			data_ptr->Purpose2 (), data_ptr->Increment () * increment, data_ptr->Count ());

		num_out++;
	}
	exe->End_Progress ();

	length_file.Close ();

	exe->Print (2, "Number of %s Records = %d", length_file.File_Type (), num_out);
}

//
////---- Report ----
//
//void Trip_Length_Array::Report (int number, char *_title, char *_key1, char *_key2)
//{
//	int size;
//	Trip_Length_Data *data_ptr, total;
//
//	//---- set header values ----
//
//	keys = 0;
//	if (_title != NULL) {
//		size = (int) strlen (_title) + 1;
//		title = new char [size];
//		str_cpy (title, size, _title);
//
//		exe->Show_Message (title);
//	}
//	if (_key1 != NULL) {
//		size = (int) strlen (_key1) + 1;
//		key1 = new char [size];
//		str_cpy (key1, size, _key1);
//		keys++;
//	}
//	if (_key2 != NULL) {
//		size = (int) strlen (_key2) + 1;
//		key2 = new char [size];
//		str_cpy (key2, size, _key2);
//		keys++;
//	}
//
//	//---- print the report ----
//
//	exe->Header_Number (number);
//
//	if (!exe->Break_Check (Num_Records () + 7)) {
//		exe->Print (1);
//		Header ();
//	}
//
//	for (data_ptr = First_Key (); data_ptr; data_ptr = Next_Key ()) {
//		if (data_ptr->Count () == 0) continue;
//
//		if (keys == 2) {
//			exe->Print (1, "%3d-%-3d", (data_ptr->Group () >> 16), (data_ptr->Group () & 0x00FF));
//		} else {
//			exe->Print (1, "%5d  ", data_ptr->Group ());
//		}
//
//		exe->Print (0, " %9d %8d %8d %8d %8d %8.2lf %8.2lf %8.2lf %8.2lf", 
//			data_ptr->Count (), (int) (data_ptr->Min_Distance () + 0.5),
//			(int) (data_ptr->Max_Distance () + 0.5), (int) (data_ptr->Average_Distance () + 0.5),
//			(int) (data_ptr->StdDev_Distance () + 0.5), data_ptr->Min_Time () / 60.0,
//			data_ptr->Max_Time () / 60.0, data_ptr->Average_Time () / 60.0, 
//			data_ptr->StdDev_Time () / 60.0);
//
//		if (total.Count () == 0) {
//			total.Count (data_ptr->Count ());
//			total.Distance (data_ptr->Distance ());
//			total.Time (data_ptr->Time ());
//			total.Distance_Sq (data_ptr->Distance_Sq ());
//			total.Time_Sq (data_ptr->Time_Sq ());
//
//			total.Min_Distance (data_ptr->Min_Distance ());
//			total.Max_Distance (data_ptr->Max_Distance ());
//
//			total.Min_Time (data_ptr->Min_Time ());
//			total.Max_Time (data_ptr->Max_Time ());
//		} else {
//			total.Count (total.Count () + data_ptr->Count ());
//			total.Distance (total.Distance () + data_ptr->Distance ());
//			total.Time (total.Time () + data_ptr->Time ());
//			total.Distance_Sq (total.Distance_Sq () + data_ptr->Distance_Sq ());
//			total.Time_Sq (total.Time_Sq () + data_ptr->Time_Sq ());
//
//			if (total.Min_Distance () > data_ptr->Min_Distance ()) total.Min_Distance (data_ptr->Min_Distance ());
//			if (total.Max_Distance () < data_ptr->Max_Distance ()) total.Max_Distance (data_ptr->Max_Distance ());
//
//			if (total.Min_Time () > data_ptr->Min_Time ()) total.Min_Time (data_ptr->Min_Time ());
//			if (total.Max_Time () < data_ptr->Max_Time ()) total.Max_Time (data_ptr->Max_Time ());
//		}
//	}
//	exe->Print (2, "Total   %9d %8d %8d %8d %8d %8.2lf %8.2lf %8.2lf %8.2lf", 
//		total.Count (), (int) (total.Min_Distance () + 0.5), (int) (total.Max_Distance () + 0.5), 
//		(int) (total.Average_Distance () + 0.5), (int) (total.StdDev_Distance () + 0.5), 
//		total.Min_Time () / 60.0, total.Max_Time () / 60.0, total.Average_Time () / 60.0,
//		total.StdDev_Time () / 60.0);
//
//	exe->Header_Number (0);
//}
//
////---- Header ----
//
//void Trip_Length_Array::Header (void)
//{
//	if (title != NULL) {
//		exe->Print (1, title);
//	} else {
//		exe->Print (1, "Trip Length Summary Report");
//	}
//	if (keys < 2 || key1 == NULL) {
//		exe->Print (2, "%19c", BLANK);
//	} else {
//		exe->Print (2, "%-7.7s%12c", key1, BLANK);
//	}
//	exe->Print (0, "------- Distance (meters) --------  --------- Time (minutes) ---------");
//
//	if (keys == 2 && key2 != NULL) {
//		exe->Print (1, "%-7.7s", key2);
//	} else if (keys == 1 && key1 != NULL) {
//		exe->Print (1, "%-7.7s", key1);
//	} else {
//		exe->Print (1, "Group  ");
//	}
//	exe->Print (0, "     Trips  Minimum  Maximum  Average   StdDev  Minimum  Maximum  Average   StdDev");
//	exe->Print (1);
//}
//
///*********************************************|***********************************************
//
//	[title]
//
//	[key1]             ------- Distance (meters) --------  --------- Time (minutes) ---------
//	[key2]      Trips  Minimum  Maximum  Average   StdDev  Minimum  Maximum  Average   StdDev 
//
//	ddd-ddd ddddddddd  ddddddd  ddddddd  ddddddd  ddddddd  ffff.ff  ffff.ff  ffff.ff  ffff.ff
// 
//	Total   ddddddddd  ddddddd  ddddddd  ddddddd  ddddddd  ffff.ff  ffff.ff  ffff.ff  ffff.ff
//
//**********************************************|***********************************************/ 
