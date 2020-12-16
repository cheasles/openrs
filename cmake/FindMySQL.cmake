# - Try to find MySQL.
# Once done this will define:
# MySQL_FOUND			- If false, do not try to use MySQL.
# MySQL_INCLUDE_DIRS	- Where to find mysql.h, etc.
# MySQL_LIBRARIES		- The libraries to link against.
# MySQL_VERSION_STRING	- Version in a string of MySQL.
#
# Created by RenatoUtsch based on eAthena implementation.
#
# Please note that this module only supports Windows and Linux officially, but
# should work on all UNIX-like operational systems too.
#

#=============================================================================
# Copyright 2012 RenatoUtsch
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if( WIN32 )
	find_path( MySQL_INCLUDE_DIR
		NAMES "mysql.h"
		PATHS "$ENV{PROGRAMFILES}/MySQL/*/include"
			  "$ENV{PROGRAMFILES(x86)}/MySQL/*/include"
			  "$ENV{SYSTEMDRIVE}/MySQL/*/include" )
	
	find_library( MySQL_LIBRARY
		NAMES "mysqlclient" "mysqlclient_r"
		PATHS "$ENV{PROGRAMFILES}/MySQL/*/lib"
			  "$ENV{PROGRAMFILES(x86)}/MySQL/*/lib"
			  "$ENV{SYSTEMDRIVE}/MySQL/*/lib" )
else()
	find_path( MySQL_INCLUDE_DIR
		NAMES "mysql.h"
		PATHS "/usr/include/mysql"
			  "/usr/local/include/mysql"
			  "/usr/mysql/include/mysql" )
	
	find_library( MySQL_LIBRARY
		NAMES "mysqlclient" "mysqlclient_r"
		PATHS "/lib/mysql"
			  "/lib64/mysql"
			  "/usr/lib/mysql"
			  "/usr/lib64/mysql"
			  "/usr/local/lib/mysql"
			  "/usr/local/lib64/mysql"
			  "/usr/mysql/lib/mysql"
			  "/usr/mysql/lib64/mysql" )
endif()



if( MySQL_INCLUDE_DIR AND EXISTS "${MySQL_INCLUDE_DIRS}/mysql_version.h" )
	file( STRINGS "${MySQL_INCLUDE_DIRS}/mysql_version.h"
		MySQL_VERSION_H REGEX "^#define[ \t]+MySQL_SERVER_VERSION[ \t]+\"[^\"]+\".*$" )
	string( REGEX REPLACE
		"^.*MySQL_SERVER_VERSION[ \t]+\"([^\"]+)\".*$" "\\1" MySQL_VERSION_STRING
		"${MySQL_VERSION_H}" )
endif()

# handle the QUIETLY and REQUIRED arguments and set MySQL_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( MySQL
	REQUIRED_VARS	MySQL_LIBRARY MySQL_INCLUDE_DIR
	VERSION_VAR		MySQL_VERSION_STRING )

set( MySQL_INCLUDE_DIRS ${MySQL_INCLUDE_DIR} )
set( MySQL_LIBRARIES ${MySQL_LIBRARY} )

mark_as_advanced( MySQL_INCLUDE_DIR MySQL_LIBRARY )

