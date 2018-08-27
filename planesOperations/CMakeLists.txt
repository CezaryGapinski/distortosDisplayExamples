#
# file: CMakeLists.txt
#
# author: Copyright (C) 2018 Cezary Gapinski cezary.gapinski@gmail.com
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
# distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

#-----------------------------------------------------------------------------------------------------------------------
# planesOperations application
#-----------------------------------------------------------------------------------------------------------------------

add_executable(planesOperations
		planesOperations.cpp)
set_target_properties(planesOperations PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON)
target_link_libraries(planesOperations PRIVATE
		distortos::distortos
		distortos::allCxxFlags)
distortosTargetLinkerScripts(planesOperations $ENV{DISTORTOS_LINKER_SCRIPT})

distortosBin(planesOperations planesOperations.bin)
distortosDmp(planesOperations planesOperations.dmp)
distortosHex(planesOperations planesOperations.hex)
distortosLss(planesOperations planesOperations.lss)
distortosMap(planesOperations planesOperations.map)
distortosSize(planesOperations)
