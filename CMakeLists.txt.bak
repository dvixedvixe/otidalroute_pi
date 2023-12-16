##---------------------------------------------------------------------------
## Author:      Dave Register
##***************************************************************************
## *   Copyright (C) 2010 by David S. Register                               *
## *   This program is free software; you can redistribute it and/or modify  *
## *   it under the terms of the GNU General Public License as published by  *
## *   the Free Software Foundation; either version 2 of the License, or     *
## *   (at your option) any later version.                                   *
## *                                                                         *
## *   This program is distributed in the hope that it will be useful,       *
## *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
## *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
## *   GNU General Public License for more details.                          *
## *                                                                         *
## *   You should have received a copy of the GNU General Public License     *
## *   along with this program; if not, write to the                         *
## *   Free Software Foundation, Inc.,                                       *
## *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
## ***************************************************************************

# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(otidalroute_pi)

SET(PACKAGE_NAME otidalroute_pi)
SET(VERBOSE_NAME otidalroute)
SET(TITLE_NAME otidalroute)
SET(CPACK_PACKAGE_CONTACT "Mike Rossiter")

SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "1")

#SET(CMAKE_BUILD_TYPE Debug)

INCLUDE("cmake/PluginConfigure.cmake")

SET(SRC_otidalroute
		src/AboutDialog.cpp
		src/AboutDialog.h
        src/bbox.cpp
        src/bbox.h
		src/GribRecord.cpp
		src/GribRecord.h
		src/GribRecordSet.h
        src/otidalroute_pi.h
        src/otidalroute_pi.cpp
        src/otidalrouteOverlayFactory.cpp
        src/otidalrouteOverlayFactory.h
        src/otidalrouteUIDialogBase.cpp
        src/otidalrouteUIDialogBase.h
        src/otidalrouteUIDialog.cpp
        src/otidalrouteUIDialog.h
        src/icons.h
        src/icons.cpp
        src/tcmgr.cpp       
        src/tcmgr.h
        src/tinystr.h
	    src/tinyxml.h
		src/tinyxml.cpp
	    src/tinyxmlerror.cpp
		src/tinyxmlparser.cpp		
		src/NavFunc.cpp
		src/NavFunc.h
		src/routeprop.cpp
		src/routeprop.h
		src/tableroutes.cpp
		src/tableroutes.h

    )

ADD_DEFINITIONS(-DTIXML_USE_STL)

ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_otidalroute})

INCLUDE("cmake/PluginJSON.cmake")

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
