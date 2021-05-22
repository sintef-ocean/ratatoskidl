cmake_minimum_required(VERSION 3.10)

set(THE_PROJECT_MAINTAINER "SINTEF Ocean <joakim.haugen@sintef.no>")
set(THE_PROJECT_VENDOR "SINTEF Ocean")
set(CPACK_PACKAGE_NAME ratatosk-idl)

option(WITH_TESTS "Try to compile idl files (requires OpenSplice)" OFF)
option(WITH_DOC   "Enable building documentation" OFF)

if(WITH_TESTS)
  set(WITH_LANG CXX)
else()
  set(WITH_LANG NONE)
endif()

file(STRINGS "${CMAKE_SOURCE_DIR}/version.txt" projectVersion)

project(RatatoskIDL
  VERSION ${projectVersion}
  LANGUAGES ${WITH_LANG}
  DESCRIPTION "Interface Definition Language files for Ratatosk")


list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(EXISTS ${CMAKE_BINARY_DIR}/conan_paths.cmake)
  set(WITH_CONAN ON)
  include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()


if(WITH_TESTS)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  find_package(OpenSplice 6.9 REQUIRED)
endif()

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}")
set(INSTALL_IDLDIR "${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}")


# --------------------------------------------------------------------
# Documentation target

if(WITH_DOC)
  add_subdirectory(doc)
endif()

# --------------------------------------------------------------------
# Adds interface target RatatoskIDL::${lib_name} from ${idl_files}
#   - It assumes ${idl_files} are located in src/${lib_name}/
#   - idl files are available with get_property on INTERFACE_SOURCES

function(ratatosk_idl_lib lib_name idl_files)
  add_library(${lib_name} INTERFACE)

  separate_arguments(idl_files)

  foreach(idl_file IN ITEMS ${idl_files})
    #Uncomment in the future
    #list(APPEND "install_${lib_name}_files" "${lib_name}/${idl_file}")
    list(APPEND "install_${lib_name}_files" "$<INSTALL_INTERFACE:${INSTALL_IDLDIR}/>${lib_name}/${idl_file}")
    list(APPEND "source_${lib_name}_files" "src/${lib_name}/${idl_file}")
  endforeach()

  # Cannot export custom properties for INTERFACE targets until perhaps CMake 3.15
  # https://gitlab.kitware.com/cmake/cmake/issues/19261
  #set_property(TARGET ${lib_name} PROPERTY idl_sources "${install_${lib_name}_files}")
  #set_property(TARGET ${lib_name} PROPERTY EXPORT_PROPERTIES "idl_sources")
  target_sources(${lib_name} INTERFACE "${install_${lib_name}_files}")

  install(TARGETS ${lib_name} EXPORT ${PROJECT_NAME})
  install(FILES ${source_${lib_name}_files}
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/${lib_name}"
    )

  add_library(${PROJECT_NAME}::${lib_name} ALIAS ${lib_name})

  set(IDL_LIB_INFO "IDL Library RatatoskIDL::${lib_name}:  ${idl_files}")

  if(CMAKE_VERSION VERSION_LESS 3.15)
    message(STATUS "${IDL_LIB_INFO}")
  else()
    message(VERBOSE "${IDL_LIB_INFO}")
  endif()

  endfunction()

# --------------------------------------------------------------------
# IDL library pingpong with multiple IDL files

ratatosk_idl_lib(pingpong "ping.idl pong.idl")

# --------------------------------------------------------------------
# FKIN data types

ratatosk_idl_lib(FKIN "fkin_types.idl")

# --------------------------------------------------------------------
# Ratatosk basic types

ratatosk_idl_lib(Ratatosk "basic_types.idl")

# --------------------------------------------------------------------
# Netatmo weather station selected types

ratatosk_idl_lib(Netatmo "weather_types.idl")


# --------------------------------------------------------------------
# Create find scripts and install import libraries

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY ExactVersion)

configure_file(cmake/projectConfig.cmake.in
  cmake/${PROJECT_NAME}Config.cmake.in @ONLY)

configure_package_config_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
  PATH_VARS INSTALL_CONFIGDIR INSTALL_IDLDIR
  INSTALL_DESTINATION "${INSTALL_CONFIGDIR}"
  )

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
  "cmake/RatatoskIDLHelpers.cmake"
  DESTINATION "${INSTALL_CONFIGDIR}"
  )

install(FILES LICENSE
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

install(EXPORT "${PROJECT_NAME}"
  FILE "${PROJECT_NAME}Targets.cmake"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "${INSTALL_CONFIGDIR}"
  )

# --------------------------------------------------------------------
# Packaging

set(THE_PACKAGE_LINUX_INSTALL_PREFIX /usr/local)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "${THE_PROJECT_VENDOR}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.rst")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_VERSION
  ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

set(CPACK_PACKAGE_FILE_NAME
  ${CPACK_PACKAGE_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

if(WIN32 AND NOT UNIX)
  set(CPACK_GENERATOR "TGZ")

else(WIN32 AND NOT UNIX)

  set(CPACK_GENERATOR "DEB;TGZ")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE all)
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${THE_PROJECT_MAINTAINER}")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY optional)
  set(CPACK_PACKAGING_INSTALL_PREFIX "${THE_PACKAGE_LINUX_INSTALL_PREFIX}")

endif(WIN32 AND NOT UNIX)

# Needed by Kluster CICD
set(THE_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME})

include(CPack)

# --------------------------------------------------------------------
# Testing

if(WITH_TESTS)
  include(RatatoskIDLHelpers)
  get_directory_property(targets BUILDSYSTEM_TARGETS)

  if(OpenSplice_FOUND)
    foreach(target IN ITEMS ${targets})
      ratatosk_add_idl_library("test-${target}" ${target} IN_HOUSE)
    endforeach()
  else()
    message(FATAL_ERROR "OpenSplice was not found, cannot perform tests")
  endif()

endif()


# --------------------------------------------------------------------
# Temporary: CICD internally on Kluster

find_package(kluster-scripts CONFIG QUIET)

if(kluster-scripts_FOUND)
  if(WITH_DOC)
    set(RATATOSKIDL_DOC doc)
  endif()
  message(STATUS "Adding aptly deploy target")
  kluster_add_aptly_targets(${RATATOSKIDL_DOC})
endif()
