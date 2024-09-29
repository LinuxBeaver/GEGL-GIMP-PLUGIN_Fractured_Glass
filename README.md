# GEGL Fractured Glass

A GIMP plugin to put your image behind fractured glass

![image](https://github.com/user-attachments/assets/fa3a24e8-f00d-47f7-853a-eb30f4f79854)

![image](https://github.com/user-attachments/assets/e1406454-3350-44c4-abb0-b3f4ebcf148e)



## OS specific location to put GEGL Filter binaries 

**Windows**
C:\Users\USERNAME\AppData\Local\gegl-0.4\plug-ins
 
**Linux**
 /home/(USERNAME)/.local/share/gegl-0.4/plug-ins
 
 **Linux (Flatpak)**
 /home/(USERNAME)/.var/app/org.gimp.GIMP/data/gegl-0.4/plug-ins

![image](https://github.com/LinuxBeaver/GEGL-glossy-balloon-text-styling/assets/78667207/f15fb5eb-c8d7-4c08-bbac-97048864e657)


## Compiling and Installing
**Linux**

All plugins compile using "build_plugin_linux.sh" when the user has **ninja**, **meson** and **gegl** but to do it manual follow these instructions

To compile and install you will need the GEGL header files (libgegl-dev on Debian based distributions or gegl on Arch Linux) and meson (meson on most distributions).

meson setup --buildtype=release build
ninja -C build


If you have an older version of gegl you may need to copy to ~/.local/share/gegl-0.3/plug-ins instead (on Ubuntu 18.04 for example).

**Windows**

All plugins compile using "build_plugin_windows.sh" when the user has **mysys2**, **ninja**, **meson** and **gegl** but to do it manual follow these instructions

The easiest way to compile this project on Windows is by using msys2. Download and install it from here: https://www.msys2.org/

Open a msys2 terminal with C:\msys64\mingw64.exe. Run the following to install required build dependencies:

pacman --noconfirm -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-meson mingw-w64-x86_64-gegl

Then build the same way you would on Linux:

meson setup --buildtype=release build
ninja -C build

