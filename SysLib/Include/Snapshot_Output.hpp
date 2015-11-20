//*********************************************************
//	Snapshot_Output.hpp - Output Interface Class
//*********************************************************

#ifndef SNAPSHOT_OUTPUT_HPP
#define SNAPSHOT_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Snapshot_File.hpp"

//---------------------------------------------------------
//	Snapshot_Output Class definition
//---------------------------------------------------------

class Snapshot_Output : public Static_Service
{
public:
	Snapshot_Output (void);
	virtual ~Snapshot_Output (void);

	void Add_Keys (void);

	void Read_Control (int steps_per_second = 1);

	bool In_Range (int step, int dir);

	void Output (void);

	void Close (void);

	void Vehicle (int value)		{ vehicle = value; }
	void Time (int value)			{ time = value; }
	void Dir (int value)			{ dir = value; }
	void Lane (int value)			{ lane = value; }
	void Offset (double value)		{ offset = value; }
	void Speed (double value)		{ speed = value; }
	void Accel (double value)		{ accel = value; }
	void VehType (int value)		{ vehtype = value; }
	void Driver (int value)			{ driver = value; }
	void Passengers (int value)		{ pass = value; }

private:
	static char * OUTPUT_SNAPSHOT_FILE_x;
	static char * OUTPUT_SNAPSHOT_FORMAT_x;
	static char * OUTPUT_SNAPSHOT_TIME_FORMAT_x;
	static char * OUTPUT_SNAPSHOT_INCREMENT_x;
	static char * OUTPUT_SNAPSHOT_TIME_RANGE_x;
	static char * OUTPUT_SNAPSHOT_LINK_RANGE_x;
	static char * OUTPUT_SNAPSHOT_COORDINATES_x;
	static char * OUTPUT_SNAPSHOT_MAX_SIZE_x;
	static char * OUTPUT_SNAPSHOT_LOCATION_FLAG_x;

	typedef struct {
		Snapshot_File *file;
		Time_Range *time_range;
		Data_Range *link_range;
		int x1, y1, x2, y2;		//---- rounded ----
		bool coord_flag;
		bool size_flag;
		int  max_size;
	} Output_Data;

	Data_Array output_data;

	bool output_flag;
	int vehicle, time, dir, lane, vehtype, driver, pass;
	double speed, offset, accel;
};

#endif
