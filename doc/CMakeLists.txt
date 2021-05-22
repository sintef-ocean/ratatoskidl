find_package(SfhBuildScripts CONFIG QUIET)

if(SfhBuildsScripts_FOUND)
  include(SfhDocHelpers)
else()
  message(STATUS "SfhBuildScripts not found: Using bundled doc/SfhDocHelpers.cmake")
  include(SfhDocHelpers.cmake)
endif()

sfh_doc_python_requirements(${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt)

set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
set(DOXYGEN_GENERATE_XML "YES")
set(DOXYGEN_FULL_PATH_NAMES "YES")
set(DOXYGEN_USE_MATHJAX "YES")
set(DOXYGEN_ENABLE_PREPROCESSING "YES")
set(DOXYGEN_MACRO_EXPANSION "YES")
set(DOXYGEN_SKIP_FUNCTION_MACROS "NO")
set(DOXYGEN_EXPAND_ONLY_PREDEF "NO")
set(DOXYGEN_EXCLUDE_PATTERNS "*.cpp")
set(DOXYGEN_EXTRACT_ALL "YES")
set(DOXYGEN_RECURSIVE "YES")
set(DOXYGEN_SHOW_FILES "YES") # affects doxygen html only

# Support inline restructured text
list(APPEND DOXYGEN_ALIASES "rst=\\verbatim embed:rst:")
list(APPEND DOXYGEN_ALIASES "endrst=\\endverbatim")
list(APPEND DOXYGEN_ALIASES "rststar=\\verbatim embed:rst:leading-asterisk")
list(APPEND DOXYGEN_ALIASES "endrststar=\\endverbatim")

# Wrap code in #ifndef DOXYGEN_SHOULD_SKIP_THIS if doxygen should skip parsing
set(DOXYGEN_PREDEFINED DOXYGEN_SHOULD_SKIP_THIS)
list(APPEND DOXYGEN_PREDEFINED "sequence=sequence_") # IDL: avoid Sphinx compilation error

set(DOXYGEN_XML_OUTPUT "xml")               # Public api
set(DOXYGEN_STRIP_FROM_PATH ${CMAKE_SOURCE_DIR}) # Needed for exhale

set(DOXYGEN_GENERATE_HTML "NO")

doxygen_add_docs(
   doc-doxy
   "../src"
   COMMENT "Generating xml doc with Doxygen")


# Sphinx html generation =============================================

# Prepare sphinx root source directory
# Clean api build every time
# Copy static resources
# Copy markdown and rst files
# This assumes that files and directories to be copied do exist
macro(sfh_add_doc_sources_target _target _source_path _build_path)

   add_custom_target(${_target}
      COMMENT "DELETE generated docs and COPY doc sources to sphinx root directory"
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${_source_path}/api
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${_build_path}
      COMMAND ${CMAKE_COMMAND} -E copy_directory markup ${_source_path}/markup
      COMMAND ${CMAKE_COMMAND} -E copy_directory static ${_source_path}/static
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/README.rst ${_source_path}/markup
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/icons/logo.png ${_source_path}/static
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/icons/icon.ico ${_source_path}/static
      COMMAND ${CMAKE_COMMAND} -E copy_directory _templates ${_source_path}/_templates
      COMMAND ${CMAKE_COMMAND} -E copy_directory _ext ${_source_path}/_ext
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )

   set(_BREATHE_USER -Dbreathe_projects.${PROJECT_NAME}=${DOXYGEN_OUTPUT_DIRECTORY}/xml)
   set(_index_file ${_build_path}/index.html)

   # At least one breathe_project must be specified for exhale to invoke breathe
   add_custom_command(
      OUTPUT ${_index_file} always.outdated
      COMMAND ${SPHINX_EXECUTABLE} -b html
      ${_BREATHE_USER}
      ${_source_path} ${_build_path}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      MAIN_DEPENDENCY ${_source_path}/conf.py
      COMMENT "Generate docs with Sphinx"
      VERBATIM
      )

endmacro()

set(SPHINX_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

# Variables used in sphinx's conf.py
set(SPHINX_PROJECT_VENDOR "SINTEF Ocean")
set(SPHINX_PROJECT_AUTHOR "")
string(TIMESTAMP THE_YEAR %Y)


# User documentation =================================================

set(SPHINX_SOURCE ${SPHINX_BUILD_DIR}/source_user) # Location of user doc source
configure_file(conf.py.in ${SPHINX_SOURCE}/conf.py @ONLY)

# Uses: NOTHING at the moment
configure_file(index.rst.in ${SPHINX_SOURCE}/index.rst @ONLY)
sfh_add_doc_sources_target(doc-user-sources
   ${SPHINX_SOURCE}
   ${SPHINX_BUILD_DIR}/html)

add_custom_target(doc-user DEPENDS ${SPHINX_BUILD_DIR}/html/index.html)

add_custom_target(doc-user-archive
   COMMENT "Create an archive file for user doc"
   DEPENDS doc-user
   COMMAND ${CMAKE_COMMAND} -E tar "cfv"
   "${CMAKE_BINARY_DIR}/user-doc.zip" "--format=zip" "."
   WORKING_DIRECTORY ${SPHINX_BUILD_DIR}/html)

add_dependencies(doc-user doc-user-sources doc-doxy)


# Add doc target and optionally install user documentation ===========

add_custom_target(doc)
add_dependencies(doc doc-user)

# Currently, make install will not install doc unless doc-user(or doc) built first
install(
   DIRECTORY "${CMAKE_BINARY_DIR}/doc/sphinx/html"
   DESTINATION "${CMAKE_INSTALL_DOCDIR}"
   OPTIONAL
   PATTERN ".doctrees*" EXCLUDE
   PATTERN "_sources*" EXCLUDE
   PATTERN ".buildinfo" EXCLUDE
   )
install(FILES "../README.rst" DESTINATION "${CMAKE_INSTALL_DOCDIR}")
