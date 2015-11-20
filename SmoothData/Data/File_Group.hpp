//*********************************************************
//	File_Group.hpp - diurnal control parameters
//*********************************************************

#ifndef FILE_GROUP_HPP
#define FILE_GROUP_HPP

#include "Data_Array.hpp"
#include "Diurnal_File.hpp"
#include "Time_Step.hpp"

//---------------------------------------------------------
//	File_Group class definition
//---------------------------------------------------------

class File_Group
{
public:
	File_Group (void);
	virtual ~File_Group (void)             { Clear (); }

	int    Group ()                        { return (group); }
	Diurnal_File * Input_File ()           { return (input_file); }
	Diurnal_File * Output_File ()          { return (output_file); }
	Diurnal_File * Initial_File ()         { return (initial_file); }
	int    Input_Time (char *string)       { return (input_time.Step (string)); }
	char * Output_Time (int value)         { return (output_time.Format_Time (value)); }
	int    Field_Number ()                 { return (nfield); }
	int    Num_Average ()                  { return (naverage); }
	int    Num_Iteration ()                { return (niter); }
	int    Increment ()                    { return (increment); }
	double Forward ()                      { return (forward); }
	double Backward ()                     { return (backward); }
	bool   Loop_Flag ()                    { return (loop_flag); }
	bool   Replicate_Flag ()               { return (replicate); }

	void Group (int value)                 { group = value; }
	void Input_File (Diurnal_File *file)   { input_file = file; }
	void Output_File (Diurnal_File *file)  { output_file = file; }
	void Initial_File (Diurnal_File *file) { initial_file = file; }
	bool Input_Format (char *format)       { input_time.Step_Size (1); return (input_time.Format (format)); }
	bool Output_Format (char *format)      { output_time.Step_Size (1); return (output_time.Format (format)); }
	void Field_Number (int value)          { nfield = value; }
	void Num_Average (int value)           { naverage = value; }
	void Num_Iteration (int value)         { niter = value; }
	void Increment (int value)             { increment = value; }
	void Forward (double value)            { forward = value; }
	void Backward (double value)           { backward = value; }
	void Loop_Flag (bool flag)             { loop_flag = flag; }
	void Replicate_Flag (bool flag)        { replicate = flag; }

	void Clear (void);

private:
	Diurnal_File *input_file;
	Diurnal_File *output_file;
	Diurnal_File *initial_file;
	Time_Step input_time;
	Time_Step output_time;
	int group;
	int nfield;
	int naverage;
	int niter;
	int increment;
	double forward;
	double backward;	
	bool loop_flag;
	bool replicate;
};

//---------------------------------------------------------
//	File_Group_Array
//---------------------------------------------------------

class File_Group_Array : public Data_Array
{
public:
	File_Group_Array (int max_records = 0);
	virtual ~File_Group_Array (void);

	bool Add (File_Group *data = NULL)   { return (Data_Array::Add ((void *) data)); }

	File_Group * New_Record (bool clear = false, int number = 1)
										 { return ((File_Group *) Data_Array::New_Record (clear, number)); }

	File_Group * First (void)            { return ((File_Group *) Data_Array::First ()); }
	File_Group * Next (void)             { return ((File_Group *) Data_Array::Next ()); }

	File_Group * Last (void)             { return ((File_Group *) Data_Array::Last ()); }
	File_Group * Previous (void)         { return ((File_Group *) Data_Array::Previous ()); }

	File_Group * operator[] (int index)  { return ((File_Group *) Record (index)); }
};

#endif

