##
#   Copyright (C) 2004 - 2016 Université de Versailles Saint-Quentin-en-Yvelines (UVSQ)
#
#   This file is part of MAQAO.
#
#  MAQAO is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public License
#   as published by the Free Software Foundation; either version 3
#   of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
##

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -std=gnu99 -fgnu89-inline -g3")

FILE(GLOB 
   files_ole
   ole/lru/lru4.c 
   ole/lru/lru8.c 
   ole/lru/lru12.c 
   ole/lru/lru16.c 
   ole/lru/lru20.c 
   ole/lru/lru24.c 
   ole/lru/lru32.c
   ole/plru/plru4.c 
   ole/plru/plru8.c 
   ole/plru/plru12.c 
   ole/plru/plru16.c
   ole/plru/plru20.c 
   ole/plru/plru24.c 
   ole/plru/plru32.c
   ole/inst.c
   ole/dump.c
   ole/bitwz.c
   ole/init.c
   ole/conf.c
   ole/ole.c
)  

### --- Create the dynamic OLE library 
ADD_LIBRARY(ole-dyn	      SHARED ${files_ole})
SET_TARGET_PROPERTIES(ole-dyn PROPERTIES PREFIX ""  OUTPUT_NAME ole)
SET_TARGET_PROPERTIES(ole-dyn PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(ole-dyn pthread m ${LUA_LIB_DYN})

INCLUDE_DIRECTORIES(ole)
