# Docs

The `CMakeList.txt` file located in this directory contains the following command:

```cmake
doxygen_add_docs(docs
    "${PROJECT_SOURCE_DIR}/one/agent"
    "${PROJECT_SOURCE_DIR}/one/arcus"
    "${PROJECT_SOURCE_DIR}/one/host"
    "${PROJECT_SOURCE_DIR}/one/game"
    ...
)
```

Which defines which folder are parsed by doxygen during the documentation creation.

Currently, we create the documentation at each build, provided that `doxygen` has been installed on you host.
