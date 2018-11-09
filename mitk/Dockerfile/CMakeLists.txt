cmake_minimum_required(VERSION 3.10)
# Check if any dependencies need to be installed
#-----------------------------------------------------------------------------
# MITK
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
#set(MITK_DEPENDS)
#set(proj_DEPENDENCIES)
#set(proj MITK)


#-----------------------------------------------------------------------------
# Create CMake options to customize the MITK build
#-----------------------------------------------------------------------------

option(MITK_USE_SUPERBUILD "Use superbuild for MITK" ON)
option(MITK_USE_BLUEBERRY "Build the BlueBerry platform in MITK" ON)
option(MITK_BUILD_EXAMPLES "Build the MITK examples" OFF)
option(MITK_BUILD_ALL_PLUGINS "Build all MITK plugins" OFF)
option(MITK_BUILD_TESTING "Build the MITK unit tests" OFF)
option(MITK_USE_CTK "Use CTK in MITK" ${MITK_USE_BLUEBERRY})
option(MITK_USE_DCMTK "Use DCMTK in MITK" ON)
option(MITK_USE_Qt5 "Use Qt 5 library in MITK" ON)
option(MITK_USE_Qt5_WebEngine "Use Qt 5 WebEngine library" ON)
option(MITK_USE_OpenCV "Use Intel's OpenCV library" OFF)
option(MITK_USE_Python "Enable Python wrapping in MITK" OFF)

if (MITK_USE_BLUEBERRY AND NOT MITK_USE_CTK)
	message("Forcing MITK_USE_CTK to ON because of MITK_USE_BLUEBERRY")
	set(MITK_USE_CTK ON CACHE BOOL "Use CTK in MITK" FORCE)
endif ()

if (MITK_USE_CTK AND NOT MITK_USE_Qt5)
	message("Forcing MITK_USE_Qt5 to ON because of MITK_USE_CTK")
	set(MITK_USE_QT ON CACHE BOOL "Use Qt 5 library in MITK" FORCE)
endif ()

set(MITK_USE_CableSwig ${MITK_USE_Python})
set(MITK_USE_GDCM 1)
set(MITK_USE_ITK 1)
set(MITK_USE_VTK 1)

mark_as_advanced(MITK_USE_SUPERBUILD
		MITK_BUILD_ALL_PLUGINS
		MITK_BUILD_TESTING
		)

set(mitk_cmake_boolean_args
		MITK_USE_SUPERBUILD
		MITK_USE_BLUEBERRY
		MITK_BUILD_EXAMPLES
		MITK_BUILD_ALL_PLUGINS
		MITK_USE_CTK
		MITK_USE_DCMTK
		MITK_USE_Qt5
		MITK_USE_Qt5_WebEngine
		MITK_USE_OpenCV
		MITK_USE_Python
		)

if (MITK_USE_Qt5)
	message(STATUS "Checking Qt prerequisites")
	set(MITK_QT5_MINIMUM_VERSION 5.6.0)
	set(MITK_QT5_COMPONENTS Concurrent OpenGL PrintSupport Script Sql Svg Widgets Xml XmlPatterns UiTools Help LinguistTools)
	if (MITK_USE_Qt5_WebEngine)
		set(MITK_QT5_COMPONENTS ${MITK_QT5_COMPONENTS} WebEngineWidgets)
	endif ()
	if (APPLE)
		set(MITK_QT5_COMPONENTS ${MITK_QT5_COMPONENTS} DBus)
	endif ()

	find_package(Qt5 ${MITK_QT5_MINIMUM_VERSION} COMPONENTS ${MITK_QT5_COMPONENTS} REQUIRED)

	#	set(MITK_QT5_MAXIMUM_VERSION 10)
	#	message(STATUS "QT5 version ${Qt5_VERSION_MINOR} found.")
	#	if(Qt5_VERSION_MINOR GREATER MITK_QT5_MAXIMUM_VERSION)
	#		message(FATAL_ERROR "Some package won't compile with Qt5 > ${MITK_QT5_MAXIMUM_VERSION} for now.")
	#	endif()

	# I guess this is to pass tho Qt5 version to the subprojects which else definitely won't work
	if (Qt5_DIR)
		get_filename_component(_Qt5_DIR "${Qt5_DIR}/../../../" ABSOLUTE)
		list(FIND CMAKE_PREFIX_PATH "${_Qt5_DIR}" _result)
		if (_result LESS 0)
			set(CMAKE_PREFIX_PATH "${_Qt5_DIR};${CMAKE_PREFIX_PATH}" CACHE PATH "" FORCE)
		endif ()
	endif ()
elseif (MITK_USE_Qt5_WebEngine)
	set(MITK_USE_Qt5_WebEngine OFF)
endif ()

# Configure the set of default pixel types
if (NOT MITK_ACCESSBYITK_INTEGRAL_PIXEL_TYPES)
	set(MITK_ACCESSBYITK_INTEGRAL_PIXEL_TYPES
			"int, unsigned int, short, unsigned short, char, unsigned char"
			CACHE STRING "List of integral pixel types used in AccessByItk and InstantiateAccessFunction macros" FORCE)
endif ()

