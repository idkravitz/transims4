//********************************************************* 
//	Arcview_File.cpp - Arcview Shape File interface
//*********************************************************

#include "Arcview_File.hpp"

#include "Projection_Service.hpp"
#include "Utility.hpp"

//-----------------------------------------------------------
//	Arcview_File constructors - access only
//-----------------------------------------------------------

Arcview_File::Arcview_File (Access_Type access) : 
	Db_Header (access, DBASE), Projection ()
{
	Initialize ();
}

//-----------------------------------------------------------
//	Arcview_File constructors - with filename
//-----------------------------------------------------------

Arcview_File::Arcview_File (char *filename, Access_Type access) : 
	Db_Header (access, DBASE), Projection ()
{
	Initialize ();

	Open (filename);
}

//-----------------------------------------------------------
//	Arcview_File destructor
//-----------------------------------------------------------

Arcview_File::~Arcview_File (void)
{
	Close ();
}

//-----------------------------------------------------------
//	Initialize
//-----------------------------------------------------------

void Arcview_File::Initialize (void)
{
	shape_type = VECTOR;
	z_flag = m_flag = false;

	Num_Points (1);
	Num_Parts (1);

	File_Type ("Arcview Shape File");

	shape_file.Initialize (Access_Type (), BINARY);
	shape_file.File_Type (File_Type ());
	shape_file.Send_Messages (Send_Messages ());

	memset (&shape_header, '\0', sizeof (shape_header));
	shape_header.file_code = 9994;
	shape_header.file_size = sizeof (shape_header) / sizeof (short);
	shape_header.version = 1000;
	shape_header.shape_type = ARC_TYPE;
	shape_header.box.xmin = shape_header.box.ymin = -NAN;
	shape_header.box.xmax = shape_header.box.ymax = NAN;
	
	if (Z_Flag ()) {
		shape_header.zbox.min = shape_header.mbox.min = -NAN;
		shape_header.zbox.max = shape_header.mbox.max = NAN;
	}
	index_file.Initialize (Access_Type (), BINARY);
	index_file.File_Type (File_Type ());
	index_file.Send_Messages (Send_Messages ());

	memset (&index_header, '\0', sizeof (index_header));
	index_header.file_code = 9994;
	index_header.file_size = sizeof (index_header) / sizeof (short);
	index_header.version = 1000;

	parts.Add (0);
}

//-----------------------------------------------------------
//	Open
//-----------------------------------------------------------

