# OpenJazz

**Jazz Jackrabbit** is a PC platform game. Produced by Epic Games (then Epic MegaGames), it was first released in 1994.

**OpenJazz** is a free, open-source version of the classic Jazz Jackrabbit games. OpenJazz can be compiled on a wide range of operating systems, including Windows, GNU/Linux and macOS. To play, you will need the files from one of the original games.

## Contents

- OpenJazz (20190106)
- ModPlug (0.8.9.0)

## Changes

- Replaced the Scale2X filtering with Nearest-Neighbor scaling
- Included the ModPlug source files and modified the Makefile for music support

## Build & Usage

Building with music support is now enabled by default. For the scaling I choosed to modify the Scale2X algorithm instead of resizing with SDL. To build and run the program the following packages are needed:

```
// Install dependencies

sudo apt-get install build-essential pkg-config automake libsdl1.2-dev libsdl1.2debian libsdl-mixer1.2 libsdl-mixer1.2-dev libsdl-image1.2 libmodplug-dev

// Build project

cd openjazz
./configure
make
```

After compiling, copy the files '**OpenJazz**' and '**openjazz.000**' in the project's root folder to the desired directory.

Copy the folder with the original game data (not included) over the same directory in which the binary is located. Finally, run with:

```
./OpenJazz *data folder*
```