if (NOT MITK_ACCESSBYITK_FLOATING_PIXEL_TYPES)
	set(MITK_ACCESSBYITK_FLOATING_PIXEL_TYPES
			"double, float"
			CACHE STRING "List of floating pixel types used in AccessByItk and InstantiateAccessFunction macros" FORCE)
endif ()

if (NOT MITK_ACCESSBYITK_COMPOSITE_PIXEL_TYPES)
	set(MITK_ACCESSBYITK_COMPOSITE_PIXEL_TYPES
			"itk::RGBPixel<unsigned char>, itk::RGBAPixel<unsigned char>"
			CACHE STRING "List of composite pixel types used in AccessByItk and InstantiateAccessFunction macros" FORCE)
endif ()

if (NOT MITK_ACCESSBYITK_VECTOR_PIXEL_TYPES)
	string(REPLACE "," ";" _integral_types ${MITK_ACCESSBYITK_INTEGRAL_PIXEL_TYPES})
	string(REPLACE "," ";" _floating_types ${MITK_ACCESSBYITK_FLOATING_PIXEL_TYPES})
	foreach (_scalar_type ${_integral_types} ${_floating_types})
		set(MITK_ACCESSBYITK_VECTOR_PIXEL_TYPES
				"${MITK_ACCESSBYITK_VECTOR_PIXEL_TYPES}itk::VariableLengthVector<${_scalar_type}>,")
	endforeach ()
	string(LENGTH "${MITK_ACCESSBYITK_VECTOR_PIXEL_TYPES}" _length)
	math(EXPR _length "${_length} - 1")
	string(SUBSTRING "${MITK_ACCESSBYITK_VECTOR_PIXEL_TYPES}" 0 ${_length} MITK_ACCESSBYITK_VECTOR_PIXEL_TYPES)
	set(MITK_ACCESSBYITK_VECTOR_PIXEL_TYPES ${MITK_ACCESSBYITK_VECTOR_PIXEL_TYPES}
			CACHE STRING "List of vector pixel types used in AccessByItk and InstantiateAccessFunction macros for itk::VectorImage types" FORCE)
endif ()

if (NOT MITK_ACCESSBYITK_DIMENSIONS)
	set(MITK_ACCESSBYITK_DIMENSIONS
			"2,3"
			CACHE STRING "List of dimensions used in AccessByItk and InstantiateAccessFunction macros")
endif ()

#-----------------------------------------------------------------------------
# Create options to inject pre-build dependencies
#-----------------------------------------------------------------------------

foreach (proj CTK DCMTK GDCM VTK ITK OpenCV CableSwig)
	if (MITK_USE_${proj})
		set(MITK_${proj}_DIR "${${proj}_DIR}" CACHE PATH "Path to ${proj} build directory")
		mark_as_advanced(MITK_${proj}_DIR)
		if (MITK_${proj}_DIR)
			list(APPEND additional_mitk_cmakevars "-D${proj}_DIR:PATH=${MITK_${proj}_DIR}")
		endif ()
	endif ()
endforeach ()

set(MITK_BOOST_ROOT "${BOOST_ROOT}" CACHE PATH "Path to Boost directory")
mark_as_advanced(MITK_BOOST_ROOT)
if (MITK_BOOST_ROOT)
	list(APPEND additional_mitk_cmakevars "-DBOOST_ROOT:PATH=${MITK_BOOST_ROOT}")
endif ()

set(MITK_SOURCE_DIR "" CACHE PATH "MITK source code location. If empty, MITK will be cloned from MITK_GIT_REPOSITORY")
#set(MITK_GIT_REPOSITORY "https://github.com/PlusMinus0/MITK.git" CACHE STRING "The git repository for cloning MITK")
#set(MITK_GIT_REPOSITORY "https://github.com/MITK/MITK.git" CACHE STRING "The git repository for cloning MITK")
#set(MITK_GIT_TAG "master" CACHE STRING "The git tag/hash to be used when cloning from MITK_GIT_REPOSITORY")
#set(MITK_GIT_TAG "experiments/sDMAS-2018.07" CACHE STRING "The git tag/hash to be used when cloning from MITK_GIT_REPOSITORY")
set(MITK_GIT_REPOSITORY "https://phabricator.mitk.org/source/mitk.git" CACHE STRING "The git repository for cloning MITK")
set(MITK_GIT_TAG "5cca5d8fda62" CACHE STRING "The git tag/hash to be used when cloning from MITK_GIT_REPOSITORY")
mark_as_advanced(MITK_SOURCE_DIR MITK_GIT_REPOSITORY MITK_GIT_TAG)

#-----------------------------------------------------------------------------
# Create the final variable containing superbuild boolean args
#-----------------------------------------------------------------------------

set(mitk_boolean_args)
foreach (mitk_cmake_arg ${mitk_cmake_boolean_args})
	list(APPEND mitk_boolean_args -D${mitk_cmake_arg}:BOOL=${${mitk_cmake_arg}})
endforeach ()

#-----------------------------------------------------------------------------
# Additional MITK CMake variables
#-----------------------------------------------------------------------------

