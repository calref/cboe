A basic cmake configuration which can be used to compile the executables on Linux and on OsX. Unfortunately, the creation of package is not implemented.

On OsX, this requires that cmake, boost and SFML are installed (and that findBoost and findSFML are present).
On Linux, you will need to install the same packages and you will also need a valid installation of OpenGL, tgui, X11.

To compile the project, you can launch the following commands:
make build
cd build
cmake ..
make