//*********************************************************
//	Message_Service.hpp - general message class
//*********************************************************

#ifndef MESSAGE_SERVICE_HPP
#define MESSAGE_SERVICE_HPP

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//---------------------------------------------------------
//	Message_Service - general message class
//---------------------------------------------------------

class Message_Service
{
public:

	Message_Service (void);
	virtual ~Message_Service (void);

	enum Exit_Code {DONE, FATAL, WARNING, CANCEL, RUNNING, DONEX};

	Exit_Code Program_Status (void)			{ return (status); }
	void Program_Status (Exit_Code stat)	{ status = stat; }

	bool Quiet (void)						{ return (quiet); }
	void Quiet (bool flag)					{ quiet = flag; }

	const char * Program (void)				{ return (program); }
	void Program (const char *p)			{ program = p; }

	const char * Version (void)				{ return (version); }
	void Version (const char *v)			{ version = v; }

	const char * CopyRight (void)			{ return (copyright); }
	void CopyRight (const char *c)			{ copyright = c; }

	time_t StartTime (void)					{ return (starttime); }
	void StartTime (time_t dt)				{ starttime = dt; }

	void Show_Banner (void);

	void Show_Message (char *format, ...);
	void Show_Message (int num_lines, char *format, ...);
	void Show_Message (int num_lines);

	bool Show_Question (char *format, ...); 
	void Show_Error (char *format, ...);
	void Show_Warning (char *format, ...);

	void Set_Progress (int step_size = 5000);
	void Show_Progress (int value = 0);
	void Show_Progress (char *text);
	void End_Progress (void);
	void End_Progress (char *text);
	int  Progress_Count (void)				{ return (progress); }

	void Pause_Process (void);

	char * Get_Control_File (void);

	int Return_Code (void);

	virtual void Exit_Stat (Exit_Code stat);

protected:
	void Banner (FILE *file = NULL);

	int  Warnings (void)			{ return (warnings); }
	void Warnings (int value)		{ warnings = value; }

	int  Max_Warnings (void)		{ return (max_warnings); }
	void Max_Warnings (int value)	{ max_warnings = value; }

	bool Warning_Flag (void)		{ return (warning_flag); }
	void Warning_Flag (bool flag)	{ warning_flag = flag; }

	bool Pause (void)				{ return (pause); }
	void Pause (bool flag)			{ pause = flag; }

	bool No_Pause (void)			{ return (no_pause); }
	void No_Pause (bool flag)		{ no_pause = flag; }

	bool Batch (void)				{ return (batch); }
	void Batch (bool flag)			{ batch = flag; }

private:
	bool quiet, pause, no_pause, batch;
	Exit_Code status;

	const char *program;
	const char *version;
	const char *copyright;
	time_t starttime;

	int progress;
	int progress_step;
	int warnings, max_warnings;
	bool warning_flag;
};
#endif
