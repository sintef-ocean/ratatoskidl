# Assumes that OpenSplice_generate_isocpp2 is present (part of our OpenSplice dist)
# output_target_name: name of dds library object
# idl_target: name of ratatosk-idl imported target
#
function(ratatosk_add_idl_library output_target_name idl_target)

  set(idlBaseDir "${CMAKE_BINARY_DIR}/idlgen")
  set(idlGenDir "${idlBaseDir}/${idl_target}")

  get_property(target_idls
    TARGET "RatatoskIDL::${idl_target}"
    PROPERTY INTERFACE_SOURCES)
#    PROPERTY idl_sources)
# Uncomment this in the future when INTERFACE targets can export custom properties, CMake >3.15?
#  if(DEFINED _ratatosk_idl_dir)
#    list(TRANSFORM target_idls PREPEND "${_ratatosk_idl_dir}/")
#  endif()

  foreach(idl_file IN ITEMS ${target_idls})

    if(${ARGC} GREATER 2)
      string(GENEX_STRIP ${idl_file} idl_file)
      set(idl_file ${CMAKE_SOURCE_DIR}/src/${idl_file})
    endif()

    OpenSplice_generate_isocpp2(
      "${idl_file}"
      "${idlGenDir}"
      ddsSources)
    list(APPEND allDdsSources "${ddsSources}")
  endforeach()

  separate_arguments(allDdsSources)

  add_library("${output_target_name}" OBJECT
    ${allDdsSources})
  target_include_directories("${output_target_name}"
    PUBLIC
    $<BUILD_INTERFACE:${idlBaseDir}>
    $<INSTALL_INTERFACE:include>)
  target_link_libraries("${output_target_name}" PUBLIC OpenSplice::isocpp2)
  target_compile_features("${output_target_name}" PUBLIC "cxx_std_17")

  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options("${output_target_name}" PRIVATE "/w")
  endif()

endfunction()
