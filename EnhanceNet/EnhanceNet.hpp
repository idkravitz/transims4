//*********************************************************
//	EnhanceNet.hpp - enhance network files
//*********************************************************

#ifndef ENHANCENET_HPP
#define ENHANCENET_HPP

#include "Network_Service.hpp"
#include "Node_List.hpp"
#include "Dir_Connect.hpp"
#include "Db_Array.hpp"

#undef  node_data
#define node_data		(*((Node_List_Array *) node_array))

#undef  dir_data
#define dir_data		(*((Dir_Connect_Array *) dir_array))

/**This class provides functional services to enhance network files.
 *
 * The main functional services provided by this class are:
 * 1)read the network files
 * 2)prepare the data
 * 3)process the results
 */
class EnhanceNet : public Network_Service
{
public:
	/**The constructor*/
	EnhanceNet (void);
	/**The destructor*/
	virtual ~EnhanceNet (void);

	/**The main execution procedure for this class.
	*
	* The main steps are:
	* 1)read the network files
	* 2)prepare the data
	* 3)process the results
	*/
	virtual void Execute (void);
	
	/**Perform additonal link processing.
	*
	* The main steps are:
	* 1)Perform additonal link processing
	*
	* @param file - the file to process
	*/	
	virtual bool Link_Processing (Db_File *file);

	/**Perform additional lane connectivity processing.
	*
	* The main steps are:
	* 1)Perform additional lane connectivity processing.
	*
	* @param file - the file to process
	*/	
	virtual bool Connectivity_Processing (Db_File *file);

protected:
	
	/** Control the program execution.
     *
     * This procedure opens all the input and output files, read in all the control 
	 * parameters from the input file, and make the data available to other
	 * functions in the module. 
     */
	virtual void Program_Control (void);

private:
	/**The connect_flag.*/
	bool connect_flag;
	/**The type_flag.*/
	bool type_flag;
	/**The connect_db.*/
	Db_Data_Array connect_db;

	/**Set the node and link data.
	*
	* The main steps are:
	* 1)initialize the node data 
	* 2)build the link list
	* 3)add to the link lists
	*/	
	void Data_Setup (void);

	/**Build connection lists.
	*
	* The main steps are:
	* 1)set connections for each entry link: set the thru link: check the number of lanes,  
	*     check the facility types, check speeds, set the link relationships, set connections for each exit link
	* 2)set connections for each exit link: set the thru link, check the number of lanes, 
	*     check the facility types,check speeds, set the link relationships 
	* 3)coordinate the approach thru movements
	* 4)coordinate the departure thru movements 
	* 5)set the connection type
	* 6)check for diverge movements 
	*/	
	void Connection_List (void);

	/**Write the lane connectivity data.
	*
	* The main steps are:
	* 1)input link dir
	* 2)output link dir 
	* 3)retrieve the connection record 
	* 4)update the connection type
	*/	
	void Write_Connect (void);
};
#endif