if (MITK_USE_Qt5 AND QT_QMAKE_EXECUTABLE)
	list(APPEND additional_mitk_cmakevars "-DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}")
endif ()

if (MITK_USE_CTK)
	list(APPEND additional_mitk_cmakevars "-DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}")
endif ()

if (MITK_INITIAL_CACHE_FILE)
	list(APPEND additional_mitk_cmakevars "-DMITK_INITIAL_CACHE_FILE:INTERNAL=${MITK_INITIAL_CACHE_FILE}")
endif ()


#-----------------------------------------------------------------------------
# Actually try to find MITK
#-----------------------------------------------------------------------------
message(STATUS "Checking for existing MITK")
if (NOT MITK_DIR)
	# Base directory for source and build
	set(MITK_BASE_DIR ${CMAKE_BINARY_DIR}/MITK/MITK)

	# Set Binary Directory for MITK for build
	set(MITK_BINARY_DIR ${MITK_BASE_DIR}/src/MITK-build)

	# Set MITK_DIR
	if (MITK_USE_SUPERBUILD)
		set(MITK_DIR_TMP "${MITK_BINARY_DIR}/MITK-build")
	else ()
		set(MITK_DIR_TMP "${MITK_BINARY_DIR}")
	endif ()

	set(MITK_DIR "${MITK_DIR_TMP}")
endif ()

find_package(MITK QUIET)
# I believe this is set to MITK_DIR-NOTFOUND after find_package
set(MITK_DIR "${MITK_DIR_TMP}")
message(STATUS "MITK_DIR: ${MITK_DIR}")

if (NOT MITK_FOUND)
	message(STATUS "MITK needs to be built")

	# Configure the MITK souce code location
	if (NOT MITK_SOURCE_DIR)
		set(mitk_source_location
				# SOURCE_DIR ${MITK_BASE_DIR}/MITK
				GIT_REPOSITORY ${MITK_GIT_REPOSITORY}
				GIT_TAG ${MITK_GIT_TAG}
				)
	else ()
		set(mitk_source_location
				SOURCE_DIR ${MITK_SOURCE_DIR}
				)
	endif ()


	find_package(Git REQUIRED)

	set(proj_DEPENDENCIES) # ??
	set(MITK_DEPENDS MITK) # ??

else ()
	message(STATUS "MITK found, performing sanity checks.")

	# The project is provided using MITK_DIR, nevertheless since other
	# projects may depend on MITK, let's add an 'empty' one
	include(ExternalProject)
	ExternalProject_Add(MITK
			DOWNLOAD_COMMAND ""
			CONFIGURE_COMMAND ""
			BUILD_COMMAND ""
			INSTALL_COMMAND ""
			DEPENDS
			${proj_DEPENDENCIES}
			)
	# Further, do some sanity checks in the case of a pre-built MITK
	set(my_itk_dir ${ITK_DIR})
	set(my_vtk_dir ${VTK_DIR})
	set(my_qmake_executable ${QT_QMAKE_EXECUTABLE})

	find_package(MITK REQUIRED)

	if (my_itk_dir AND ITK_DIR)
		if (NOT my_itk_dir STREQUAL ${ITK_DIR})
			message(FATAL_ERROR "ITK packages do not match:\n	  ${MY_PROJECT_NAME}: ${my_itk_dir}\n  MITK: ${ITK_DIR}")
		endif ()
	endif ()

	if (my_vtk_dir AND VTK_DIR)
		if (NOT my_vtk_dir STREQUAL ${VTK_DIR})
			message(FATAL_ERROR "VTK packages do not match:\n	  ${MY_PROJECT_NAME}: ${my_vtk_dir}\n  MITK: ${VTK_DIR}")
		endif ()
	endif ()

	if (my_qmake_executable AND MITK_QMAKE_EXECUTABLE)
		if (NOT
				my_qmake_executable STREQUAL ${MITK_QMAKE_EXECUTABLE})
			message(FATAL_ERROR "Qt qmake does not match:\n
		${MY_PROJECT_NAME}: ${my_qmake_executable}\n MITK:
		${MITK_QMAKE_EXECUTABLE}")
		endif ()
	endif ()

endif ()

include(ExternalProject)
ExternalProject_Add(MITK
	${mitk_source_location}
	#    BINARY_DIR ${MITK_BINARY_DIR}
	PREFIX MITK
#	PATCH_COMMAND git apply ${CMAKE_CURRENT_LIST_DIR}/external/MITK.patch
	INSTALL_COMMAND ""
	#    CMAKE_GENERATOR ${gen}
	CMAKE_ARGS
	  ${ep_common_args}
	  ${mitk_boolean_args}
	  ${additional_mitk_cmakevars}
	  -DQt5_DIR=${Qt5_DIR}  # Pass Qt5_DIR that was used to configure this build
	  -DBUILD_SHARED_LIBS:BOOL=ON
	  -DBUILD_TESTING:BOOL=${MITK_BUILD_TESTING}
	CMAKE_CACHE_ARGS ${ep_common_cache_args}
	CMAKE_CACHE_DEFAULT_ARGS ${ep_common_cache_default_args}
	DEPENDS ${proj_DEPENDENCIES}
	)
