/*
Statically compiled font for emergency messages

Copyright (C) 2004 Huw D M Davies, Dmitry Timoshkov

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,
USA
*/

/*
Origin: vgasys.fon, somehow built from system.sfd, comes from Wine
(http://winehq.org). We keep a prebuilt vgasys.fon because it's
complicated to rebuild it, see the fonts/ directory in the Wine source
package.
*/

/* Translated to a C static char array using:

   (echo "unsigned char vgasys_fon[] = {";
    od -v -txC ../share/freedink/vgasys.fon \
    | sed -e 's/^[0-9]\+//' -e s'/ \([0-9a-f][0-9a-f]\)/0x\1,/g' -e'$d' \
    | sed -e'$s/,$/};/'
   ) > vgasys_fon.c

   It's a modified version from
   http://www.codeguru.com/forum/showthread.php?t=373518
   (I added the -v option for od and removed 'const')
*/

/* Note: I read there're various implementations of a tool called
   'bin2c' to do the job.  This might come in handy if there's ever
   need to do that again. */

extern const unsigned char vgasys_fon[6512] = {
0x4d,0x5a,0x0d,0x01,0x01,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0xff,0xff,0x00,0x00,
0xb8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,
0x0e,0x1f,0xba,0x0e,0x00,0xb4,0x09,0xcd,0x21,0xb8,0x01,0x4c,0xcd,0x21,0x54,0x68,
0x69,0x73,0x20,0x50,0x72,0x6f,0x67,0x72,0x61,0x6d,0x20,0x63,0x61,0x6e,0x6e,0x6f,
0x74,0x20,0x62,0x65,0x20,0x72,0x75,0x6e,0x20,0x69,0x6e,0x20,0x44,0x4f,0x53,0x20,
0x6d,0x6f,0x64,0x65,0x0d,0x0a,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x4e,0x45,0x05,0x01,0x84,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x83,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x2b,0x00,0x40,0x00,0x40,0x00,0x7a,0x00,0x84,0x00,0x84,0x00,0x06,0x01,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x04,0x00,0x07,0x80,0x01,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x08,0x00,0x50,0x00,
0x32,0x00,0x00,0x00,0x00,0x00,0x08,0x80,0x01,0x00,0x00,0x00,0x00,0x00,0x1c,0x00,
0x7b,0x01,0x30,0x10,0x50,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x07,0x46,0x4f,0x4e,0x54,0x44,0x49,0x52,0x06,0x53,0x79,0x73,0x74,0x65,
0x6d,0x00,0x00,0x00,0x00,0x00,0x27,0x46,0x4f,0x4e,0x54,0x52,0x45,0x53,0x20,0x31,
0x30,0x30,0x2c,0x39,0x36,0x2c,0x39,0x36,0x20,0x3a,0x20,0x53,0x79,0x73,0x74,0x65,
0x6d,0x20,0x31,0x30,0x20,0x28,0x56,0x47,0x41,0x20,0x72,0x65,0x73,0x29,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0x00,0x50,0x00,0x00,0x03,0xa7,0x17,0x00,0x00,0x43,0x6f,0x70,0x79,0x72,0x69,
0x67,0x68,0x74,0x20,0x28,0x43,0x29,0x20,0x32,0x30,0x30,0x34,0x20,0x48,0x75,0x77,
0x20,0x44,0x20,0x4d,0x20,0x44,0x61,0x76,0x69,0x65,0x73,0x2c,0x20,0x44,0x6d,0x69,
0x74,0x72,0x79,0x20,0x54,0x69,0x6d,0x6f,0x73,0x68,0x6b,0x6f,0x76,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x00,0x60,0x00,0x60,0x00,0x0d,0x00,
0x03,0x00,0x00,0x00,0x00,0x00,0x00,0xbc,0x02,0x00,0x00,0x00,0x10,0x00,0x21,0x07,
0x00,0x0f,0x00,0x20,0xff,0x60,0x00,0x1c,0x01,0x00,0x00,0x00,0x00,0xa0,0x17,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x53,0x79,0x73,0x74,0x65,0x6d,0x00,0x00,0x00,0x00,
0x00,0x03,0xa7,0x17,0x00,0x00,0x43,0x6f,0x70,0x79,0x72,0x69,0x67,0x68,0x74,0x20,
0x28,0x43,0x29,0x20,0x32,0x30,0x30,0x34,0x20,0x48,0x75,0x77,0x20,0x44,0x20,0x4d,
0x20,0x44,0x61,0x76,0x69,0x65,0x73,0x2c,0x20,0x44,0x6d,0x69,0x74,0x72,0x79,0x20,
0x54,0x69,0x6d,0x6f,0x73,0x68,0x6b,0x6f,0x76,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0a,0x00,0x60,0x00,0x60,0x00,0x0d,0x00,0x03,0x00,0x00,0x00,
0x00,0x00,0x00,0xbc,0x02,0x00,0x00,0x00,0x10,0x00,0x21,0x07,0x00,0x0f,0x00,0x20,
0xff,0x60,0x00,0x1c,0x01,0x00,0x00,0x00,0x00,0xa0,0x17,0x00,0x00,0x00,0x00,0x00,
0x00,0xe0,0x05,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x04,0x00,0xe0,0x05,0x00,0x00,0x04,0x00,0xf0,0x05,0x00,0x00,
0x06,0x00,0x00,0x06,0x00,0x00,0x08,0x00,0x10,0x06,0x00,0x00,0x08,0x00,0x20,0x06,
0x00,0x00,0x0b,0x00,0x30,0x06,0x00,0x00,0x09,0x00,0x50,0x06,0x00,0x00,0x04,0x00,
0x70,0x06,0x00,0x00,0x04,0x00,0x80,0x06,0x00,0x00,0x04,0x00,0x90,0x06,0x00,0x00,
0x06,0x00,0xa0,0x06,0x00,0x00,0x08,0x00,0xb0,0x06,0x00,0x00,0x04,0x00,0xc0,0x06,
0x00,0x00,0x04,0x00,0xd0,0x06,0x00,0x00,0x04,0x00,0xe0,0x06,0x00,0x00,0x04,0x00,
0xf0,0x06,0x00,0x00,0x08,0x00,0x00,0x07,0x00,0x00,0x08,0x00,0x10,0x07,0x00,0x00,
0x08,0x00,0x20,0x07,0x00,0x00,0x08,0x00,0x30,0x07,0x00,0x00,0x08,0x00,0x40,0x07,
0x00,0x00,0x08,0x00,0x50,0x07,0x00,0x00,0x08,0x00,0x60,0x07,0x00,0x00,0x08,0x00,
0x70,0x07,0x00,0x00,0x08,0x00,0x80,0x07,0x00,0x00,0x08,0x00,0x90,0x07,0x00,0x00,
0x04,0x00,0xa0,0x07,0x00,0x00,0x04,0x00,0xb0,0x07,0x00,0x00,0x08,0x00,0xc0,0x07,
0x00,0x00,0x08,0x00,0xd0,0x07,0x00,0x00,0x08,0x00,0xe0,0x07,0x00,0x00,0x08,0x00,
0xf0,0x07,0x00,0x00,0x0e,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x20,0x08,0x00,0x00,
0x0a,0x00,0x30,0x08,0x00,0x00,0x09,0x00,0x50,0x08,0x00,0x00,0x0a,0x00,0x70,0x08,
0x00,0x00,0x09,0x00,0x90,0x08,0x00,0x00,0x08,0x00,0xb0,0x08,0x00,0x00,0x0a,0x00,
0xc0,0x08,0x00,0x00,0x0a,0x00,0xe0,0x08,0x00,0x00,0x04,0x00,0x00,0x09,0x00,0x00,
0x07,0x00,0x10,0x09,0x00,0x00,0x09,0x00,0x20,0x09,0x00,0x00,0x08,0x00,0x40,0x09,
0x00,0x00,0x0c,0x00,0x50,0x09,0x00,0x00,0x0a,0x00,0x70,0x09,0x00,0x00,0x0a,0x00,
0x90,0x09,0x00,0x00,0x09,0x00,0xb0,0x09,0x00,0x00,0x0a,0x00,0xd0,0x09,0x00,0x00,
0x0a,0x00,0xf0,0x09,0x00,0x00,0x09,0x00,0x10,0x0a,0x00,0x00,0x08,0x00,0x30,0x0a,
0x00,0x00,0x0a,0x00,0x40,0x0a,0x00,0x00,0x08,0x00,0x60,0x0a,0x00,0x00,0x0e,0x00,
0x70,0x0a,0x00,0x00,0x09,0x00,0x90,0x0a,0x00,0x00,0x0a,0x00,0xb0,0x0a,0x00,0x00,
0x09,0x00,0xd0,0x0a,0x00,0x00,0x04,0x00,0xf0,0x0a,0x00,0x00,0x04,0x00,0x00,0x0b,
0x00,0x00,0x04,0x00,0x10,0x0b,0x00,0x00,0x05,0x00,0x20,0x0b,0x00,0x00,0x08,0x00,
0x30,0x0b,0x00,0x00,0x05,0x00,0x40,0x0b,0x00,0x00,0x08,0x00,0x50,0x0b,0x00,0x00,
0x08,0x00,0x60,0x0b,0x00,0x00,0x07,0x00,0x70,0x0b,0x00,0x00,0x08,0x00,0x80,0x0b,
0x00,0x00,0x08,0x00,0x90,0x0b,0x00,0x00,0x04,0x00,0xa0,0x0b,0x00,0x00,0x08,0x00,
0xb0,0x0b,0x00,0x00,0x08,0x00,0xc0,0x0b,0x00,0x00,0x04,0x00,0xd0,0x0b,0x00,0x00,
0x04,0x00,0xe0,0x0b,0x00,0x00,0x07,0x00,0xf0,0x0b,0x00,0x00,0x04,0x00,0x00,0x0c,
0x00,0x00,0x0c,0x00,0x10,0x0c,0x00,0x00,0x08,0x00,0x30,0x0c,0x00,0x00,0x08,0x00,
0x40,0x0c,0x00,0x00,0x08,0x00,0x50,0x0c,0x00,0x00,0x08,0x00,0x60,0x0c,0x00,0x00,
0x05,0x00,0x70,0x0c,0x00,0x00,0x08,0x00,0x80,0x0c,0x00,0x00,0x04,0x00,0x90,0x0c,
0x00,0x00,0x08,0x00,0xa0,0x0c,0x00,0x00,0x08,0x00,0xb0,0x0c,0x00,0x00,0x0a,0x00,
0xc0,0x0c,0x00,0x00,0x08,0x00,0xe0,0x0c,0x00,0x00,0x08,0x00,0xf0,0x0c,0x00,0x00,
0x08,0x00,0x00,0x0d,0x00,0x00,0x05,0x00,0x10,0x0d,0x00,0x00,0x04,0x00,0x20,0x0d,
0x00,0x00,0x05,0x00,0x30,0x0d,0x00,0x00,0x05,0x00,0x40,0x0d,0x00,0x00,0x04,0x00,
0x50,0x0d,0x00,0x00,0x04,0x00,0x60,0x0d,0x00,0x00,0x04,0x00,0x70,0x0d,0x00,0x00,
0x05,0x00,0x80,0x0d,0x00,0x00,0x04,0x00,0x90,0x0d,0x00,0x00,0x06,0x00,0xa0,0x0d,
0x00,0x00,0x08,0x00,0xb0,0x0d,0x00,0x00,0x0a,0x00,0xc0,0x0d,0x00,0x00,0x0a,0x00,
0xe0,0x0d,0x00,0x00,0x04,0x00,0x00,0x0e,0x00,0x00,0x0f,0x00,0x10,0x0e,0x00,0x00,
0x09,0x00,0x30,0x0e,0x00,0x00,0x08,0x00,0x50,0x0e,0x00,0x00,0x04,0x00,0x60,0x0e,
0x00,0x00,0x04,0x00,0x70,0x0e,0x00,0x00,0x09,0x00,0x80,0x0e,0x00,0x00,0x04,0x00,
0xa0,0x0e,0x00,0x00,0x04,0x00,0xb0,0x0e,0x00,0x00,0x04,0x00,0xc0,0x0e,0x00,0x00,
0x04,0x00,0xd0,0x0e,0x00,0x00,0x07,0x00,0xe0,0x0e,0x00,0x00,0x07,0x00,0xf0,0x0e,
0x00,0x00,0x07,0x00,0x00,0x0f,0x00,0x00,0x06,0x00,0x10,0x0f,0x00,0x00,0x09,0x00,
0x20,0x0f,0x00,0x00,0x04,0x00,0x40,0x0f,0x00,0x00,0x09,0x00,0x50,0x0f,0x00,0x00,
0x08,0x00,0x70,0x0f,0x00,0x00,0x07,0x00,0x80,0x0f,0x00,0x00,0x04,0x00,0x90,0x0f,
0x00,0x00,0x04,0x00,0xa0,0x0f,0x00,0x00,0x08,0x00,0xb0,0x0f,0x00,0x00,0x04,0x00,
0xc0,0x0f,0x00,0x00,0x09,0x00,0xd0,0x0f,0x00,0x00,0x04,0x00,0xf0,0x0f,0x00,0x00,
0x08,0x00,0x00,0x10,0x00,0x00,0x08,0x00,0x10,0x10,0x00,0x00,0x08,0x00,0x20,0x10,
0x00,0x00,0x08,0x00,0x30,0x10,0x00,0x00,0x04,0x00,0x40,0x10,0x00,0x00,0x08,0x00,
0x50,0x10,0x00,0x00,0x05,0x00,0x60,0x10,0x00,0x00,0x0a,0x00,0x70,0x10,0x00,0x00,
0x05,0x00,0x90,0x10,0x00,0x00,0x07,0x00,0xa0,0x10,0x00,0x00,0x08,0x00,0xb0,0x10,
0x00,0x00,0x04,0x00,0xc0,0x10,0x00,0x00,0x0a,0x00,0xd0,0x10,0x00,0x00,0x08,0x00,
0xf0,0x10,0x00,0x00,0x05,0x00,0x00,0x11,0x00,0x00,0x08,0x00,0x10,0x11,0x00,0x00,
0x04,0x00,0x20,0x11,0x00,0x00,0x04,0x00,0x30,0x11,0x00,0x00,0x05,0x00,0x40,0x11,
0x00,0x00,0x08,0x00,0x50,0x11,0x00,0x00,0x07,0x00,0x60,0x11,0x00,0x00,0x04,0x00,
0x70,0x11,0x00,0x00,0x05,0x00,0x80,0x11,0x00,0x00,0x04,0x00,0x90,0x11,0x00,0x00,
0x05,0x00,0xa0,0x11,0x00,0x00,0x07,0x00,0xb0,0x11,0x00,0x00,0x0b,0x00,0xc0,0x11,
0x00,0x00,0x0b,0x00,0xe0,0x11,0x00,0x00,0x0b,0x00,0x00,0x12,0x00,0x00,0x08,0x00,
0x20,0x12,0x00,0x00,0x08,0x00,0x30,0x12,0x00,0x00,0x08,0x00,0x40,0x12,0x00,0x00,
0x08,0x00,0x50,0x12,0x00,0x00,0x08,0x00,0x60,0x12,0x00,0x00,0x08,0x00,0x70,0x12,
0x00,0x00,0x08,0x00,0x80,0x12,0x00,0x00,0x0d,0x00,0x90,0x12,0x00,0x00,0x09,0x00,
0xb0,0x12,0x00,0x00,0x09,0x00,0xd0,0x12,0x00,0x00,0x09,0x00,0xf0,0x12,0x00,0x00,
0x09,0x00,0x10,0x13,0x00,0x00,0x09,0x00,0x30,0x13,0x00,0x00,0x04,0x00,0x50,0x13,
0x00,0x00,0x04,0x00,0x60,0x13,0x00,0x00,0x04,0x00,0x70,0x13,0x00,0x00,0x04,0x00,
0x80,0x13,0x00,0x00,0x0a,0x00,0x90,0x13,0x00,0x00,0x0a,0x00,0xb0,0x13,0x00,0x00,
0x0a,0x00,0xd0,0x13,0x00,0x00,0x0a,0x00,0xf0,0x13,0x00,0x00,0x0a,0x00,0x10,0x14,
0x00,0x00,0x0a,0x00,0x30,0x14,0x00,0x00,0x0a,0x00,0x50,0x14,0x00,0x00,0x08,0x00,
0x70,0x14,0x00,0x00,0x0a,0x00,0x80,0x14,0x00,0x00,0x0a,0x00,0xa0,0x14,0x00,0x00,
0x0a,0x00,0xc0,0x14,0x00,0x00,0x0a,0x00,0xe0,0x14,0x00,0x00,0x0a,0x00,0x00,0x15,
0x00,0x00,0x0a,0x00,0x20,0x15,0x00,0x00,0x09,0x00,0x40,0x15,0x00,0x00,0x08,0x00,
0x60,0x15,0x00,0x00,0x08,0x00,0x70,0x15,0x00,0x00,0x08,0x00,0x80,0x15,0x00,0x00,
0x08,0x00,0x90,0x15,0x00,0x00,0x08,0x00,0xa0,0x15,0x00,0x00,0x08,0x00,0xb0,0x15,
0x00,0x00,0x08,0x00,0xc0,0x15,0x00,0x00,0x0c,0x00,0xd0,0x15,0x00,0x00,0x07,0x00,
0xf0,0x15,0x00,0x00,0x08,0x00,0x00,0x16,0x00,0x00,0x08,0x00,0x10,0x16,0x00,0x00,
0x08,0x00,0x20,0x16,0x00,0x00,0x08,0x00,0x30,0x16,0x00,0x00,0x04,0x00,0x40,0x16,
0x00,0x00,0x04,0x00,0x50,0x16,0x00,0x00,0x04,0x00,0x60,0x16,0x00,0x00,0x04,0x00,
0x70,0x16,0x00,0x00,0x08,0x00,0x80,0x16,0x00,0x00,0x08,0x00,0x90,0x16,0x00,0x00,
0x08,0x00,0xa0,0x16,0x00,0x00,0x08,0x00,0xb0,0x16,0x00,0x00,0x08,0x00,0xc0,0x16,
0x00,0x00,0x08,0x00,0xd0,0x16,0x00,0x00,0x08,0x00,0xe0,0x16,0x00,0x00,0x06,0x00,
0xf0,0x16,0x00,0x00,0x08,0x00,0x00,0x17,0x00,0x00,0x08,0x00,0x10,0x17,0x00,0x00,
0x08,0x00,0x20,0x17,0x00,0x00,0x08,0x00,0x30,0x17,0x00,0x00,0x08,0x00,0x40,0x17,
0x00,0x00,0x08,0x00,0x50,0x17,0x00,0x00,0x08,0x00,0x60,0x17,0x00,0x00,0x08,0x00,
0x70,0x17,0x00,0x00,0x04,0x00,0x80,0x17,0x00,0x00,0x00,0x00,0x90,0x17,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0xcc,0xcc,0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x36,0x36,0x7f,0x36,0x36,0x6c,0x6c,0xfe,0x6c,0x6c,0x00,0x00,0x00,
0x00,0x00,0x18,0x18,0x3c,0x66,0x60,0x60,0x3c,0x06,0x06,0x66,0x3c,0x18,0x18,0x00,
0x00,0x00,0x00,0x70,0xd8,0xd9,0x73,0x06,0x0c,0x19,0x33,0x63,0xc1,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0xc0,0x80,0x00,0x00,0x00,0xc0,0x60,0x60,0xc0,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x66,0x66,0x3c,0x38,0x6d,0x67,0x66,0x3d,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x30,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x30,
0x00,0x00,0x00,0xc0,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0xc0,
0x00,0x00,0x00,0x30,0x30,0xfc,0x30,0x78,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x7e,0x18,0x18,0x18,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0xc0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x30,0x30,0x30,0x30,0x60,0x60,0x60,0x60,0xc0,0xc0,0xc0,0xc0,0x00,
0x00,0x00,0x00,0x3c,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x18,0x78,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x66,0x06,0x0c,0x18,0x30,0x60,0x60,0x7e,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x06,0x06,0x3c,0x06,0x06,0x06,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x0e,0x1e,0x36,0x36,0x66,0x66,0x66,0x7f,0x06,0x06,0x00,0x00,0x00,
0x00,0x00,0x00,0x7e,0x60,0x60,0x60,0x7c,0x66,0x06,0x06,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x60,0x60,0x7c,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x7e,0x06,0x0c,0x0c,0x18,0x18,0x30,0x30,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x66,0x66,0x3c,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x66,0x66,0x66,0x3e,0x06,0x06,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x60,0x60,0xc0,0x00,0x00,
0x00,0x00,0x00,0x00,0x06,0x0c,0x18,0x30,0x60,0x30,0x18,0x0c,0x06,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x30,0x18,0x0c,0x06,0x0c,0x18,0x30,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x66,0x06,0x0c,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,
0x00,0x00,0x0f,0x18,0x30,0x67,0x6c,0x6c,0x6c,0x6c,0x63,0x30,0x18,0x0f,0x00,0x00,
0x00,0x00,0xc0,0x60,0x30,0x98,0xd8,0xd8,0xd8,0xf8,0x70,0x00,0x70,0xc0,0x00,0x00,
0x00,0x00,0x00,0x18,0x18,0x3c,0x3c,0x66,0x66,0x66,0x7e,0xc3,0xc3,0x00,0x00,0x00,
0x00,0x00,0x00,0x7f,0x61,0x61,0x61,0x7f,0x61,0x61,0x61,0x61,0x7f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x1e,0x33,0x61,0x60,0x60,0x60,0x60,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x7e,0x63,0x61,0x61,0x61,0x61,0x61,0x61,0x63,0x7e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x7f,0x60,0x60,0x60,0x7e,0x60,0x60,0x60,0x60,0x7f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x7f,0x60,0x60,0x60,0x7e,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x1e,0x33,0x61,0x60,0x60,0x67,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x61,0x61,0x61,0x61,0x7f,0x61,0x61,0x61,0x61,0x61,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0xcc,0xcc,0x78,0x00,0x00,0x00,
0x00,0x00,0x00,0x63,0x66,0x6c,0x78,0x70,0x78,0x6c,0x66,0x63,0x61,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7f,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x70,0x70,0x79,0x79,0x6f,0x6f,0x66,0x66,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0xe0,0xe0,0xe0,0xe0,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x61,0x71,0x71,0x79,0x6d,0x6d,0x67,0x63,0x63,0x61,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x1e,0x33,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x7f,0x61,0x61,0x61,0x61,0x7f,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x1e,0x33,0x61,0x61,0x61,0x61,0x61,0x67,0x33,0x1f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x7f,0x61,0x61,0x61,0x61,0x7f,0x61,0x61,0x61,0x61,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x3e,0x63,0x63,0x60,0x38,0x0e,0x03,0x63,0x63,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xff,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xc3,0xc3,0xc3,0x66,0x66,0x66,0x3c,0x3c,0x18,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0xc3,0xc3,0xc3,0x67,0x67,0x67,0x3c,0x3c,0x18,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x0c,0x0c,0x0c,0x98,0x98,0x98,0xf0,0xf0,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0xc1,0xc1,0x63,0x36,0x1c,0x1c,0x36,0x63,0xc1,0xc1,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0xc0,0xc0,0x61,0x33,0x1e,0x0c,0x0c,0x0c,0x0c,0x0c,0x00,0x00,0x00,
0x00,0x00,0x00,0xc0,0xc0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xff,0x01,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0xff,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x70,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x70,
0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x60,0x60,0x60,0x60,0x30,0x30,0x30,0x30,0x00,
0x00,0x00,0x00,0xe0,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0xe0,
0x00,0x00,0x00,0x20,0x70,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
0x00,0x00,0x60,0x30,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x06,0x3e,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x60,0x7c,0x66,0x66,0x66,0x66,0x66,0x7c,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x60,0x60,0x60,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x06,0x06,0x06,0x3e,0x66,0x66,0x66,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x66,0x7c,0x60,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x30,0x60,0x60,0xf0,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x66,0x66,0x66,0x66,0x66,0x3e,0x06,0x66,0x3c,
0x00,0x00,0x00,0x60,0x60,0x60,0x7c,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0xc0,
0x00,0x00,0x00,0x60,0x60,0x60,0x66,0x6c,0x78,0x70,0x78,0x6c,0x66,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0x66,0x66,0x66,0x66,0x66,0x7c,0x60,0x60,0x60,
0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x66,0x66,0x66,0x66,0x66,0x3e,0x06,0x06,0x06,
0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x70,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x30,0x18,0x0c,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0xf0,0x60,0x60,0x60,0x60,0x60,0x30,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xc3,0xc3,0x66,0x66,0x3c,0x18,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xcc,0x6d,0x6d,0x3f,0x33,0x33,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xc3,0x66,0x3c,0x18,0x3c,0x66,0xc3,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xc3,0xc3,0x66,0x66,0x3c,0x3c,0x18,0x18,0x30,0x60,
0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x06,0x0c,0x18,0x30,0x60,0x7e,0x00,0x00,0x00,
0x00,0x00,0x00,0x18,0x30,0x30,0x30,0x30,0x30,0x60,0x30,0x30,0x30,0x30,0x30,0x18,
0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,
0x00,0x00,0x00,0xc0,0x60,0x60,0x60,0x60,0x60,0x30,0x60,0x60,0x60,0x60,0x60,0xc0,
0x00,0x00,0x00,0xe8,0xb8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x60,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6c,0xd8,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xdb,0xdb,0x00,0x00,0x00,
0x00,0x00,0x00,0x0c,0x0c,0x7f,0x7f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0c,0x0c,0x7f,0x7f,0x0c,0x0c,0x0c,0x7f,0x7f,0x0c,0x0c,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x70,0xd8,0xd9,0x73,0x06,0x0c,0x1b,0x36,0x66,0xc3,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0xc0,0x80,0x00,0x00,0x00,0x9c,0xf6,0xf6,0x9c,0x00,0x00,0x00,
0x36,0x1c,0x00,0x3e,0x63,0x63,0x60,0x38,0x0e,0x03,0x63,0x63,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x0c,0x18,0x30,0x60,0x30,0x18,0x0c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x36,0x1c,0x00,0xff,0x01,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0xff,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x60,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x6c,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x36,0x6c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x78,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0xed,0x4a,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x66,0x3c,0x18,0x00,0x3c,0x66,0x30,0x18,0x0c,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x60,0x30,0x18,0x0c,0x18,0x30,0x60,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x66,0x3c,0x18,0x00,0x7e,0x06,0x0c,0x18,0x30,0x60,0x7e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x08,0x3c,0x6e,0x68,0x68,0x70,0x76,0x3c,0x10,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x60,0x30,0x30,0x7c,0x30,0x30,0x60,0x7e,0x00,0x00,0x00,
0x00,0x00,0x00,0x66,0x3c,0x24,0x24,0x3c,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xc3,0xc3,0x66,0x66,0xff,0x18,0xff,0x18,0x18,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x60,0x60,0x60,0x60,0x60,0x60,
0x00,0x00,0x00,0x3c,0x66,0x38,0x78,0x2c,0x34,0x1e,0x1c,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x3f,0x61,0xcc,0xd2,0xd0,0xd0,0xd2,0xcc,0x61,0x3f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x10,0x70,0x50,0x70,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x6c,0xd8,0x6c,0x36,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x06,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x3f,0x61,0xde,0xd2,0xde,0xd8,0xd4,0xd2,0x61,0x3f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x80,0x00,0x00,0x00,0x00,
0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x70,0x50,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7e,0x18,0x18,0x00,0x7e,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0xb0,0x30,0x60,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0xb0,0x60,0xb0,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x60,0x60,0x60,
0x00,0x00,0x00,0x3c,0x7c,0x7c,0x7c,0x7c,0x3c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x18,0x70,
0x00,0x00,0x00,0x60,0xe0,0x60,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x70,0x50,0x50,0x50,0x70,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0x6c,0x36,0x6c,0xd8,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x30,0x70,0x31,0x33,0x36,0x0c,0x19,0x32,0x63,0xc0,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0xc0,0x80,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,
0x00,0x00,0x00,0x30,0x70,0x31,0x33,0x36,0x0d,0x1a,0x30,0x61,0xc3,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0xc0,0x80,0x00,0x00,0x80,0xc0,0xc0,0x80,0xc0,0x00,0x00,0x00,
0x00,0x00,0x00,0x30,0x58,0x31,0x5b,0x36,0x0c,0x19,0x32,0x63,0xc0,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0xc0,0x80,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,
0x00,0x00,0x00,0x18,0x18,0x00,0x18,0x18,0x30,0x60,0x66,0x66,0x3c,0x00,0x00,0x00,
0x38,0x18,0x0c,0x18,0x18,0x3c,0x3c,0x66,0x66,0x66,0x7e,0xc3,0xc3,0x00,0x00,0x00,
0x1c,0x18,0x30,0x18,0x18,0x3c,0x3c,0x66,0x66,0x66,0x7e,0xc3,0xc3,0x00,0x00,0x00,
0x18,0x3c,0x66,0x18,0x18,0x3c,0x3c,0x66,0x66,0x66,0x7e,0xc3,0xc3,0x00,0x00,0x00,
0x7a,0x5e,0x00,0x18,0x18,0x3c,0x3c,0x66,0x66,0x66,0x7e,0xc3,0xc3,0x00,0x00,0x00,
0x66,0x66,0x00,0x18,0x18,0x3c,0x3c,0x66,0x66,0x66,0x7e,0xc3,0xc3,0x00,0x00,0x00,
0x18,0x3c,0x18,0x00,0x18,0x18,0x3c,0x66,0x66,0x66,0x7e,0xc3,0xc3,0x00,0x00,0x00,
0x00,0x00,0x00,0x1f,0x1e,0x36,0x36,0x37,0x66,0x66,0x7e,0xc6,0xc7,0x00,0x00,0x00,
0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0xe0,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,
0x00,0x00,0x00,0x1e,0x33,0x61,0x60,0x60,0x60,0x60,0x61,0x33,0x1e,0x0c,0x06,0x1c,
0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,
0x38,0x18,0x0c,0x7f,0x60,0x60,0x60,0x7e,0x60,0x60,0x60,0x60,0x7f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0e,0x0c,0x18,0x7f,0x60,0x60,0x60,0x7e,0x60,0x60,0x60,0x60,0x7f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0c,0x1e,0x33,0x7f,0x60,0x60,0x60,0x7e,0x60,0x60,0x60,0x60,0x7f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x33,0x33,0x00,0x7f,0x60,0x60,0x60,0x7e,0x60,0x60,0x60,0x60,0x7f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xc0,0x60,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x30,0x60,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x60,0x90,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x90,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x7e,0x63,0x61,0x61,0xf9,0x61,0x61,0x61,0x63,0x7e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x3d,0x2f,0x00,0x61,0x71,0x71,0x79,0x6d,0x6d,0x67,0x63,0x63,0x61,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,
0x1c,0x0c,0x06,0x1e,0x33,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x0e,0x0c,0x18,0x1e,0x33,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x0c,0x1e,0x33,0x1e,0x33,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x3d,0x2f,0x00,0x1e,0x33,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x33,0x33,0x00,0x1e,0x33,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x3c,0x18,0x3c,0x66,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x1f,0x33,0x67,0x67,0x6d,0x6d,0x79,0x79,0x33,0x7e,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x1c,0x0c,0x06,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x0e,0x0c,0x18,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x0c,0x1e,0x33,0x00,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x33,0x33,0x00,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x33,0x1e,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x07,0x06,0x0c,0xc0,0xc0,0x61,0x33,0x1e,0x0c,0x0c,0x0c,0x0c,0x0c,0x00,0x00,0x00,
0x00,0x00,0x00,0xc0,0xc0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x60,0x7e,0x63,0x63,0x63,0x63,0x7e,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x3c,0x66,0x66,0x66,0x6c,0x66,0x66,0x66,0x66,0x6c,0x00,0x00,0x00,
0x00,0x00,0x38,0x18,0x0c,0x00,0x3c,0x66,0x06,0x3e,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x1c,0x18,0x30,0x00,0x3c,0x66,0x06,0x3e,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x18,0x3c,0x66,0x00,0x3c,0x66,0x06,0x3e,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x7a,0x5e,0x00,0x3c,0x66,0x06,0x3e,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x66,0x66,0x00,0x3c,0x66,0x06,0x3e,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x18,0x3c,0x18,0x00,0x3c,0x66,0x06,0x3e,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x66,0x06,0x3f,0x66,0x66,0x3f,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x60,0x60,0xc0,0x00,0x60,0xc0,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x66,0x60,0x60,0x60,0x66,0x3c,0x18,0x0c,0x38,
0x00,0x00,0x38,0x18,0x0c,0x00,0x3c,0x66,0x66,0x7c,0x60,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x1c,0x18,0x30,0x00,0x3c,0x66,0x66,0x7c,0x60,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x18,0x3c,0x66,0x00,0x3c,0x66,0x66,0x7c,0x60,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x66,0x66,0x00,0x3c,0x66,0x66,0x7c,0x60,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0xe0,0x60,0x30,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x70,0x60,0xc0,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0xf0,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x90,0x90,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x76,0x18,0x6c,0x0c,0x3e,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x7a,0x5e,0x00,0x7c,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,
0x00,0x00,0x38,0x18,0x0c,0x00,0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x1c,0x18,0x30,0x00,0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x18,0x3c,0x66,0x00,0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x7a,0x5e,0x00,0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x66,0x66,0x00,0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x78,0x00,0x30,0x30,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x6e,0x6e,0x7e,0x76,0x76,0x7c,0x00,0x00,0x00,
0x00,0x00,0x38,0x18,0x0c,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x1c,0x18,0x30,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x18,0x3c,0x66,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x66,0x66,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3e,0x00,0x00,0x00,
0x00,0x00,0x0e,0x0c,0x18,0x00,0xc3,0xc3,0x66,0x66,0x3c,0x3c,0x18,0x18,0x30,0x60,
0x00,0x00,0x00,0x60,0x60,0x60,0x7c,0x66,0x66,0x66,0x66,0x66,0x7c,0x60,0x60,0x60,
0x00,0x00,0x00,0x66,0x66,0x00,0xc3,0xc3,0x66,0x66,0x3c,0x3c,0x18,0x18,0x30,0x60,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x53,0x79,0x73,0x74,0x65,0x6d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
