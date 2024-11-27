set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_EXCLUDE_PATTERNS "*/internal/*")
set(DOXYGEN_PROJECT_LOGO "${PROJECT_SOURCE_DIR}/doxygen/resources/i3d_logo_64.png")
set(DOXYGEN_HTML_EXTRA_STYLESHEET "${PROJECT_SOURCE_DIR}/doxygen/extra_style.css")
set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)
set(DOXYGEN_PROJECT_NAME "Game Hosting SDK")
set(DOXYGEN_SHOW_FILES YES)

set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs")

doxygen_add_docs(docs
    "${PROJECT_SOURCE_DIR}/one/arcus/c_api.h"
    "${PROJECT_SOURCE_DIR}/one/arcus/c_error.h"
    "${PROJECT_SOURCE_DIR}/one/fake/arcus/game/one_server_wrapper.cpp"
    "${PROJECT_SOURCE_DIR}/one/fake/arcus/game/one_server_wrapper.h"
    "${PROJECT_SOURCE_DIR}/one/ping/c_api.h"
    "${PROJECT_SOURCE_DIR}/one/ping/c_error.h"

    "${PROJECT_SOURCE_DIR}/one/fake/arcus/agent/readme.md"
    "${PROJECT_SOURCE_DIR}/one/fake/arcus/game/readme.md"
    "${PROJECT_SOURCE_DIR}/one/fake/readme.md"
    "${PROJECT_SOURCE_DIR}/one/fake/ping/readme.md"
    "${PROJECT_SOURCE_DIR}/one/ping/readme.md"
    "${PROJECT_SOURCE_DIR}/one/arcus/readme.md"
    "${PROJECT_SOURCE_DIR}/tests/readme.md"
    "${PROJECT_SOURCE_DIR}/tools/windows/readme.md"
    "${PROJECT_SOURCE_DIR}/tools/linux/readme.md"
    "${PROJECT_SOURCE_DIR}/tools/all/readme.md"
    "${PROJECT_SOURCE_DIR}/tools/readme.md"
    "${PROJECT_SOURCE_DIR}/vscode.md"
    "${PROJECT_SOURCE_DIR}/build-windows.md"
    "${PROJECT_SOURCE_DIR}/build-linux.md"
    "${PROJECT_SOURCE_DIR}/readme.md"
)

add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doxygen/Doxyfile.docs
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)

configure_file("${PROJECT_SOURCE_DIR}/doxygen/resources/i3d_logo_64.png" "${PROJECT_SOURCE_DIR}/build/docs/html/i3d_logo_64.png" COPYONLY)
