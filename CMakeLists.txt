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

project(eoltrackerweb CXX)

set(CMAKE_CXX_STANDARD 17)

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
if (POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(Cutelyst3Qt5 REQUIRED)
find_package(cpptoml REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED systemd)
pkg_get_variable(SYSTEMDUNITDIR systemd systemdsystemunitdir)

set(EOLTRACKER_WEBROOT "${CMAKE_CURRENT_SOURCE_DIR}/root" CACHE STRING "Where to expect the web templates and static assets")
set(EOLTRACKER_CONFDIR "${CMAKE_CURRENT_SOURCE_DIR}/dist" CACHE STRING "Where to expect the config file")
add_compile_definitions(WEBROOT="${EOLTRACKER_WEBROOT}")
add_compile_definitions(CONFDIR="${EOLTRACKER_CONFDIR}")

# Auto generate moc files
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Cutelyst2Qt5_INCLUDE_DIR}
)

# webpack js
add_custom_target(yarn ALL "yarn"
  DEPENDS "package.json"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  VERBATIM USES_TERMINAL
)
add_custom_target(webpack ALL "npx" "webpack" "--config" "webpack.config.js"
  BYPRODUCTS "${EOLTRACKER_WEBROOT}/static/js/main.js"
  DEPENDS "webpack.config.js" "js/timeline.js"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  VERBATIM USES_TERMINAL
)
add_dependencies(webpack yarn)
add_custom_target(js ALL)
add_dependencies(js webpack)

# actual web project
add_subdirectory(src)

# add backend project for eoltracker
add_subdirectory(lib)

# install webroot stuff to real webroot
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/root/static ${CMAKE_CURRENT_SOURCE_DIR}/root/templates DESTINATION ${EOLTRACKER_WEBROOT})
# install config file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/eol-tracker.conf DESTINATION ${EOLTRACKER_CONFDIR})
# install systemd service
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/eol-tracker.service DESTINATION ${SYSTEMDUNITDIR})
