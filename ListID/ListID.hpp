//*********************************************************
//	ListID.hpp - extract record IDs from a file
//*********************************************************

#ifndef LISTID_HPP
#define LISTID_HPP

#include "Execution_Service.hpp"
#include "Db_File.hpp"
#include "Utility.hpp"

/** This class provides functional services to extract record IDs from a file.
 *
 * The main functional services provided by this class are:
 * 1)Process the control parameters
 * 2)Read/Extract IDs from the input file
 * 3)Write the extracted IDs to the output file
 */
class ListID : public Execution_Service
{
public:
	/**The constructor*/
	ListID (void);
	/**The destructor*/
	virtual ~ListID (void);

	/**The main execution procedure for the ListID module.
	 *
	 * The main steps are:
	 * 1)Read/Extract IDs from the input file
	 * 2)Write the extracted IDs to the output file
	 */
	virtual void Execute (void);

protected:
	/**The data input file to be read.*/
	static char * INPUT_DATA_FILE;
	/**The output file to contain the extracted record IDs.*/
	static char * OUTPUT_ID_FILE;
	/**The number of header records*/
	static char * NUM_HEADER_RECORDS;

	/** Control the program execution.
     *
     * This procedure read in all the control parameters from the input
	 * file, and make the data available to other functions in the  
	 * module. 
     */ 
	virtual void Program_Control (void);

private:
	/**the record header number.*/
	int num_head;
	/**The input data file.*/
	Db_File input_file;
	/**The output data file.*/
	Db_File	output_file;
};
#endif
