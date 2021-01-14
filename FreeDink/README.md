# FreeDink

**Dink Smallwood** is an adventure/role-playing game made by RTsoft. The game features the twisted adventure of young Dink Smallwood, a pig farmer who leaves his town and goes on a quest to save the world.

**GNU FreeDink** is a new and portable version of the game engine, which runs the original game as well as its D-Mods, with close compatibility, under multiple platforms.

## Contents

- FreeDink (109.6)
- FreeDink-Data (1.08.20190120)
- DFArc (3.14)

## Changes

- FreeDink has been modified to load the game data from the same folder as the binary
- FreeDink-Data now includes the required font file
- DFArc and D-Mods paths remain unchanged

## Build & Usage

The original project is currently unmantained, and the dependencies to build are deprecated in newer distros. That said, the project can be built in Ubuntu-based distros running Xenial (16.04):

```
// Install dependencies

sudo apt-get install gnulib autoconf automake pkg-config help2man gettext autopoint rsync build-essential pkg-config libsdl2-dev libsdl2-image-dev libsdl2-image-dbg libsdl2-mixer-dev libsdl2-mixer-dbg libsdl2-ttf-dev libsdl2-ttf-dbg libsdl2-gfx-dev libsdl2-gfx-dbg libfontconfig1-dev libglm-dev cxxtest

// Build project

cd freedink
sh bootstrap
./configure
make

```

Still, in modern distros like Focal (20.04), the available dependencies are needed to run the game:

```
sudo apt-get install gnulib autoconf automake pkg-config help2man gettext autopoint rsync build-essential pkg-config libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev libfontconfig1-dev libglm-dev cxxtest
```

After compiling, navigate to the '**src**' folder and copy '**freedink**' and '**freedinkedit**' to the desired directory.

Copy the '**dink**' folder located in '**freedink-data**' over the same directory in which the binaries are located. Finally, run with:

```
./freedink
```

Haven't tested DFArc and loading of D-Mods, but those should work as usual, using the original paths.