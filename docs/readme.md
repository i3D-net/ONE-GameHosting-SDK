# Docs

The `CMakeList.txt` file located in this directory defines which folders are parsed by doxygen during the documentation creation.

Documentation is created each cmake build, provided that `doxygen` is installed.

Doxygen comments should only be added to source code that is part of the external API integrated into and used by game servers to communicate with One. All other source should be documented via regular comments and readme files.