bool Arcview_File::Open (char *filename)
{
	int n;
	char pathname [FILE_NAME_SIZE];
	bool messages, status;

	if (filename == NULL) {
		filename = Filename ();
		if (filename == NULL) return (false);
	}
	str_cpy (pathname, sizeof (pathname), filename);

	messages = Send_Messages ();
	Send_Messages (false);

	shape_file.File_Type (File_Type ());
	shape_file.File_Access (File_Access ());

	index_file.File_Type (File_Type ());
	index_file.File_Access (File_Access ());
	index_file.Send_Messages (false);

	//---- open the shape file ----

	if (!shape_file.Open (pathname)) {
		return (Status (NOT_OPEN));
	}
	if (File_Access () == CREATE) {
		Write_Projection ();

		switch (Shape_Type ()) {
			case DOT:
				shape_header.shape_type = POINT_TYPE;
				break;
			case MDOTS:
				shape_header.shape_type = MULTIPOINT_TYPE;
				break;
			case LINE:
			case VECTOR:
				shape_header.shape_type = ARC_TYPE;
				break;
			case POLYGON:
				shape_header.shape_type = POLYGON_TYPE;
				break;
			default:
				break;
		}
		if (Z_Flag () || M_Flag ()) {
			if (!Z_Flag ()) {
				Z_Flag (true);
			}
			shape_header.shape_type += (M_Flag ()) ? 20 : 10;
		}
		shape_header.file_size = sizeof (shape_header) / sizeof (short);
		shape_header.box.xmin = shape_header.box.ymin = -NAN;
		shape_header.box.xmax = shape_header.box.ymax = NAN;

		if (Z_Flag ()) {
			shape_header.zbox.min = shape_header.mbox.min = -NAN;
			shape_header.zbox.max = shape_header.mbox.max = NAN;
		}
		Reorder_Bits (&shape_header, 7);

		if (!shape_file.Write (&shape_header, sizeof (shape_header), 0L)) {
			return (Status (HEADER_WRITE));
		}
	} else {
		if (!shape_file.Read (&shape_header, sizeof (shape_header), 0L)) {
			return (Status (HEADER_READ));
		}
	}
	Reorder_Bits (&shape_header, 7);

	switch ((shape_header.shape_type % 10)) {
		case POINT_TYPE:		//---- point ----
			Shape_Type (DOT);
			break;
		case MULTIPOINT_TYPE:	//---- multipoint ----
			Shape_Type (MDOTS);
			break;
		case ARC_TYPE:			//---- arc ----
			Shape_Type (VECTOR);
			break;
		case POLYGON_TYPE:		//---- polygon ----
			Shape_Type (POLYGON);
			break;
		default:
			return (Status (HEADER_READ));
	}
	Z_Flag ((shape_header.shape_type / 10) > 0);
	M_Flag ((shape_header.shape_type / 10) == 2);

	extent.xmin = shape_header.box.xmin;
	extent.ymin = shape_header.box.ymin;
	extent.xmax = shape_header.box.xmax;
	extent.ymax = shape_header.box.ymax;

	Convert (&(extent.xmin), &(extent.ymin));
	Convert (&(extent.xmax), &(extent.ymax));

	if (M_Flag ()) {
		_extent.min = shape_header.mbox.min;
		_extent.max = shape_header.mbox.max;
	} else if (Z_Flag ()) {
		_extent.min = shape_header.zbox.min;
		_extent.max = shape_header.zbox.max;
	}
	shape_file.First_Offset (sizeof (shape_header));
	shape_file.Record_Size (sizeof (Arc_Record));
	
	Num_Points (1);
	Num_Parts (1);

	//---- open the index file ----
	
	n = (int) strlen (pathname);
	memcpy (pathname + n - 3, "shx", 3);

	if (!index_file.Open (pathname)) {
		return (Status (NOT_OPEN));
	}

	if (File_Access () == CREATE) {
		index_header.file_size = sizeof (index_header) / sizeof (short);

		Reorder_Bits (&index_header, 7);
		
		if (!index_file.Write (&index_header, sizeof (index_header), 0L)) {
			return (Status (HEADER_WRITE));
		}

		//---- delete ArcGIS spatial index files ----

		memcpy (pathname + n - 3, "sbn", 3);
		remove (pathname);

		memcpy (pathname + n - 3, "sbx", 3);
		remove (pathname);

		//---- remove uDig support files ----

		memcpy (pathname + n - 3, "qix", 3);
		remove (pathname);

		memcpy (pathname + n - 3, "fix", 3);
		remove (pathname);

	} else {
		if (!index_file.Read (&index_header, sizeof (index_header), 0L)) {
			return (Status (HEADER_READ));
		}
	}
	Reorder_Bits (&index_header, 7);

	index_file.First_Offset (sizeof (index_header));
	index_file.Record_Size (sizeof (Arc_Record));

	//---- open the dbase file ----
	
	memcpy (pathname + n - 3, "dbf", 3);

	status = Db_Header::Open (pathname);

	Send_Messages (messages);
	index_file.Send_Messages (messages);
	
	return (status);
}

//-----------------------------------------------------------
//	Close
//-----------------------------------------------------------

