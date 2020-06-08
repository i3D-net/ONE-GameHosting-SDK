set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

doxygen_add_docs(docs
    "${PROJECT_SOURCE_DIR}/one/agent"
    "${PROJECT_SOURCE_DIR}/one/host"
    "${PROJECT_SOURCE_DIR}/one/game"
)
