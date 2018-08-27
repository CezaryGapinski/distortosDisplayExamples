#
# file: CMakeLists.txt
#
# author: Copyright (C) 2018 Cezary Gapinski cezary.gapinski@gmail.com
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
# distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

cmake_minimum_required(VERSION 3.7)
project(displayExamples)

include(distortos-utilities)

#-----------------------------------------------------------------------------------------------------------------------
# distortos library
#-----------------------------------------------------------------------------------------------------------------------

add_subdirectory(distortos)

#-----------------------------------------------------------------------------------------------------------------------
# displayExamples applications
#-----------------------------------------------------------------------------------------------------------------------

add_subdirectory(planesOperations)