bool Arcview_File::Close (void)
{
	//---- close the shape file ----

	if (shape_file.Is_Open ()) {
		if (shape_file.File_Access () != READ) {
			switch (Shape_Type ()) {
				case DOT:
					shape_header.shape_type = POINT_TYPE;
					break;
				case MDOTS:
					shape_header.shape_type = MULTIPOINT_TYPE;
					break;
				case LINE:
				case VECTOR:
					shape_header.shape_type = ARC_TYPE;
					break;
				case POLYGON:
					shape_header.shape_type = POLYGON_TYPE;
					break;
				default:
					break;
			}
			if (shape_header.box.xmin > shape_header.box.xmax) {
				shape_header.box.xmin = shape_header.box.xmax = 0;
			}
			if (shape_header.box.ymin > shape_header.box.ymax) {
				shape_header.box.ymin = shape_header.box.ymax = 0;
			}
			if (Z_Flag ()) {
				shape_header.shape_type += (M_Flag ()) ? 20 : 10;

				if (M_Flag () || shape_header.zbox.min > shape_header.zbox.max) {
					shape_header.zbox.min = shape_header.zbox.max = 0;
				}
				if (shape_header.mbox.min > shape_header.mbox.max) {
					shape_header.mbox.min = shape_header.mbox.max = 0;
				}
			}
			shape_header.file_size = shape_file.File_Size () / sizeof (short);

			Reorder_Bits (&shape_header, 7);

			if (!shape_file.Write (&shape_header, sizeof (shape_header), 0L)) {
				return (Status (HEADER_WRITE));
			}
			Reorder_Bits (&shape_header, 7);
		}
		shape_file.Close ();
	}

	//---- close the index file ----

	if (index_file.Is_Open ()) {
		if (index_file.File_Access () != READ) {
			index_header.shape_type = shape_header.shape_type;
			index_header.box.xmin = shape_header.box.xmin;
			index_header.box.ymin = shape_header.box.ymin;
			index_header.box.xmax = shape_header.box.xmax;
			index_header.box.ymax = shape_header.box.ymax;

			if (Z_Flag ()) {
				index_header.zbox.min = shape_header.zbox.min;
				index_header.zbox.max = shape_header.zbox.max;
				index_header.mbox.min = shape_header.mbox.min;
				index_header.mbox.max = shape_header.mbox.max;
			}
			index_header.file_size = index_file.File_Size () / sizeof (short);
			Reorder_Bits (&index_header, 7);

			if (!index_file.Write (&index_header, sizeof (index_header), 0L)) {
				return (Status (HEADER_WRITE));
			}
			Reorder_Bits (&index_header, 7);
		}
		index_file.Close ();
	}

	//---- close the dbase file -----

	return (Db_Header::Close ());
}

//----------------------------------------------------------
//	Z_Flag
//----------------------------------------------------------

void Arcview_File::Z_Flag (bool flag)
{
	if (z_flag != flag) {
		z_flag = flag;
		if (File_Access () == CREATE) M_Flag (z_flag);

		if (z_flag) {
			int number = Max_Points ();

			points.Z_Flag (z_flag);

			if (number > 0) {
				Max_Points (number);
			}
		}
	}
}

//----------------------------------------------------------
//	Read_Record
//----------------------------------------------------------

