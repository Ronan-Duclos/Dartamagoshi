# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Dartamagoshi_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Dartamagoshi_autogen.dir\\ParseCache.txt"
  "Dartamagoshi_autogen"
  )
endif()
