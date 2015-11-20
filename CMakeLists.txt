cmake_minimum_required (VERSION 2.8.4)

project (Transims) # Currently version 4.x support only

set(PROJECT_GCC_FLAGS " -mtune=native -O3") # adjust flags as your soul desires
set(PROJECT_GCC_FLAGS "${PROJECT_GCC_FLAGS} -w") # comment this if you want to see tons of warnings (probably you are not)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PROJECT_GCC_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_GCC_FLAGS}")

foreach(target_with_data_dir IN ITEMS
		AdjustPlans ConvertTours ConvertTrips Emissions EventSum LineSum Microsimulator ModeChoice PopSyn Progression
		Router SmoothData SmoothPlans TransimsNet TransitNet)
	set(${target_with_data_dir}_extra_dirs "Data")
endforeach(target_with_data_dir)

include_directories(SysLib/Include)
find_package(Threads REQUIRED)
foreach(syslib_dir IN ITEMS "Data"
							"Dbfile"
							"Demand/Data"
							"Demand/Files"
							"Demand/Service"
							"Execution"
							"Network/Data"
							"Network/Files"
							"Network/Service"
							"Output"
							"Program"
							"Projection"
							"Utility")
	aux_source_directory(SysLib/${syslib_dir} SOURCES)
endforeach(syslib_dir)
add_library(SysLib STATIC ${SOURCES})
target_link_libraries(SysLib ${CMAKE_DL_LIBS})
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET SysLib PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET SysLib PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(SysLib "${CMAKE_THREAD_LIBS_INIT}")
endif()

foreach(target_name IN ITEMS ActGen ActBalance ActivityPattern AdjustPlans ArcAddZ ArcDelay ArcDiff ArcNet ArcPlan
	ArcProblem ArcRider ArcSnapshot ArcTraveler CheckSurvey ConvertTours ConvertTrips CoordMatch
	DiurnalMatrix DynusTNet DynusTPlan Emissions EnhanceNet EventSum ExportTransit FileFormat GISNet
	HHList IntControl LineSum LinkData LinkDelay LinkSum ListID LocationChoice LocationData Microsimulator ModeChoice NewFormat
	PlanCompare PlanPrep PlanSelect PlanSum PlanTrips PopSyn ProblemSelect ProblemSum Progression PUMSPrep Relocate Reschedule
	RiderPrep Router RunSetup SF3Dictionary SF3Prep SideFriction SmoothData SmoothPlans SubareaNet SubareaPlans TPPlusNet
	TPPlusRoute TransimsNet TransitNet TripPrep TripSum Validate VehGen VISSIMNet VISSIMPlan ZoneData)
	unset(SOURCES)
	aux_source_directory(${target_name} SOURCES)
	if (${target_name}_extra_dirs)
		foreach(dirname IN ITEMS ${${target_name}_extra_dirs})
			aux_source_directory(${target_name}/${dirname} SOURCES)
		endforeach(dirname)
	endif()
	add_executable(${target_name} ${SOURCES})
	if(THREADS_HAVE_PTHREAD_ARG)
	  set_property(TARGET ${target_name} PROPERTY COMPILE_OPTIONS "-pthread")
	  set_property(TARGET ${target_name} PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
	endif()
	if(CMAKE_THREAD_LIBS_INIT)
	  target_link_libraries(${target_name} "${CMAKE_THREAD_LIBS_INIT}")
	endif()
	target_link_libraries(${target_name} ${CMAKE_DL_LIBS})
	target_link_libraries(${target_name} SysLib)
endforeach(target_name)

