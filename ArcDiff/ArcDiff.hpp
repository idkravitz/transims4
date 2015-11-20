//*********************************************************
//	ArcDiff.hpp - ArcView Link Difference Files
//*********************************************************

#ifndef ARCDIFF_HPP
#define ARCDIFF_HPP

#include "Execution_Service.hpp"
#include "Arcview_File.hpp"
#include "Data_Array.hpp"
#include "Utility.hpp"

class ArcDiff : public Execution_Service
{
public:
	ArcDiff (void);
	virtual ~ArcDiff (void);

	virtual void Execute (void);

protected:

	static char * ARCVIEW_DELAY_FILE_1;
	static char * ARCVIEW_DELAY_FILE_2;
	static char * ARCVIEW_LINK_DIR_FILE_1;
	static char * ARCVIEW_LINK_DIR_FILE_2;
	static char * ARCVIEW_RIDERSHIP_FILE_1;
	static char * ARCVIEW_RIDERSHIP_FILE_2;
	static char * NEW_ARCVIEW_DIFF_FILE;
	static char * ARCVIEW_DELAY_FILE_1_x;
	static char * ARCVIEW_DELAY_FILE_2_x;
	static char * ARCVIEW_LINK_DIR_FILE_1_x;
	static char * ARCVIEW_LINK_DIR_FILE_2_x;
	static char * ARCVIEW_RIDERSHIP_FILE_1_x;
	static char * ARCVIEW_RIDERSHIP_FILE_2_x;
	static char * NEW_ARCVIEW_DIFF_FILE_x;
	static char * PERCENT_DIFFERENCE_FLAG;

	virtual void Program_Control (void);

private:

	enum Group_Types { DELAY_TYPE, LINK_DIR_TYPE, RIDER_TYPE };

	bool percent_flag;

	typedef struct {
		int group_num;
		Arcview_File delay1;
		Arcview_File delay2;
		Arcview_File diff;
	} Delay_Group;

	Data_Array delay_group;

	void Merge_Delay (Delay_Group *arcview);
};
#endif