bool Arcview_File::Read_Record (int number)
{
	int i, npts, *parts, type;
	off_t offset;
	
	double *z;
	XY_Point  *pt;
	Arc_Record record;
	Arc_Shape  shape;
	Arc_Multi_Point  multi;

	if (!Check_File ()) return (false);
	if (!shape_file.Check_File ()) return (false);
	if (!index_file.Check_File ()) return (false);

	if (number <= 0) {
		number = Record_Number () + 1;
	}
	offset = index_file.First_Offset () + (number - 1) * sizeof (record);

	if (!index_file.Read (&record, sizeof (record), offset)) {
		return (false);
	}
	Reorder_Bits (&record, 2);

	offset = record.rec_num * sizeof (short);

	if (!shape_file.Read (&record, sizeof (record), offset)) {
		return (false); 
	}
	Reorder_Bits (&record, 2);

	//---- get the shape type ----

	if (!shape_file.Read (&type, sizeof (int))) {
		return (Status (RECORD_SYNTAX));
	}

	switch ((type % 10)) {
		case POINT_TYPE:		//---- point ----

			Num_Points (1);
			Shape_Type (DOT);
			
			pt = Get_XY_Data ();

			if (!shape_file.Read (pt, sizeof (XY_Point))) {
				return (Status (RECORD_SYNTAX));
			}
			Convert (pt);

			range.xmin = range.xmax = pt->x;
			range.ymin = range.ymax = pt->y;

			if (Z_Flag ()) {
				z = Get_Z_Data ();

				if (!shape_file.Read (z, sizeof (double))) {
					return (Status (RECORD_SYNTAX));
				}
				if (*z <= NAN) *z = 0.0;
				_range.min = _range.max = *z;
			} 
			break;
				
		case ARC_TYPE:			//---- arc ----
		case POLYGON_TYPE:		//---- polygon ----

			if (!shape_file.Read (&shape, sizeof (shape))) {
				return (Status (RECORD_SYNTAX));
			}
			if (shape.num_pts == 0 || shape.num_parts == 0) return (false);

			Num_Points (shape.num_pts);
			Num_Parts (shape.num_parts);

			range.xmin = shape_header.box.xmin;
			range.ymin = shape_header.box.ymin;
			range.xmax = shape_header.box.xmax;
			range.ymax = shape_header.box.ymax;

			Convert (&(range.xmin), &(range.ymin));
			Convert (&(range.xmax), &(range.ymax));

			//---- get the part offsets ----

			parts = Get_Parts ();

			if (!shape_file.Read (parts, shape.num_parts * sizeof (int))) {
				return (Status (RECORD_SYNTAX));
			}

			//---- get the point data ----

			pt = Get_XY_Data ();

			if (!shape_file.Read (pt, shape.num_pts * sizeof (XY_Point))) {
				return (Status (RECORD_SYNTAX));
			}
			npts = shape.num_pts;

			Shape_Type ((type == ARC_TYPE) ? ((npts == 2) ? LINE : VECTOR) : POLYGON);

			for (i=0; i < npts; i++, pt++) {
				Convert (pt);
			}
			if (Z_Flag ()) {
				if (!shape_file.Read (&_range, sizeof (_range))) {
					return (Status (RECORD_SYNTAX));
				}
				z = Get_Z_Data ();

				if (!shape_file.Read (z, shape.num_pts * sizeof (double))) {
					return (Status (RECORD_SYNTAX));
				}
				if (_range.min <= NAN) _range.min = 0;
				if (_range.max <= NAN) _range.max = 0;

				for (i=0; i < npts; i++, z++) {
					if (*z <= NAN) *z = 0;
				}
			}
			break;
				
		case MULTIPOINT_TYPE:		//---- multipoint ----

			if (!shape_file.Read (&multi, sizeof (multi))) {
				return (Status (RECORD_SYNTAX));
			}
			if (multi.num_pts == 0) return (false);

			Num_Points (multi.num_pts);
			
			range.xmin = multi.box.xmin;
			range.ymin = multi.box.ymin;
			range.xmax = multi.box.xmax;
			range.ymax = multi.box.ymax;

			Convert (&(range.xmin), &(range.ymin));
			Convert (&(range.xmax), &(range.ymax));

			pt = Get_XY_Data ();

			if (!shape_file.Read (pt, multi.num_pts * sizeof (XY_Point))) {
				return (Status (RECORD_SYNTAX));
			}
			npts = multi.num_pts;

			Shape_Type (MDOTS);
			
			for (i=0; i < npts; i++, pt++) {
				Convert (pt);
			}
			if (Z_Flag ()) {
				if (!shape_file.Read (&_range, sizeof (_range))) {
					return (Status (RECORD_SYNTAX));
				}
				z = Get_Z_Data ();

				if (!shape_file.Read (z, multi.num_pts * sizeof (double))) {
					return (Status (RECORD_SYNTAX));
				}
				if (_range.min <= NAN) _range.min = 0;
				if (_range.max <= NAN) _range.max = 0;

				for (i=0; i < npts; i++, z++) {
					if (*z <= NAN) *z = 0;
				}
			}
			break;

		default:
			Num_Points (0);
			Num_Parts (0);
			return (false);
			break;
	}
	return (Db_Header::Read_Record (number));
}

//----------------------------------------------------------
//	Write_Record
//----------------------------------------------------------

