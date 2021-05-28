///////////////////////////////////////////////////////////////////////////////////////////////////
/* Keyboard Driver
	keybd.h

	This header defines special ASCII codes, and uniquely identifies different key combinations.

	A basic keyboard driver that registers itself to receive keyboard interrupts, and reads the key
	scancode from the keyboard port. It converts scancodes into ASCII and displays some of them on
	the screen, others are converted into commands to send to the server.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _KEYBD_H
#define _KEYBD_H

#include <stdint.h>

#define CAPS_OFF		0
#define CAPS_ON			1
#define SHIFT_OFF		0
#define SHIFT_ON		1
#define ALT_OFF			0
#define ALT_ON			2
#define CTRL_OFF		0
#define CTRL_ON			4

#define NUMLOCK_ON		1
#define NUMLOCK_OFF		0

/* Scancodes for the special modifier keys.*/
#define SC_CTRL			29
#define SC_LSHIFT		42
#define SC_RSHIFT		54
#define SC_ALT			56
#define SC_BACKSPACE	14
#define SC_TAB			15
#define SC_ESC			1

#define	Ctrl(c)		((c) | CTRL)	
#define Alt(c)		((c) | ALT)		
#define CtrlAlt(c)	Alt(Ctrl(c))

#define EXT		0x0100			/* Normal function keys	*/
#define CTRL	0x0200			/* Control key			*/
#define SHIFT	0x0400			/* Shift key			*/
#define ALT		0x0800			/* Alternate key		*/
#define NUM		0x1000			/* Number pad keys		*/

/* Special ASCII */
#define NUL		0x00			// ^@	\0	Null
#define SOH		0x01			// ^A	Start of Heading
#define STX		0x02			// ^B	Start of Text
#define ETX		0x03			// ^C	End of Text
#define EOT		0x04			// ^D	End of Transmission
#define ENQ		0x05			// ^E	Enquiry
#define ACK		0x06			// ^F	Acknowledgement
#define BEL		0x07			// ^G	\a	Bell
#define BS		0x08			// ^H	\b	Backspace
#define HT		0x09			// ^I	\t	Horizontal Tab
#define LF		0x0A			// ^J	\n	Line Feed
#define VT		0x0B			// ^K	\v	Vertical Tab
#define FF		0x0C			// ^L	\f	Form Feed
#define CR		0x0D			// ^M	\r	Carriage Return
#define SO		0x0E			// ^N	Shift Out
#define SI		0x0F			// ^O	Shift In
#define DLE		0x10			// ^P	Data Link Escape
#define DC1		0x11			// ^Q	Device Control 1 (often XON)
#define DC2		0x12			// ^R	Device Control 2
#define DC3		0x13			// ^S	Device Control 3 (often XOFF)
#define DC4		0x14			// ^T	Device Control 4
#define NAK		0x15			// ^U	Negative Acknowledgement
#define SYN		0x16			// ^V	Synchronous Idle
#define ETB		0x17			// ^W	End of Transmission Block
#define CAN		0x18			// ^X	Cancel
#define EM		0x19			// ^Y	End of Medium
#define SUB		0x1A			// ^Z	Substitute
#define ESC		0x1B			// ^[	\e	Escape
#define FS		0x1C			// ^\	File Separator
#define GS		0x1D			// ^]	Group Separator
#define RS		0x1E			// ^^	Record Separator
#define US		0x1F			// ^_	Unit Separator
//#define DEL	0x7F			// ^?	Delete - this is defined below in the number pad values.

#define ENTER	CR

/* Lock keys */
#define CAPS_LOCK	(0x01 + EXT)
#define	NUM_LOCK	(0x02 + EXT)
#define SCROLL_LOCK	(0x03 + EXT)

/* Numeric keypad */
#define HOME	('7' + NUM)
#define UP		('8' + NUM)
#define PGUP	('9' + NUM)
#define LEFT	('4' + NUM)
#define MID		('5' + NUM)
#define RIGHT	('6' + NUM)
#define END		('1' + NUM)
#define DOWN	('2' + NUM)
#define PGDN	('3' + NUM)
#define INSRT	('0' + NUM)
#define DEL		('.' + NUM)
#define NMIN	('-' + NUM)
#define PLUS	('+' + NUM)

