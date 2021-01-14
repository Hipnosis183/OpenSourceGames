# WarMUX

**WarMUX** (known as **Wormux** until November 21st, 2010) is a multi-platform Worms-like strategy game.

## Contents

- WarMUX (11.04.1)
- Bonus Maps (20100501)
- Community Maps (20110313)
- Community Skins (20110313)

## Changes

- Fixed build errors
- Game data and configuration directories are now relative
- Included missing locale data and all the extra content released

## Build & Usage

The source has been fixed in order to compile in modern distros like Ubuntu-based Focal (20.04) releases:

```
// Install dependencies

sudo apt-get install build-essential g++ gettext libxml2-dev libsdl-ttf2.0-dev libsdl-mixer1.2-dev libsdl-image1.2-dev libsdl-gfx1.2-dev libsdl-net1.2-dev libxml++2.6-dev

// Build project

cd warmux
./configure
make
```

After compiling, navigate to the '**src**' folder and copy the file '**warmux**' to the desired directory. Also you'll need to copy the '**data**' folder located in the root of '**warmux**' over the same directory in which the binary is located.

Copy the '**data**', '**local**' and '**locale**' folders located in '**warmux-data**' over the same directory in which the binary is located; overwrite when asked. Finally, run with:

```
./warmux
```