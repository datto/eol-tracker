#[[
This file is part of eol-tracker.

Copyright © 2020 Datto, Inc.
Author: Dakota Williams <drwilliams@datto.com>

Licensed under the GNU Lesser General Public License Version 3
Fedora-License-Identifier: LGPLv3+
SPDX-2.0-License-Identifier: LGPL-3.0+
SPDX-3.0-License-Identifier: LGPL-3.0-or-later

eol-tracker is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

eol-tracker is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with eol-tracker.  If not, see <https://www.gnu.org/licenses/>.
]]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_DNF QUIET libdnf)

find_path(libdnf_INCLUDE_DIRS
    NAMES libdnf/libdnf.h
    HINTS ${PC_DNF_INCLUDEDIR} ${PC_DNF_INCLUDE_DIRS})
set(libdnf_INCLUDE_DIRS ${libdnf_INCLUDE_DIRS} ${PC_DNF_INCLUDE_DIRS})

find_library(libdnf_LIBRARIES
    NAMES dnf
    HINTS ${PC_libdnf_LIBDIR} ${PC_libdnf_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libdnf
    REQUIRED_VARS libdnf_LIBRARIES libdnf_INCLUDE_DIRS)

if(libdnf_FOUND AND NOT TARGET libdnf::libdnf)
    add_library(libdnf::libdnf UNKNOWN IMPORTED)
    set_target_properties(libdnf::libdnf PROPERTIES
        IMPORTED_LOCATION ${libdnf_LIBRARIES}
        INTERFACE_INCLUDE_DIRECTORIES "${libdnf_INCLUDE_DIRS}")
endif()
