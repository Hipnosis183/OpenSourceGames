This is GNU FreeDink, a portable and enhanced version of the Dink
Smallwood game engine.

FreeDink is free software, and you are welcome to redistribute it
under certain conditions; see the GNU GPL for details.
( cf. COPYING and http://gnu.org/licenses/gpl.html )


Building
--------

If you are compiling GNU FreeDink from sources, check BUILD for
instructions.


About Dink Smallwood
--------------------

Dink Smallwood is an adventure/role-playing game, similar to classic
Zelda, made by RTsoft. Besides twisted humour, it includes the actual
game editor, allowing players to create hundreds of new adventures
called Dink Modules or D-Mods for short. The Dink Network
(http://www.dinknetwork.com/) hosts a copy of almost all of them.


GNU FreeDink
------------

GNU FreeDink is a new and portable version of the game engine, which
runs the original game as well as its D-Mods, with close
compatibility, under multiple platforms.

FreeDink can run in 2 modes:

* v1.08 (default): this matches the latest release of the Dink engine,
  which includes some new features and some backward-incompatible
  changes. Use it for recent D-Mods.

* v1.07 (with option '--v1.07'): this matches v1.07 of the Dink
  engine, essentially unmodified since 1998. Use it to play D-Mods
  released before 2006.

Next step is extending the engine while preserving accurate support
for released D-Mods.

On the technical level, the internals of the engines experience a
clean-up to allow for portability and later improvements.

Check out FreeDink's website at http://www.freedink.org/ .


Game keys
---------

Keyboard controls
  <ARROW KEYS>	Makes Dink walk/push
  <CTRL>	Attack/Use equipped item/Select option
  <SPACE>	Talk/Examine/Manipulate
  <SHIFT>	Use equipped magic
  <ENTER>	Inventory/Equip screen
  <ESC>		Options
  <M>		Display map (if you have one)
  <Alt-Return>	Toggle fullscreen mode
  <Tab>		When maintained, enable Turbo mode

Keyboard controls: Inventory/Equip screen
  <ARROW KEYS>	Highlight Item/Weapon or Magic
  <CTRL>	Select highlighted Item/Weapon or Magic
  <ENTER>	Exit inventory screen

Joystick/Gamepad controls
  <ANY DIR.>	Makes Dink walk/push
  <BUTTON 1>	Attack/Use equipped item/Select option
  <BUTTON 2>	Talk/Examine/Manipulate
  <BUTTON 3>	Use equipped magic
  <BUTTON 4>	Inventory/Equip screen
  <BUTTON 5>	Options
  <BUTTON 6>	Display map (if you have one)

Joystick/Gamepad controls: Inventory/Equip screen
  <ANY DIR.>	Highlight Item/Weapon or Magic
  <BUTTON 1>	Select highlighted Item/Weapon or Magic
  <BUTTON 4>	Exit Inventory/Equip screen

Other
  <Alt-M>	Stop playing current MIDI song
  <Alt-Q>	Quick quit to Windows
  <Alt-D>	Enable debug mode

D-Mods may add additional keys, usually they will explain it within
the game.


About GNU
---------

FreeDink is part of the GNU project, whose aim is to create "GNU", a
completely free (as in freedom) operating system. Its most famous
variant these days is GNU/Linux. A engine to play the Dink Smallwood
game and D-Mods is definitely a must for the GNU OS ;) Check
http://www.gnu.org/ for more information.


Game data
---------

Get the free game data separately from the 'freedink-data' package at:

  http://ftp.gnu.org/gnu/freedink/

Note:
The original game contained sounds that were not owned by RTsoft and
hence could not be released under a free license.  We're looking for
free replacements!  Please send contributions at bug-freedink@gnu.org.
See http://www.freedink.org/doc/sounds/ for more information.


Other FreeDink applications
---------------------------

DFArc, a graphical front-end run the game and manage D-Mods, was also
ported and extended. Get new version 3 at http://www.freedink.org/ .


Technical information
---------------------

Check the 'doc' directory for technical/developer considerations.


---
Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.