/* Function keys */
#define F1		(0x10 + EXT)
#define F2		(0x11 + EXT)
#define F3		(0x12 + EXT)
#define F4		(0x13 + EXT)
#define F5		(0x14 + EXT)
#define F6		(0x15 + EXT)
#define F7		(0x16 + EXT)
#define F8		(0x17 + EXT)
#define F9		(0x18 + EXT)
#define F10		(0x19 + EXT)
#define F11		(0x1A + EXT)
#define F12		(0x1B + EXT)

/* Shift+Fn */
#define SF1		(F1 + SHIFT)
#define SF2		(F2 + SHIFT)
#define SF3		(F3 + SHIFT)
#define SF4		(F4 + SHIFT)
#define SF5		(F5 + SHIFT)
#define SF6		(F6 + SHIFT)
#define SF7		(F7 + SHIFT)
#define SF8		(F8 + SHIFT)
#define SF9		(F9 + SHIFT)
#define SF10	(F10 + SHIFT)
#define SF11	(F11 + SHIFT)
#define SF12	(F12 + SHIFT)

#define MAP_COLS		2		/* Number of columns in keymap */
#define NR_SCAN_CODES	0x80	/* Number of scan codes (rows in keymap) */
#define KEY_RELEASED	NR_SCAN_CODES

typedef uint16_t keymap_t[NR_SCAN_CODES * MAP_COLS];

