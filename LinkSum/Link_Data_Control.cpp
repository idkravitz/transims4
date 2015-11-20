//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Link_Data_Control
//---------------------------------------------------------

void LinkSum::Link_Data_Control (char *name, char *file, char *format, char *field)
{
	Data_Group file_rec;
	Range_Data *range_ptr;

	Print (1);
	file_rec.file = new Link_Data_File ();
	file_rec.field = 0;

	file_rec.file->File_Type (name);
	file_rec.file->Filename (Project_Filename (file));
	file_rec.field = Performance_Code (field);

	switch (file_rec.field) {
		case VC_DATA:
		case RATIO_DATA:
		case QUEUE_DATA:
		case DENSITY_DATA:
		case MAX_DENSITY_DATA:
		case CONG_TIME_DATA:
			file_rec.file->Num_Decimals (2);
			break;
		case TTIME_DATA:
		case SPEED_DATA:
		case DELAY_DATA:
		case VMT_DATA:
		case VHT_DATA:
		case CONG_VMT_DATA:
		case CONG_VHT_DATA:
			file_rec.file->Num_Decimals (1);
			break;
		default:
			file_rec.file->Num_Decimals (0);
			break;
	}

	if (format != NULL) {
		file_rec.file->Dbase_Format (format);
	} else {
		format = Get_Control_String (DEFAULT_OUTPUT_FORMAT);

		if (format != NULL) {
			file_rec.file->Dbase_Format (format);
		}
	}
	file_rec.file->Create ();

	for (range_ptr = time_periods.First (); range_ptr; range_ptr = time_periods.Next ()) {
		file_rec.file->Add (range_ptr);
	}
	file_rec.file->Create_Fields ();
	file_rec.file->Write_Header ();

	if (!data_group.Add (&file_rec)) {
		Error ("Creating Link Data File");
	}
}
