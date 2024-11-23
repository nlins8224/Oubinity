#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "FastNoise2::FastNoise" for configuration ""
set_property(TARGET FastNoise2::FastNoise APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(FastNoise2::FastNoise PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libFastNoise.a"
  )

list(APPEND _cmake_import_check_targets FastNoise2::FastNoise )
list(APPEND _cmake_import_check_files_for_FastNoise2::FastNoise "${_IMPORT_PREFIX}/lib/libFastNoise.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
