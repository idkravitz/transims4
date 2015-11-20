//*********************************************************
//	Model_Group.hpp - mode choice models by purpose
//*********************************************************

#ifndef MODEL_GROUP_HPP
#define MODEL_GROUP_HPP

#include "Static_Service.hpp"
#include "Pointer_Array.hpp"
#include "Db_File.hpp"
#include "User_Program.hpp"

#define MAX_PURPOSE		100

//---------------------------------------------------------
//	Model_Group class definition
//---------------------------------------------------------

class Model_Group
{
public:
	Model_Group (int group = 0);

	int  Group ()                        { return (group); }
	void Group (int value)               { group = value; }

	bool Script_File (char *type, char *filename);

	bool Compile (int count, Db_Base **files, bool report);

	void Stack_Report (bool header)      { program.Print_Commands (header); }

	int  Random_Seed (int value)         { return (program.Random_Seed (value)); }

	int  Execute (void)                  { return (program.Execute ()); }

private:

	int  group;
	Db_File script_file;
	User_Program program;
};

//---------------------------------------------------------
//	Model_Group_Array
//---------------------------------------------------------

class Model_Group_Array : public Data_Pointer_Array , public Static_Service
{
public:
	Model_Group_Array (int max_records = 0);
	
	virtual void Delete_Pointers (void);

	bool Add (Model_Group *data)          { return (Data_Pointer_Array::Add (data)); }
	
	Model_Group * Record (int index)      { return ((Model_Group *) Data_Pointer_Array::Record (index)); }

	Model_Group * First (void)            { return ((Model_Group *) Data_Pointer_Array::First ()); }
	Model_Group * Next (void)             { return ((Model_Group *) Data_Pointer_Array::Next ()); }

	Model_Group * Last (void)             { return ((Model_Group *) Data_Pointer_Array::Last ()); }
	Model_Group * Previous (void)         { return ((Model_Group *) Data_Pointer_Array::Previous ()); }

	Model_Group * operator[] (int index)  { return (Record (index)); }

	Model_Group * Model (int purpose);

	bool Purpose_Range (char *str_ptr);

private:

	int purpose_map [MAX_PURPOSE];
};

#endif
