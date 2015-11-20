//*********************************************************
//	Control_List.hpp - list of control keys
//*********************************************************

#ifndef CONTROL_LIST_HPP
#define CONTROL_LIST_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Control_List -- class definition
//---------------------------------------------------------

class Control_List : public Data_Array
{
public:
	Control_List (void);
	~Control_List (void);

	bool   Read_Control_File (char *control_file);

	void   Add_Control_Key (char *key);

	char * Get_Control_String (char *key, ...);
	bool   Get_Control_Flag (char *key, ...); 

	int    Highest_Control_Group (char *key, ...);
	int    Number_Control_Group (char *key, ...);

	char * Current_Key (void)			{ return (current_key); }
	void   Current_Key (char *key)		{ current_key = key; }

	char * First_Key (void);
	char * Next_Key (void);
	
	char * First_Skip_Key (void);
	char * Next_Skip_Key (void);

	void   Disable_Control_Key (char *key);
	void   Activate_Control_Key (char *key);
	
	void   Clean (void);

private:
	typedef struct {
		char *key;
		bool nest;
		bool active;
		union {
			char *line;
			Control_List *group;
		};
	} Control;

	char *current_key;
	int skip_offset;
};
#endif
