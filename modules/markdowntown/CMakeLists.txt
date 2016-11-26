include_directories(
	"include")

add_custom_target(parser_bison COMMAND
	bison --defines=${CMAKE_CURRENT_SOURCE_DIR}/source/markdowntown.y.hh
	    -o ${CMAKE_CURRENT_SOURCE_DIR}/source/markdowntown.y.cc
	    -vt ${CMAKE_CURRENT_SOURCE_DIR}/source/markdowntown.y)

add_custom_target(parser_flex COMMAND flex
		-v
		-o ${CMAKE_CURRENT_SOURCE_DIR}/source/markdowntown.l.cc
		--header-file=${CMAKE_CURRENT_SOURCE_DIR}/source/markdowntown.l.hh
		${CMAKE_CURRENT_SOURCE_DIR}/source/markdowntown.l)


file(GLOB PARSER_SOURCES
	"source/*.cc")

add_executable(module_markdowntown ${PARSER_SOURCES})
set_target_properties(module_markdowntown PROPERTIES
    OUTPUT_NAME "markdowntown"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    PREFIX "lib" )
