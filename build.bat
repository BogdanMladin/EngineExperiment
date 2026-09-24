if not exist "./build" (
	mkdir build)
)
cd build
cl -Z7  ../src/Win32_BoxArranger.cpp /link User32.lib Gdi32.lib Winmm.lib
cd ..

