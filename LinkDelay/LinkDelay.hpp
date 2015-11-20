//*********************************************************
//	LinkDelay.hpp - manipulate the link delay file
//*********************************************************

#ifndef LINKDELAY_HPP
#define LINKDELAY_HPP

#include "Demand_Service.hpp"
#include "Delay_File.hpp"
#include "Link_File.hpp"
#include "Time_Step.hpp"
#include "Smooth_Data.hpp"

class LinkDelay : public Demand_Service
{
public:

	LinkDelay (void);
	virtual ~LinkDelay (void);
	
	virtual void Execute (void);

protected:
	static char * PREVIOUS_LINK_DELAY_FILE;
	static char * PREVIOUS_LINK_DELAY_FORMAT;
	static char * PREVIOUS_WEIGHTING_FACTOR;
	static char * PREVIOUS_LINK_FILE;
	static char * TIME_OF_DAY_FORMAT;
	static char * PROCESSING_METHOD;
	static char * SMOOTH_GROUP_SIZE;
	static char * PERCENT_MOVED_FORWARD;
	static char * PERCENT_MOVED_BACKWARD;
	static char * NUMBER_OF_ITERATIONS;
	static char * CIRCULAR_GROUP_FLAG;
	static char * TIME_PERIOD_SORT;

	virtual void Program_Control (void);
	virtual bool Delay_Processing (Db_File *file);

private:
	enum Combine_Method { REPLACE_LINKS, SIMPLE_AVERAGE, WEIGHTED_AVERAGE };

	Link_File previous_link;
	Delay_File previous_delay;

	int naverage, niter, increment, nerror;
	double forward, backward, factor;	
	bool loop_flag, previous_flag, sort_flag, link_flag;

	Link_Array previous_data;
	AB_Key ab_array;

	Combine_Method method;
	Time_Step time_step;
	Smooth_Data smooth_data;

	void Read_Previous_Link (void);	

	void Combine_Delay (void);
	
	void Output_Delay (void);

	void Error_Check (void);
	
	void Smooth_Delay (TTime_Data *ttime_ptr, int num, Connect_Time **turn_ptr);
};
#endif
