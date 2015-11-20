//*********************************************************
//	Traveler_Output.hpp - Output Interface Class
//*********************************************************

#ifndef TRAVELER_OUTPUT_HPP
#define TRAVELER_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Traveler_File.hpp"

//---------------------------------------------------------
//	Traveler_Output Class definition
//---------------------------------------------------------

class Traveler_Output : public Static_Service
{
public:
	Traveler_Output (void);
	virtual ~Traveler_Output (void);

	void Add_Keys (void);

	void Read_Control (int steps_per_second = 1);

	bool In_Range (int traveler, int step, int dir);

	void Output (void);

	void Close (void);

	void Traveler (int value)		{ traveler = value; }
	void Vehicle (int value)		{ vehicle = value; }
	void Time (int value)			{ time = value; }
	void From_Dir (int value)		{ from_dir = value; }
	void From_Offset (int value)	{ from_offset = value; }
	void To_Dir (int value)			{ to_dir = value; }
	void To_Offset (int value)		{ to_offset = value; }
	void Speed (int value)			{ speed = value; }
	void Lane (int value)			{ lane = value; }

private:
	static char * OUTPUT_TRAVELER_RANGE_x;
	static char * OUTPUT_TRAVELER_FILE_x;
	static char * OUTPUT_TRAVELER_FORMAT_x;
	static char * OUTPUT_TRAVELER_TIME_FORMAT_x;
	static char * OUTPUT_TRAVELER_TIME_RANGE_x;
	static char * OUTPUT_TRAVELER_LINK_RANGE_x;

	typedef struct {
		Data_Range    *range;
		Time_Range    *time_range;
		Data_Range    *link_range;
		Traveler_File *file;
		bool          file_flag;
	} Output_Data;

	Data_Array output_data;

	bool output_flag;
	int traveler, vehicle, time, from_dir, from_offset, to_dir, to_offset, speed, lane;
};

#endif