keymap_t keymap = {
/* column:			0			1			*/
/* scan-code		!Shift		Shift		*/
/* -----------------------------------------*/
/* 00 - none	*/	0,			0,
/* 01 - ESC	*/		ESC,		ESC,
/* 02 - '1'	*/		'1',		'!',
/* 03 - '2'	*/		'2',		'@',
/* 04 - '3'	*/		'3',		'#',
/* 05 - '4'	*/		'4',		'$',
/* 06 - '5'	*/		'5',		'%',
/* 07 - '6'	*/		'6',		'^',
/* 08 - '7'	*/		'7',		'&',
/* 09 - '8'	*/		'8',		'*',
/* 10 - '9'	*/		'9',		'(',
/* 11 - '0'	*/		'0',		')',
/* 12 - '-'	*/		'-',		'_',
/* 13 - '='	*/		'=',		'+',
/* 14 - BS	*/		Ctrl('H'),		Ctrl('H'),
/* 15 - TAB	*/		Ctrl('I'),		Ctrl('I'),
/* 16 - 'q'	*/		'q',		'Q',
/* 17 - 'w'	*/		'w',		'W',
/* 18 - 'e'	*/		'e',		'E',
/* 19 - 'r'	*/		'r',		'R',
/* 20 - 't'	*/		't',		'T',
/* 21 - 'y'	*/		'y',		'Y',
/* 22 - 'u'	*/		'u',		'U',
/* 23 - 'i'	*/		'i',		'I',
/* 24 - 'o'	*/		'o',		'O',
/* 25 - 'p'	*/		'p',		'P',
/* 26 - '['	*/		'[',		'{',
/* 27 - ']'	*/		']',		'}',
/* 28 - CR/LF	*/	ENTER,		ENTER,
/* 29 - Ctrl	*/	CTRL,		CTRL,
/* 30 - 'a'	*/		'a',		'A',
/* 31 - 's'	*/		's',		'S',
/* 32 - 'd'	*/		'd',		'D',
/* 33 - 'f'	*/		'f',		'F',
/* 34 - 'g'	*/		'g',		'G',
/* 35 - 'h'	*/		'h',		'H',
/* 36 - 'j'	*/		'j',		'J',
/* 37 - 'k'	*/		'k',		'K',
/* 38 - 'l'	*/		'l',		'L',
/* 39 - ';'	*/		';',		':',
/* 40 - '\''	*/	'\'',		'"',
/* 41 - '`'	*/		'`',		'~',
/* 42 - L SHIFT	*/	SHIFT,	SHIFT,
/* 43 - '\\'	*/	'\\',		'|',
/* 44 - 'z'	*/		'z',		'Z',
/* 45 - 'x'	*/		'x',		'X',
/* 46 - 'c'	*/		'c',		'C',
/* 47 - 'v'	*/		'v',		'V',
/* 48 - 'b'	*/		'b',		'B',
/* 49 - 'n'	*/		'n',		'N',
/* 50 - 'm'	*/		'm',		'M',
/* 51 - ','	*/		',',		'<',
/* 52 - '.'	*/		'.',		'>',
/* 53 - '/'	*/		'/',		'?',
/* 54 - R SHIFT */	SHIFT,		SHIFT,
/* 55 - '*'	*/		'*',		'*',
/* 56 - ALT	*/		ALT,		ALT,
/* 57 - ' '	*/		' ',		' ',
/* 58 - CapsLck	*/	CAPS_LOCK,		CAPS_LOCK,
/* 59 - F1	*/		F1,			SF1,
/* 60 - F2	*/		F2,			SF2,
/* 61 - F3	*/		F3,			SF3,
/* 62 - F4	*/		F4,			SF4,
/* 63 - F5	*/		F5,			SF5,
/* 64 - F6	*/		F6,			SF6,
/* 65 - F7	*/		F7,			SF7,
/* 66 - F8	*/		F8,			SF8,
/* 67 - F9	*/		F9,			SF9,
/* 68 - F10	*/		F10,		SF10,
/* 69 - NumLock	*/	NUM_LOCK,		NUM_LOCK,
/* 70 - ScrLock	*/	SCROLL_LOCK,		SCROLL_LOCK,
/* 71 - Home	*/	HOME,		HOME,
/* 72 - CurUp	*/	UP,			UP,
/* 73 - PgUp	*/	PGUP,		PGUP,
/* 74 - '-'	*/		NMIN,		NMIN,
/* 75 - Left	*/	LEFT,		LEFT,
/* 76 - MID	*/		MID,		MID,
/* 77 - Right	*/	RIGHT,		RIGHT,
/* 78 - '+'	*/		PLUS,		PLUS,
/* 79 - End	*/		END,		END,
/* 80 - Down	*/	DOWN,		DOWN,
/* 81 - PgDown	*/	PGDN,		PGDN,
/* 82 - Insert	*/	INSRT,		INSRT,
/* 83 - Delete	*/	DEL,		DEL,
/* 84 - Enter	*/	ENTER,		ENTER,
/* 85 - ???	*/		0,			0,
/* 86 - ???	*/		'<',		'>',
/* 87 - F11	*/		F11,		SF11,
/* 88 - F12	*/		F12,		SF12,
/* 89 - ???	*/		0,			0,
/* 90 - ???	*/		0,			0,
/* 91 - ???	*/		0,			0,
/* 92 - ???	*/		0,			0,
/* 93 - ???	*/		0,			0,
/* 94 - ???	*/		0,			0,
/* 95 - ???	*/		0,			0,
/* 96 - ??? */		0,			0,
/* 97 - ???	*/		0,			0,
/* 98 - ???	*/		0,			0,
/* 99 - ???	*/		0,			0,
/*100 - ???	*/		0,			0,
/*101 - ???	*/		0,			0,
/*102 - ???	*/		0,			0,
/*103 - ???	*/		0,			0,
/*104 - ???	*/		0,			0,
/*105 - ???	*/		0,			0,
/*106 - ???	*/		0,			0,
/*107 - ???	*/		0,			0,
/*108 - ???	*/		0,			0,
/*109 - ???	*/		0,			0,
/*110 - ???	*/		0,			0,
/*111 - ???	*/		0,			0,
/*112 - ???	*/		0,			0,
/*113 - ???	*/		0,			0,
/*114 - ???	*/		0,			0,
/*115 - ???	*/		0,			0,
/*116 - ???	*/		0,			0,
/*117 - ???	*/		0,			0,
/*118 - ???	*/		0,			0,
/*119 - ???	*/		0,			0,
/*120 - ???	*/		0,			0,
/*121 - ???	*/		0,			0,
/*122 - ???	*/		0,			0,
/*123 - ???	*/		0,			0,
/*124 - ???	*/		0,			0,
/*125 - ???	*/		0,			0,
/*126 - ???	*/		0,			0,
/*127 - ???	*/		0,			0
};

#endif /* _KEYBD_H */
