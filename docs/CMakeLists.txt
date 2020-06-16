set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_EXCLUDE_PATTERNS "*/rapidjson/*")

doxygen_add_docs(docs
    "${PROJECT_SOURCE_DIR}/one/agent"
    "${PROJECT_SOURCE_DIR}/one/arcus"
    "${PROJECT_SOURCE_DIR}/one/server"
    "${PROJECT_SOURCE_DIR}/one/game"
)

add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/docs/Doxyfile.docs
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)
