//*********************************************************
//	Report_Service.hpp - general report class
//*********************************************************

#ifndef REPORT_SERVICE_HPP
#define REPORT_SERVICE_HPP

#include "Message_Service.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Report_Service - general message class
//---------------------------------------------------------

class Report_Service : public Message_Service
{
public:

	Report_Service (void);
	virtual ~Report_Service (void);

	const char * Control_File (void)		{ return (control_file); }
	void  Control_File (const char *name)	{ control_file = name; }
	
	const char * Title (void)				{ return ((title != NULL) ? title : ""); }
	void  Title (char *name);

	void   Extension (char * value)			{ str_cpy (extension, sizeof (extension), value); }
	char * Extension (void)					{ return (extension); }
	
	bool  Enable_Parameter (void)			{ return (parameter); }
	void  Enable_Parameter (bool flag)		{ parameter = flag; }

	bool  Enable_Partitions (void)			{ return (enable); }
	void  Enable_Partitions (bool flag)		{ enable = flag; }

	bool  Partition (void)					{ return (partition); }
	void  Partition (bool flag)				{ partition = flag; };

	int   Partition_Number (void)			{ return (partition_number); }
	void  Partition_Number (int num)		{ partition_number = num; }

	void  Exit_Stat (Exit_Code stat);

	void  Write (int num_lines, char *format, ...);
	void  Write (int num_lines);

	void  Print (int num_lines, char *format, ...);
	void  Print (int num_lines);

	void  Warning (char *format, ...);
	void  Error (char *format, ...);

	void  Print_Filename (int num_lines, char *label, char *filename);
	void  File_Error (char *label, char *format, ...);
	
	void  Write_Time (int num_lines, char *format, ...);

	void  Header_Number (int number)		{ header = number; }
	void  Print_Banner (void);
	bool  Break_Check (int num_lines);
	void  Page_Size (int width, int lines);
	void  New_Page (void);
	void  Flush (void);

	int   Page_Lines (void)					{ return (page_lines); }
	int   Page_Width (void)					{ return (page_width); }

	virtual void Page_Header ()				{ };

	char * Fix_Directory (char dir []);

protected:
	void Open_Report (char *report_name, bool report_flag);
	bool Page_Check (int new_lines);

	int  Header_Number (void)				{ return (header); }

private:
	FILE *report;

	const char *title, *control_file;
	int lines, page, page_width, page_lines, header;

	static char extension [40];
	bool parameter, partition, enable;
	int	 partition_number;
};
#endif
