set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_EXCLUDE_PATTERNS "*/internal/*")

doxygen_add_docs(docs
    "${PROJECT_SOURCE_DIR}/docs/mainpage.md"
    "${PROJECT_SOURCE_DIR}/readme.md"
    "${PROJECT_SOURCE_DIR}/one/arcus"
)

add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/docs/Doxyfile.docs
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)
