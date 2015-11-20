//*********************************************************
//	Execution_Service.hpp - general execution class
//*********************************************************

#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include "Report_Service.hpp"
#include "System_Defines.hpp"
#include "Control_List.hpp"

//---------------------------------------------------------
//	Report_Service - general message class
//---------------------------------------------------------

class Execution_Service : public Report_Service, public Control_List
{
public:
	
	Execution_Service (void);
	virtual ~Execution_Service (void);

	void Key_List (char *keys []);
	void Report_List (char *names []);

	bool   Report_Flag (int type);
	int    Report_Type (int number);
	double Report_Data (int number = 0);

	int  First_Report (void);
	int  Next_Report (void);

	void   Project_Directory (char *dir);
	char * Project_Directory (void)			{ return (project_directory); }

	char * Project_Filename (char *name, char *ext = NULL)	
											{ return (Filename (Project_Directory (), name, ext)); }

	virtual void Execute (void);

	int Start_Execution (int commands, char *command []);

protected:
	static char *TITLE, *REPORT_FILE, *REPORT_FLAG, *PROJECT_DIRECTORY, *DEFAULT_FILE_FORMAT;
	static char *MAX_WARNING_MESSAGES, *MAX_WARNING_EXIT_FLAG, *TRAVELER_SCALING_FACTOR;

	virtual void Program_Control (void);
	void List_Reports (void);

	char * Filename (char *dir, char *name, char *ext = NULL);

private:
	typedef struct {
		char *name;
		bool flag;
	} Report_Name;

	typedef struct {
		int  type;
		double data;
	} Report_Number;

	bool Command_Line (int commands, char *command []);
	void Show_Syntax (void);

	void Syntax_Help (void);
	void Key_Check (void);

	void Read_Reports (void);
	bool report_flag;

	char report_key [FIELD_BUFFER];
	char *project_directory;

	static char filename [FILE_NAME_SIZE];

	Data_Array report_name;
	Data_Array report_number;
};
#endif
