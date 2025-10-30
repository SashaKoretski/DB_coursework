# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\kr_kg_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\kr_kg_autogen.dir\\ParseCache.txt"
  "kr_kg_autogen"
  )
endif()
