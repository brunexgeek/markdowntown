include_directories(
	"../../modules/markdowntown/include")

file(GLOB PARSER_SOURCES
	"source/*.cc")

add_executable(tool_markdowntown ${PARSER_SOURCES})
target_link_libraries(tool_markdowntown module_markdowntown)
set_target_properties(tool_markdowntown PROPERTIES
    OUTPUT_NAME "markdowntown"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    PREFIX "" )
