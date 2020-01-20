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

include(ExternalProject)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(LibDNF REQUIRED)

add_library (eoltracker SHARED 
    "src/OperatingSystem.cpp"
    "src/Package.cpp"
    "src/Project.cpp"
    "src/RepoScan.cpp"
    "src/Version.cpp"
)

set(EOLTRACKER_COMPILE_OPTIONS 
    -ggdb
    -fnon-call-exceptions
    -pedantic
    -Werror
    -Wall
    -Wextra
    -Wcast-align
    -Wwrite-strings
    -Wfloat-equal
    -Woverloaded-virtual
    -Wno-aggregate-return
    -funswitch-loops
    -Wno-variadic-macros
    -Wno-literal-suffix
    -Wno-missing-field-initializers
)

target_compile_options(eoltracker PRIVATE ${EOLTRACKER_COMPILE_OPTIONS})
set_target_properties(eoltracker PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(eoltracker PUBLIC include)
target_link_libraries(eoltracker PUBLIC
    libdnf::libdnf
    Cutelyst::Core
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Cutelyst::Utils::Sql)
