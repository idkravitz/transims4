//*********************************************************
//	Vehicle_Shape.hpp - vehicle polygon function
//*********************************************************

#ifndef VEHICLE_SHAPE_HPP
#define VEHICLE_SHAPE_HPP

#include "System_Defines.hpp"
#include "Point_Array.hpp"

bool Vehicle_Shape (XYZ_Point p1, XYZ_Point p2, double width, Point_Data *points);
bool Vehicle_Shape (XYZ_Point *p, int num_pts, double width, Point_Data *points);

#endif
