//*********************************************************
//	Occupancy_Output.hpp - Output Interface Class
//*********************************************************

#ifndef OCCUPANCY_OUTPUT_HPP
#define OCCUPANCY_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Occupancy_File.hpp"

#pragma pack (push, 4)

//---------------------------------------------------------
//	Occupancy_Output Class definition
//---------------------------------------------------------

class Occupancy_Output : public Static_Service
{
public:
	Occupancy_Output (void);
	virtual ~Occupancy_Output (void);

	void Add_Keys (void);

	void Read_Control (void);

	void Initialize_Cells (void);

	void Output_Check (void);

	void Close (void);

private:
	static char * OUTPUT_OCCUPANCY_FILE_x;
	static char * OUTPUT_OCCUPANCY_FORMAT_x;
	static char * OUTPUT_OCCUPANCY_TIME_FORMAT_x;
	static char * OUTPUT_OCCUPANCY_INCREMENT_x;
	static char * OUTPUT_OCCUPANCY_TIME_RANGE_x;
	static char * OUTPUT_OCCUPANCY_LINK_RANGE_x;
	static char * OUTPUT_OCCUPANCY_MAX_FLAG_x;
	static char * OUTPUT_OCCUPANCY_COORDINATES_x;

	typedef struct {
		int lanes;
		int cells;
		int total;
		int *data;
	} Cell_Data;

	typedef struct {
		Occupancy_File *file;
		Time_Range *time_range;
		Data_Range *link_range;
		Data_Array *cell_data;
		int start_time;
		int end_time;
		int x1, y1, x2, y2;		//---- rounded ----
		bool coord_flag;
		bool max_flag;
	} Output_Data;

	Data_Array output_data;

	double cell_size;
	bool output_flag;

	void Write_Occupancy (Output_Data *output);
};
#pragma pack (pop)
#endif
