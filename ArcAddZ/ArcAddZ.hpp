//*********************************************************
//	ArcAddZ.hpp - Add Z Coordinates to ArcView Shape Files
//*********************************************************

#ifndef ARCADDZ_HPP
#define ARCADDZ_HPP

#include "Execution_Service.hpp"
#include "Arcview_File.hpp"
#include "Data_Array.hpp"
#include "Utility.hpp"

class ArcAddZ : public Execution_Service
{
public:
	ArcAddZ (void);
	virtual ~ArcAddZ (void);

	virtual void Execute (void);

protected:

	static char * ARCVIEW_SHAPE_FILE;
	static char * NEW_ARCVIEW_SHAPE_FILE;
	static char * ARCVIEW_SHAPE_FILE_x;
	static char * NEW_ARCVIEW_SHAPE_FILE_x;
	static char * ADD_Z_AS_MEASURE;
	static char * DEFAULT_Z_VALUE;

	virtual void Program_Control (void);

private:
	bool measure_flag;
	double z_coord;

	typedef struct {
		int group_num;
		Arcview_File input;
		Arcview_File output;
	} File_Group;

	Data_Array file_group;

	void Copy_Shape (File_Group *arcview);
};
#endif
