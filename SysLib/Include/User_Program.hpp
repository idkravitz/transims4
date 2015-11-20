//*********************************************************
//	User_Program.hpp - Processes User Programs
//*********************************************************

#ifndef USER_PROGRAM_HPP
#define USER_PROGRAM_HPP

#include "Program_Code.hpp"
#include "Integer_Array.hpp"
#include "Db_Base.hpp"
#include "Utility.hpp"
#include "Random.hpp"

//---- space allocation parameters ----

#define	MAX_LEVEL		100
#define	MAX_NEST		100
#define MAX_LOOP		10
#define MAX_BREAK		10
#define	MAX_TABLE		10

//---------------------------------------------------------
//	User_Program Class definition
//---------------------------------------------------------

class User_Program : public Program_Code
{
public:
	User_Program (void);
	User_Program (int count, Db_Base *file1, ...);
	User_Program (int count, Db_Base **files);
	~User_Program (void)							{ Clear (); }

	bool Set_Files (int count, Db_Base *file1, ...);
	bool Set_Files (int count, Db_Base **files);

	bool Compile (char *command);
	bool Compile (Db_File *script, bool list_flag);
	int	 Execute (int record = -1);
	void Print_Commands (bool header = true);
	void List_Commands (char *command);
	int  Random_Seed (int seed = 0)					{ return (random.Seed (seed)); }

	void Clear (void);

protected:
	bool Initialize (int count);
			
	int num_files;
	Db_Base **file;
	int *file_type, *read_only;
		
private:

	Token_Data *token_ptr;
	Command_Data cmd, *cmd_ptr;
	Stack_Data *s, *s1;

	Stack_Array stack;
	Command_Array command, token_level;
	Integer_Array lvalue;
	Double_Array fvalue;
	Svalue_Array svalue;
	Svalue_Index_Array svalue_index;

	Random random;

	int	*file_stat, *out_flag;

	char *buf, buffer [STRING_BUFFER], token [FIELD_BUFFER];

	int record, line_num, level, assign_set, last_value, first_read, if_level;
	int nest, if_jump [MAX_NEST], then_jump [MAX_NEST];
	int loop, while_jump [MAX_LOOP], loop_jump [MAX_LOOP];
	int num_break [MAX_LOOP], break_jump [MAX_LOOP] [MAX_BREAK];

	Integer_List table [3] [MAX_TABLE];

	bool table_flag, declare_flag, ascii_write, open_par;
	int table_type, declare_type;
	int sindex;

	bool Assign (void);
	bool Relation (void);
	bool Math (void);
	bool Function (void);
	bool Convert (void);
	bool Date_Time (void);
	bool Input_Output (void);
	bool Table (void);
	bool Read_Value (void);

	bool Initial_Table (void);
	bool Initial_Declare (void);

	bool Get_Token (void);
	bool Process_Token (void);
	bool Process_Command (int type);

	bool Syntax_Error (char *text);
};
#endif