bool Arcview_File::Write_Record (int number)
{
	bool insert_flag, first;
	int i, type, size;
	off_t offset;

	double *z = 0;
	XY_Point  *pt;
	Arc_Record record;
	Arc_Shape  shape;
	Arc_Multi_Point  multi;

	if (!Check_File ()) return (false);
	if (!shape_file.Check_File ()) return (false);
	if (!index_file.Check_File ()) return (false);
	if (Num_Points () == 0) return (false);

	//---- determine the record type and size ----
	
	switch (Shape_Type ()) {
		
		case DOT:

			record.rec_size = (sizeof (int) + sizeof (XY_Point)) / sizeof (short);

			if (Z_Flag ()) {
				record.rec_size += ((M_Flag ()) ? 1 : 2) * sizeof (double) / sizeof (short);
			}
			break;

		case LINE:
		case VECTOR:
		case POLYGON:

			record.rec_size = (sizeof (int) + sizeof (shape) + 
				Num_Parts () * sizeof (int) + 
				Num_Points () * sizeof (XY_Point)) / sizeof (short);

			if (Z_Flag ()) {
				record.rec_size += ((M_Flag ()) ? 1 : 2) * 
					(sizeof (_range) + Num_Points () * sizeof (double)) / sizeof (short);
			}
			break;
				
		case MDOTS:

			record.rec_size = (sizeof (int) + sizeof (multi) + 
				Num_Points () * sizeof (XY_Point)) / sizeof (short);

			if (Z_Flag ()) {
				record.rec_size += ((M_Flag ()) ? 1 : 2) * 
					(sizeof (_range) + Num_Points () * sizeof (double)) / sizeof (short);
			}
			break;
			
		default:		//---- unsupported graphics type ----
			return (true);
	}

	//---- set the index record and shape header ----

	if (number > 0 && number != Record_Number () + 1) {
		offset = index_file.First_Offset () + (number - 1) * sizeof (record);

		if (!index_file.Read (&record, sizeof (record), offset)) {
			return (false);
		}
		Reorder_Bits (&record, 2);

		offset = record.rec_num * sizeof (short);
		record.rec_num = number;

		insert_flag = false;

		Reorder_Bits (&record, 1);

		if (!shape_file.Write (&record, sizeof (record), offset)) {
			return (false); 
		}
	} else {
		off_t index_offset = index_file.File_Size ();
		offset = shape_file.File_Size ();

		record.rec_num = offset / sizeof (short);
		
		Reorder_Bits (&record, 2);
	
		if (!index_file.Write (&record, sizeof (record), index_offset)) {
			return (false);
		}
		record.rec_num = (index_offset - index_file.First_Offset ()) / sizeof (record) + 1;
		insert_flag = true;

		Reorder_Bits (&record, 1);

		if (!shape_file.Write (&record, sizeof (record), offset)) {
			return (false); 
		}		
	}
				
	//---- store the shape record ----
	
	switch (Shape_Type ()) {
		
		case DOT:	//---- point ----

			type = POINT_TYPE;
			if (Z_Flag ()) {
				type += (M_Flag ()) ? 20 : 10;
			}
			shape_header.shape_type = type;
			
			pt = Get_XY_Data ();

			Convert (pt);
			
			if (pt->x < shape_header.box.xmin) {
				shape_header.box.xmin = pt->x;
			}
			if (pt->y < shape_header.box.ymin) {
				shape_header.box.ymin = pt->y;
			}
			if (pt->x > shape_header.box.xmax) {
				shape_header.box.xmax = pt->x;
			}
			if (pt->y > shape_header.box.ymax) {
				shape_header.box.ymax = pt->y;
			}

			if (Z_Flag ()) {
				z = Get_Z_Data ();

				if (*z > shape_header.mbox.max) {
					shape_header.mbox.max = *z;
				}
				if (*z < shape_header.mbox.min) {
					shape_header.mbox.min = *z;
				}
				if (!M_Flag ()) {
					if (*z > shape_header.zbox.max) {
						shape_header.zbox.max = *z;
					}
					if (*z < shape_header.zbox.min) {
						shape_header.zbox.min = *z;
					}
				}
			}
			if (insert_flag) {
				if (!shape_file.Write (&type, sizeof (int))) {
					return (false);
				}
				if (!shape_file.Write (pt, sizeof (XY_Point))) {
					return (false);
				}
				if (Z_Flag ()) {
					if (!shape_file.Write (z, sizeof (double))) {
						return (false);
					}
					if (!M_Flag ()) {
						if (!shape_file.Write (z, sizeof (double))) {
							return (false);
						}
					}
				}
			} else {
				offset += sizeof (record);
				if (!shape_file.Write (&type, sizeof (int), offset)) {
					return (false);
				}
				offset += sizeof (int);
				if (!shape_file.Write (pt, sizeof (XY_Point), offset)) {
					return (false);
				}
				if (Z_Flag ()) {
					offset += sizeof (double);
					if (!shape_file.Write (z, sizeof (double), offset)) {
						return (false);
					}
					if (!M_Flag ()) {
						offset += sizeof (double);
						if (!shape_file.Write (z, sizeof (double), offset)) {
							return (false);
						}
					}
				}
			}
			break;

		case LINE:
		case VECTOR:
		case POLYGON:

			if (Shape_Type ()== POLYGON) {
				type = POLYGON_TYPE;
			} else {
				type = ARC_TYPE;
			}
			if (Z_Flag ()) {
				type += (M_Flag ()) ? 20 : 10;
			}
			shape_header.shape_type = type;
				
			shape.box.xmin = 0;
			shape.box.ymin = 0;
			shape.box.xmax = 0;
			shape.box.ymax = 0;
			shape.num_parts = Num_Parts ();
			shape.num_pts = Num_Points ();
			first = true;

			pt = Get_XY_Data ();

			for (i=0; i < shape.num_pts; i++, pt++) {
				Convert (pt);
			
				if (first) {
					shape.box.xmin = pt->x;
					shape.box.ymin = pt->y;
					shape.box.xmax = pt->x;
					shape.box.ymax = pt->y;
					first = false;
				} else {
					if (pt->x < shape.box.xmin) {
						shape.box.xmin = pt->x;
					}
					if (pt->y < shape.box.ymin) {
						shape.box.ymin = pt->y;
					}
					if (pt->x > shape.box.xmax) {
						shape.box.xmax = pt->x;
					}
					if (pt->y > shape.box.ymax) {
						shape.box.ymax = pt->y;
					}
				}
			}
			if (shape.box.xmin < shape_header.box.xmin) {
				shape_header.box.xmin = shape.box.xmin;
			}
			if (shape.box.ymin < shape_header.box.ymin) {
				shape_header.box.ymin = shape.box.ymin;
			}
			if (shape.box.xmax > shape_header.box.xmax) {
				shape_header.box.xmax = shape.box.xmax;
			}
			if (shape.box.ymax > shape_header.box.ymax) {
				shape_header.box.ymax = shape.box.ymax;
			}

			if (Z_Flag ()) {
				_range.min = 0;
				_range.max = 0;
				first = true;

				z = Get_Z_Data ();

				for (i=0; i < Num_Points (); i++, z++) {
					if (first) {
						_range.min = _range.max = *z;
						first = false;
					} else {
						if (*z < _range.min) {
							_range.min = *z;
						}
						if (*z > _range.max) {
							_range.max = *z;
						}
					}
				}
				if (_range.min < shape_header.mbox.min) {
					shape_header.mbox.min = _range.min;
				}
				if (_range.max > shape_header.mbox.max) {
					shape_header.mbox.max = _range.max;
				}
				if (!M_Flag ()) {
					if (_range.min < shape_header.zbox.min) {
						shape_header.zbox.min = _range.min;
					}
					if (_range.max > shape_header.zbox.max) {
						shape_header.zbox.max = _range.max;
					}
				}
			}

			if (insert_flag) {
				if (!shape_file.Write (&type, sizeof (int))) {
					return (false);
				}
				if (!shape_file.Write (&shape, sizeof (shape))) {
					return (false);
				}
				if (!shape_file.Write (Get_Parts (), shape.num_parts * sizeof (int))) {
					return (false);
				}
				if (!shape_file.Write (Get_XY_Data (), shape.num_pts * sizeof (XY_Point))) {
					return (false);
				}
				if (Z_Flag ()) {
					if (!shape_file.Write (&_range, sizeof (_range))) {
						return (false);
					}
					if (!shape_file.Write (Get_Z_Data (), shape.num_pts * sizeof (double))) {
						return (false);
					}
					if (!M_Flag ()) {
						if (!shape_file.Write (&_range, sizeof (_range))) {
							return (false);
						}
						if (!shape_file.Write (Get_Z_Data (), shape.num_pts * sizeof (double))) {
							return (false);
						}
					}
				}
			} else {
				offset += sizeof (record);
				if (!shape_file.Write (&type, sizeof (int), offset)) {
					return (false);
				}
				offset += sizeof (int);
				if (!shape_file.Write (&shape, sizeof (shape), offset)) {
					return (false);
				}
				offset += sizeof (shape);
				size = shape.num_parts * sizeof (int);
				if (!shape_file.Write (Get_Parts (), size, offset)) {
					return (false);
				}
				offset += size;
				size = shape.num_pts * sizeof (XY_Point);
				if (!shape_file.Write (Get_XY_Data (), size, offset)) {
					return (false);
				}
				if (Z_Flag ()) {
					offset += size;
					size = sizeof (_range);
					if (!shape_file.Write (&_range, size, offset)) {
						return (false);
					}
					offset += size;
					size = shape.num_pts * sizeof (double);
					if (!shape_file.Write (Get_Z_Data (), size, offset)) {
						return (false);
					}
					if (!M_Flag ()) {
						offset += size;
						size = sizeof (_range);
						if (!shape_file.Write (&_range, size, offset)) {
							return (false);
						}
						offset += size;
						size = shape.num_pts * sizeof (double);
						if (!shape_file.Write (Get_Z_Data (), size, offset)) {
							return (false);
						}
					}
				}
			}
			break;
				
		case MDOTS:

			type = MULTIPOINT_TYPE;
			if (Z_Flag ()) {
				type += (M_Flag ()) ? 20 : 10;
			}
			shape_header.shape_type = type;

			multi.box.xmin = 0;
			multi.box.ymin = 0;
			multi.box.xmax = 0;
			multi.box.ymax = 0;
			multi.num_pts = Num_Points ();
			first = true;
			
			pt = Get_XY_Data ();
			
			for (i=0; i < multi.num_pts; i++, pt++) {
				Convert (pt);

				if (first) {
					multi.box.xmin = pt->x;
					multi.box.ymin = pt->y;
					multi.box.xmax = pt->x;
					multi.box.ymax = pt->y;
					first = false;
				} else {
					if (pt->x < multi.box.xmin) {
						multi.box.xmin = pt->x;
					}
					if (pt->y < multi.box.ymin) {
						multi.box.ymin = pt->y;
					}
					if (pt->x > multi.box.xmax) {
						multi.box.xmax = pt->x;
					}
					if (pt->y > multi.box.ymax) {
						multi.box.ymax = pt->y;
					}
				}
			}
			if (multi.box.xmin < shape_header.box.xmin) {
				shape_header.box.xmin = multi.box.xmin;
			}
			if (multi.box.ymin < shape_header.box.ymin) {
				shape_header.box.ymin = multi.box.ymin;
			}
			if (multi.box.xmax > shape_header.box.xmax) {
				shape_header.box.xmax = multi.box.xmax;
			}
			if (multi.box.ymax > shape_header.box.ymax) {
				shape_header.box.ymax = multi.box.ymax;
			}

			if (Z_Flag ()) {
				_range.min = 0;
				_range.max = 0;
				first = true;

				z = Get_Z_Data ();

				for (i=0; i < Num_Points (); i++, z++) {
					if (first) {
						_range.min = _range.max = *z;
						first = false;
					} else {
						if (*z < _range.min) {
							_range.min = *z;
						}
						if (*z > _range.max) {
							_range.max = *z;
						}
					}
				}
				if (_range.min < shape_header.mbox.min) {
					shape_header.mbox.min = _range.min;
				}
				if (_range.max > shape_header.mbox.max) {
					shape_header.mbox.max = _range.max;
				}
				if (!M_Flag ()) {
					if (_range.min < shape_header.zbox.min) {
						shape_header.zbox.min = _range.min;
					}
					if (_range.max > shape_header.zbox.max) {
						shape_header.zbox.max = _range.max;
					}
				}
			}

			if (insert_flag) {
				if (!shape_file.Write (&type, sizeof (int))) {
					return (false);
				}
				if (!shape_file.Write (&multi, sizeof (multi))) {
					return (false);
				}
				if (!shape_file.Write (Get_XY_Data (), multi.num_pts * sizeof (XY_Point))) {
					return (false);
				}
				if (Z_Flag ()) {
					if (!shape_file.Write (&_range, sizeof (_range))) {
						return (false);
					}
					if (!shape_file.Write (Get_Z_Data (), multi.num_pts * sizeof (double))) {
						return (false);
					}
					if (!M_Flag ()) {
						if (!shape_file.Write (&_range, sizeof (_range))) {
							return (false);
						}
						if (!shape_file.Write (Get_Z_Data (), multi.num_pts * sizeof (double))) {
							return (false);
						}
					}
				}
			} else {
				offset += sizeof (record);
				if (!shape_file.Write (&type, sizeof (int), offset)) {
					return (false);
				}
				offset += sizeof (int);
				if (!shape_file.Write (&multi, sizeof (multi), offset)) {
					return (false);
				}
				offset += sizeof (multi);
				size = multi.num_pts * sizeof (XY_Point);
				if (!shape_file.Write (Get_XY_Data (), size, offset)) {
					return (false);
				}
				if (Z_Flag ()) {
					offset += size;
					size = sizeof (_range);
					if (!shape_file.Write (&_range, size, offset)) {
						return (false);
					}
					offset += size;
					size = multi.num_pts * sizeof (double);
					if (!shape_file.Write (Get_Z_Data (), size, offset)) {
						return (false);
					}
					if (!M_Flag ()) {
						offset += size;
						size = sizeof (_range);
						if (!shape_file.Write (&_range, size, offset)) {
							return (false);
						}
						offset += size;
						size = multi.num_pts * sizeof (double);
						if (!shape_file.Write (Get_Z_Data (), size, offset)) {
							return (false);
						}
					}
				}
			}
			break;
			
		default:		//---- unsupported graphics type ----
			return (true);
	}
	return (Db_Header::Write_Record (number));
}

