# Copyright (c) 2012, Vladislav Sitalo <root@stvad.org>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#     Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#     Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include(FindPackageHandleStandardArgs)
include(CMakePushCheckState)
include(CheckFunctionExists)
if(POCKETSPHINX_INCLUDE_DIR AND POCKETSPHINX_LIBRARIES)
set(Pocketsphinx_FIND_QUIETLY TRUE)
endif(POCKETSPHINX_INCLUDE_DIR AND POCKETSPHINX_LIBRARIES)

find_package(Sphinxbase REQUIRED)

FIND_PATH(POCKETSPHINX_INCLUDE_DIR pocketsphinx/pocketsphinx.h)
find_library(POCKETSPHINX_LIBRARIES pocketsphinx PATHS $ENV{LD_LIBRARY_PATH}/ /usr/lib)
find_package_handle_standard_args(Pocketsphinx REQUIRED_VARS POCKETSPHINX_LIBRARIES POCKETSPHINX_INCLUDE_DIR)

if(Pocketsphinx_FOUND OR POCKETSPHINX_FOUND)
  cmake_push_check_state()
  set(CMAKE_REQUIRED_INCLUDES ${POCKETSPHINX_INCLUDE_DIR})
  set(CMAKE_REQUIRED_LIBRARIES ${POCKETSPHINX_LIBRARIES})
  check_function_exists(ps_get_uttid POCKETSPHINX_HAS_UTTID_APIS)
  cmake_pop_check_state()
endif()
