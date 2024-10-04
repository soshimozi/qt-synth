# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\synthesizer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\synthesizer_autogen.dir\\ParseCache.txt"
  "synthesizer_autogen"
  )
endif()