//----------------------------------------------------------
//	Reorder_Bits
//----------------------------------------------------------

void Arcview_File::Reorder_Bits (void *dat, int words)
{
	int i;
	char *data, ch;

	data = (char *) dat;

	for (i=0; i < words; i++, data+=4) {
		ch = *data;
		*data = *(data+3);
		*(data+3) = ch;

		ch = *(data+1);
		*(data+1) = *(data+2);
		*(data+2) = ch;
	}
}

//---------------------------------------------------------
//	Set_Projection
//---------------------------------------------------------

bool Arcview_File::Set_Projection (Projection_Data input, Projection_Data output)
{
	if (!Projection::Set_Projection (input, output)) return (false);

	if (File_Access () == CREATE) {
		Write_Projection ();
	}
	return (true);
}

//---------------------------------------------------------
//	Write_Projection
//---------------------------------------------------------

void Arcview_File::Write_Projection (void)
{
	if (shape_file.Is_Open ()) {
		Projection_Service service;

		char *projcs = service.Get_Projection_String (Output_Projection ());

		if (projcs [0] != '\0') {
			char pathname [FILE_NAME_SIZE];

			FILE *fp;
			int n;

			str_cpy (pathname, sizeof (pathname), shape_file.Filename ());

			n = (int) strlen (pathname);
			memcpy (pathname + n - 3, "prj", 3);

			fp = f_open (pathname, "wb");

			if (fp) {
				fputs (projcs, fp);
				fclose (fp);
			}
		}
	}
}
