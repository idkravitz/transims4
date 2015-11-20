//*********************************************************
//	SubareaNet.hpp - Create a subarea network files
//*********************************************************

#ifndef SUBAREANET_HPP
#define	SUBAREANET_HPP

#include "Network_Service.hpp"
#include "Integer_Array.hpp"
#include "Arcview_File.hpp"
#include "Db_Array.hpp"

class SubareaNet : public Network_Service
{
public:
	SubareaNet (void);
	virtual ~SubareaNet (void);
	
	virtual void Execute (void);

	virtual bool Node_Processing (Db_File *file);
	virtual bool Zone_Processing (Db_File *file);
	virtual bool Shape_Processing (Db_File *file);
	virtual bool Link_Processing (Db_File *file);
	virtual bool Pocket_Processing (Db_File *file);
	virtual bool Lane_Use_Processing (Db_File *file);
	virtual bool Toll_Processing (Db_File *file);
	virtual bool Connectivity_Processing (Db_File *file);
	virtual bool Turn_Processing (Db_File *file);
	virtual bool Parking_Processing (Db_File *file);
	virtual bool Location_Processing (Db_File *file);
	virtual bool Access_Processing (Db_File *file);
	virtual bool Sign_Processing (Db_File *file);
	virtual bool Signal_Processing (Db_File *file);
	virtual bool Timing_Processing (Db_File *file);
	virtual bool Phasing_Processing (Db_File *file);
	virtual bool Detector_Processing (Db_File *file);
	virtual bool Coordinator_Processing (Db_File *file);
	virtual bool Stop_Processing (Db_File *file);

protected:

	static char * SUBAREA_BOUNDARY_POLYGON;
	static char * EXTERNAL_OFFSET_LENGTH;

	virtual void Program_Control (void);

private:

	int max_location, max_parking, max_access, max_stop, max_route, max_zone;
	int nnode, nzone, nlink, nshort, nboundary, nshape, npocket, nconnect;
	int nlocation, nparking, naccess, nlane_use, nturn, ntoll;
	int nsign, nsignal, ntiming, nphasing, ndetector, ncoord;
	int nstop, nroute, nschedule, ndriver, external_offset;
	int new_location, new_parking, new_access, new_stop, new_route;
	bool transit_flag;

	Arcview_File subarea_boundary;	

	Db_Data_Array node_db, timing_db, coordinator_db;

	void Write_Node (void);
	void Write_Shape (void);
	void Write_Boundary (void);
	void Write_Timing (void);
	void Write_Coordinator (void);
	void Write_Route (void);
};
#endif
