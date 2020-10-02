set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_EXCLUDE_PATTERNS "*/internal/*")
set(DOXYGEN_HTML_EXTRA_STYLESHEET "${PROJECT_SOURCE_DIR}/docs/extra_style.css")
set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)
set(DOXYGEN_PROJECT_NAME "i3D.net Game Hosting SDK")

doxygen_add_docs(docs
    "${PROJECT_SOURCE_DIR}/docs/mainpage.md"
    "${PROJECT_SOURCE_DIR}/readme.md"
    "${PROJECT_SOURCE_DIR}/one/arcus/c_api.h"
    "${PROJECT_SOURCE_DIR}/one/arcus/c_error.h"
)

add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/docs/Doxyfile.docs
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)
