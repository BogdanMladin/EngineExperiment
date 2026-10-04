if not exist "./build" (
	mkdir build)
)
cd build
cl -Z7  ../src/Win32_Paladin.cpp /link User32.lib Gdi32.lib Winmm.lib
cd ..

