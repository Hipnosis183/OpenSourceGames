# Egoboo

**Egoboo** is an open-source, cross-platform computer game. It's a three-dimensional dungeon crawling adventure that borrows a lot of elements from the roguelike genre. It is an action-based adventure game with a unique mood and style. In Egoboo, you will slay hordes of foul monsters, explore many unusual locations, and get lots of really cool loot. Your ultimate quest is to save Lord Bishop, the king of the land, from the evil Dracolich. It is a difficult task, and you will have to face unbelievable danger to accomplish your goal.

## Contents

- Egoboo (2.8.1)

## Changes

- Fixed build errors
- Game data and configuration directories are now relative

## Build & Usage

The source has been fixed in order to compile in modern distros like Ubuntu-based Focal (20.04) releases:

```
// Install dependencies

sudo apt-get install build-essential libsdl1.2-dev libsdl-ttf2.0-dev libsdl-mixer1.2-dev libsdl-image1.2-dev libphysfs-dev libenet-dev

// Build project

cd egoboo/src
make all
```

After compiling, navigate to the '**src/game**' folder and copy the file '**egoboo-2.x**' to the desired directory. Also you'll need to copy the '**basicdat**', '**config**' and '**modules**' folders located in the root folder over the same directory in which the binary is located. Finally, run with:

```
./egoboo-2.x
```