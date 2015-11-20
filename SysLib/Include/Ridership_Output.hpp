//*********************************************************
//	Ridership_Output.hpp - Output Interface Class
//*********************************************************

#ifndef RIDERSHIP_OUTPUT_HPP
#define RIDERSHIP_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Ridership_File.hpp"

//---------------------------------------------------------
//	Ridership_Output Class definition
//---------------------------------------------------------

class Ridership_Output : public Static_Service
{
public:
	Ridership_Output (void);
	virtual ~Ridership_Output (void);

	void Add_Keys (void);

	void Read_Control (int steps_per_second = 1);

	void Output (void);

	void Close (void);

private:
	static char * OUTPUT_RIDERSHIP_FILE_x;
	static char * OUTPUT_RIDERSHIP_FORMAT_x;
	static char * OUTPUT_RIDERSHIP_TIME_FORMAT_x;
	static char * OUTPUT_RIDERSHIP_TIME_RANGE_x;
	static char * OUTPUT_RIDERSHIP_ROUTE_RANGE_x;
	static char * OUTPUT_RIDERSHIP_ALL_STOPS_x;

	typedef struct {
		Time_Range     *time_range;
		Data_Range     *route_range;
		Ridership_File *file;
		bool            all_stops;
	} Output_Data;

	Data_Array output_data;
	bool output_flag;
};

#endif
