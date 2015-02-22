// Copyright (c) 2005-2015 Ross Smith II. See Mit LICENSE in /LICENSE

/*
to do:
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/devcons_3tfc.asp

-? | --prompt	Prompt for user to press a key, then wait for user to press y or Y
                if y/Y not pressed in 15 seconds, reverts back to original mode
-s | --safe     Only change to modes that have already been used previously
                Modes are stored in the registry
*/

#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif

#define _WIN32_WINNT 0x501

#include <windows.h>
#include <WinUser.h>
#include <stdio.h>
//#include <atlbase.h> // required for Stackwalker
#include <limits.h>
#include <stdlib.h>

#include "Stackwalker.h"
#include "Optimize.h"
#include "SystemMessage.h"
#include "debug.h"
#include "getopt.h"

#include "version.h"

#define APPNAME			VER_INTERNAL_NAME
#define APPVERSION		VER_STRING2
#define APPCOPYRIGHT	VER_LEGAL_COPYRIGHT

static char *progname;

static char *short_options = "ab:ce:f:gh:ilLm:M:o:pqrRstTvw:x:y:Z?";

static struct option long_options[] = {
	{"height",			required_argument,	0, 'h'},
	{"h",				required_argument,	0, 'h'},

	{"width",			required_argument,	0, 'w'},
	{"w",				required_argument,	0, 'w'},

	{"bits",			required_argument,	0, 'b'},
	{"bpp",				required_argument,	0, 'b'},
	{"b",				required_argument,	0, 'b'},
	{"depth",			required_argument,	0, 'b'},

	{"current",			no_argument,		0, 'c'},
	{"c",				no_argument,		0, 'c'},

	{"refresh",			required_argument,	0, 'f'},
	{"frequency",		required_argument,	0, 'f'},
	{"freq",			required_argument,	0, 'f'},
	{"f",				required_argument,	0, 'f'},

	{"orientation",		required_argument,	0, 'o'},
	{"o",				required_argument,	0, 'o'},

	{"positionx",		required_argument,	0, 'x'},
	{"x",				required_argument,	0, 'x'},

	{"positiony",		required_argument,	0, 'y'},
	{"y",				required_argument,	0, 'y'},

	{"max",				no_argument,		0, 'a'},
	{"maximum",			no_argument,		0, 'a'},
	{"a",				no_argument,		0, 'a'},

	{"min",				no_argument,		0, 'i'},
	{"mininum",			no_argument,		0, 'i'},
	{"i",				no_argument,		0, 'i'},

	{"mode",			required_argument,	0, 'e'},
	{"e",				required_argument,	0, 'e'},

	{"monitor",			required_argument,	0, 'm'},
	{"m",				required_argument,	0, 'm'},

	{"query",			no_argument,		0, 'q'},
	{"q",				no_argument,		0, 'q'},

	{"list",			no_argument,		0, 'l'},
	{"l",				no_argument,		0, 'l'},

	{"monitor-name",	required_argument,	0, 'M'},
	{"M",				required_argument,	0, 'M'},

	{"list-monitors",	no_argument,		0, 'L'},
	{"L",				no_argument,		0, 'L'},

	{"raw",				no_argument,		0, 'Z'},
	{"Z",				no_argument,		0, 'Z'},

	{"save",			no_argument,		0, 's'},
	{"s",				no_argument,		0, 's'},

	{"global",			no_argument,		0, 'g'},
	{"g",				no_argument,		0, 'g'},

	{"temporary",		no_argument,		0, 'T'},
	{"T",				no_argument,		0, 'T'},

	{"reset",			no_argument,		0, 'r'},
	{"r",				no_argument,		0, 'r'},

	{"no-reset",		no_argument,		0, 'R'},
	{"R",				no_argument,		0, 'R'},

	{"primary",			no_argument,		0, 'p'},
	{"p",				no_argument,		0, 'p'},

	{"test",			no_argument,		0, 't'},
	{"t",				no_argument,		0, 't'},

	{"version",			no_argument,		0, 'v'},
	{"v",				no_argument,		0, 'v'},

	{"help",			no_argument,		0, '?'},
	{"h",				no_argument,		0, '?'},
	{"?",				no_argument,		0, '?'},

	{NULL,				0,					0, 0}
};

struct _resolution {
	char	*name;
	int 	width;
	int 	height;
	char	*description;
	char	*sar;
	char	*dar;
	char	*pixels;
};

typedef struct _resolution Resolution;

static Resolution resolutions[] = {
/*
source: http://en.wikipedia.org/wiki/List_of_common_resolutions
added:  CGA, EGA, WSVGA
							aspect
width	height	pixels		ratio		name		description												application				organization
-------	-------	-----------	-----------	-----------	-------------------------------------------------------	-----------------------	--------------------- */

// Name	Width	Height	Standard	SAR	DAR	Pixels
{"",	16,	16,	"Microvision",	"0.042361111",	"1",	"256"			},
{"",	32,	32,	"PocketStation",	"0.042361111",	"1",	"1024"			},
{"",	40,	30,	"Etch A Sketch Animator",	"0.16875",	"1.333",	"1200"			},
{"",	48,	32,	"GameKing I (GM-218), VMU",	"0.126388889",	"1.5",	"1536"			},
{"",	64,	64,	"Hartung Game Master",	"0.042361111",	"1",	"4096"			},
{"",	75,	64,	"Epoch Game Pocket Computer",	"75:64",	"1.172",	"4800"			},
{"",	150,	40,	"Entex Adventure Vision",	"0.627777778",	"3.75",	"6000"			},
{"",	96,	64,	"Pokémon mini",	"0.126388889",	"1.5",	"6144"			},
{"",	102,	64,	"Ruputer",	"2.147222222",	"0.627",	"6528"			},
{"",	240,	64,	"Atari Portfolio, TRS-80 Model 100",	"0.627777778",	"3.75",	"15360"			},
{"",	160,	102,	"Atari Lynx",	"3.36875",	"1.569",	"16320"			},
{"",	160,	120,	"QQVGA",	"0.16875",	"1.333",	"19200"			},
{"",	120,	160,	"QQVGA (portrait mode)",	"0.127777778",	"0.75",	"19200"			},
{"",	160,	144,	"Nintendo Game Boy, Game Boy Color",	"0.422916667",	"1.111",	"23040"			},
{"",	160,	152,	"Neo Geo Pocket Color",	"0.846527778",	"1.053",	"24320"			},
{"",	160,	160,	"Palm (PDA) LoRES",	"0.042361111",	"1",	"25600"			},
{"",	140,	192,	"Apple II HiRes (6 color) and Apple IIe Double HiRes (16 color), grouping subpixels",	"1.491666667",	"0.729",	"26880"			},
{"",	160,	200,	"VIC-II multicolor, IBM PCjr 16-color, Amstrad CPC",	"0.170138889",	"0.8",	"32000"			},
{"",	224,	144,	"WonderSwan",	"0.589583333",	"1.555",	"32256"			},
{"",	176,	208,	"Nokia Series 60 smartphones (Nokia 7650, plus First and Second Edition models only)",	"0.467361111",	"0.846",	"36608"			},
{"",	240,	160,	"HQVGA, Nintendo Game Boy Advance",	"0.126388889",	"1.5",	"38400"			},
{"",	160,	240,	"HQVGA (portrait mode)",	"0.085416667",	"0.667",	"38400"			},
{"",	176,	220,	"Older Java MIDP devices like Sony Ericsson K600",	"0.170138889",	"0.8",	"38720"			},
{"",	160,	256,	"Acorn BBC 20 column modes",	"0.213888889",	"0.625",	"40960"			},
{"",	208,	208,	"Nokia 5500 Sport",	"0.042361111",	"1",	"43264"			},
{"",	256,	192,	"TMS9918 Modes 1 (e.g. TI-99/4a) and 2, ZX Spectrum, MSX, Nintendo DS (each screen)",	"0.16875",	"1.333",	"49152"			},
{"",	280,	192,	"Apple II HiRes (1 bit per pixel)",	"1.475",	"1.458",	"53760"			},
{"",	240,	240,	"Apple iPod Nano 6G",	"0.042361111",	"1",	"57600"			},
{"",	320,	192,	"Atari 400/800",	"0.210416667",	"1.667",	"61440"			},
{"CGA",	320,	200,	"CGA 4-color, Atari ST 16 color, Commodore 64 VIC-II Hires, Amiga OCS NTSC Lowres, Apple IIGS LoRes, MCGA",	"0.336805556",	"1.6",	"64000"			},
{"",	256,	256,	"Elektronika BK",	"0.042361111",	"1",	"65536"			},
{"",	208,	320,	"UIQ 2.x based smartphones",	"0.555555556",	"0.65",	"66560"			},
{"",	320,	224,	"Sega Nomad",	"0.421527778",	"1.429",	"71680"			},
{"QVGA",	320,	240,	"QVGA, Mega Drive, Nintendo 3DS (lower screen)",	"0.16875",	"1.333",	"76800"			},
{"",	240,	320,	"QVGA (portrait mode)",	"0.127777778",	"0.75",	"76800"			},
{"",	320,	256,	"Acorn BBC 40 column modes, Amiga OCS PAL Lowres",	"0.211111111",	"1.25",	"81920"			},
{"",	400,	240,	"WQVGA (common on Windows Mobile 6 handsets)",	"0.210416667",	"1.667",	"96000"			},
{"",	320,	320,	"Palm (PDA) HiRES",	"0.042361111",	"1",	"102400"			},
{"WQVGA",	432,	240,	"WQVGA",	"0.378472222",	"1.8",	"103680"			},
{"",	560,	192,	"Apple IIe Double Hires (1 bit per pixel)",	"1.466666667",	"2.917",	"107520"			},
{"",	400,	270,	"TurboExpress",	"1.685416667",	"1.481",	"108000"			},
{"",	480,	234,	"A WQVGA variant, used commonly for Portable DVD Players, Digital photo frames, GPS receivers and devices such as the Kenwood DNX-5120, and Glospace SGK-70. Often falsely marketed as \"16:9\"",	"3.360416667",	"2.051",	"112320"			},
{"",	480,	250,	"Teletext and Viewdata 40×25 character screens (PAL non-interlaced)",	"2.017361111",	"1.92",	"120000"			},
{"",	400,	300,	"Quarter SVGA (no official name, selectable in some PC shooters)",	"0.16875",	"1.333",	"120000"			},
{"",	240,	376,	"Apple iPod Nano 5G (portrait mode)",	"0.684027778",	"0.64",	"120320"			},
{"",	640,	200,	"Atari ST 4 color, CGA mono, Amiga OCS NTSC Hires, Apple IIGS HiRes, Nokia Series 80 smartphones",	"0.670138889",	"3.2",	"128000"			},
{"",	480,	272,	"Sony PlayStation Portable (note: When playing video files created by user, the max. resolution is restricted to 368x208(16:9) or 320x240(4:3)), Zune HD",	"1.261805556",	"1.765",	"130560"			},
{"",	512,	256,	"Elektronika BK",	"0.084027778",	"2",	"131072"			},
{"",	352,	416,	"Nokia Series 60 smartphones (E60, E70, N80, N90)",	"0.467361111",	"0.846",	"146432"			},
{"HVGA",	640,	240,	"HVGA, Handheld PC",	"0.335416667",	"2.667",	"153600"			},
{"",	480,	320,	"HVGA, Palm Tungsten T3 Apple iPhone, 3G, & 3GS",	"0.126388889",	"1.5",	"153600"			},
{"",	320,	480,	"HVGA (portrait), Palm (PDA) HiRES+",	"0.085416667",	"0.667",	"153600"			},
{"",	240,	640,	"HVGA (portrait mode)",	"0.130555556",	"0.375",	"153600"			},
{"",	640,	256,	"Acorn BBC 80 column modes, Amiga OCS PAL Hires",	"0.209722222",	"2.5",	"163840"			},
{"",	512,	342,	"Black & white Macintosh (9\")",	"0.126388889",	"1.5",	"175104"			},
{"",	800,	240,	"Nintendo 3DS (upper screen in 3D mode) (2x 400x240, one for each eye)",	"0.41875",	"3.333",	"192000"			},
{"",	512,	384,	"Macintosh LC (12\")/Color Classic (also selectable in many PC shooters)",	"0.16875",	"1.333",	"196608"			},
{"",	640,	320,	"Nokia Series 90 smartphones (7700, 7710)",	"0.084027778",	"2",	"204800"			},
{"EGA",	640,	350,	"EGA",	"2.690972222",	"1.829",	"224000"			},
{"",	640,	360,	"nHD, used by Nokia 5800, Nokia 5530, Nokia X6, Nokia N97, Nokia N8 [2]",	"0.672916667",	"1.778",	"230400"			},
{"",	480,	500,	"Teletext and Viewdata 40×25 character screens (PAL interlaced)",	"1.017361111",	"0.96",	"240000"			},
{"",	720,	348,	"HGC",	"2.520138889",	"2.069",	"250560"			},
{"MDA",	720,	350,	"MDA",	"3.024305556",	"2.057",	"252000"			},
{"",	640,	400,	"Atari ST mono, Amiga OCS NTSC Hires interlaced",	"0.336805556",	"1.6",	"256000"			},
{"",	720,	364,	"Apple Lisa",	"180:91",	"1.978",	"262080"			},
{"D1",	720,	480,	"D1", "", "", ""},
{"D1P",	720,	576,	"D1P", "", "", ""},
{"",	800,	352,	"Nokia E90 Communicator",	"1.049305556",	"2.273",	"281600"			},
{"",	600,	480,	"(unnamed)",	"0.211111111",	"1.25",	"288000"			},
{"VGA",	640,	480,	"VGA, MCGA (in monochome), Sun-1 color",	"0.16875",	"1.333",	"307200"			},
{"",	640,	512,	"Amiga OCS PAL Hires interlaced",	"0.211111111",	"1.25",	"327680"			},
{"WVGA",	768,	480,	"WVGA",	"0.673611111",	"1.6",	"368640"			},
{"WGA",	800,	480,	"WVGA or WGA, List of mobile phones with WVGA display, Nokia 770/800N/N810/N900, Asus Eee PC 700 and 701 series, Pandora",	"0.210416667",	"1.667",	"384000"			},
{"FWVGA",	854,	480,	"FWVGA, List of mobile phones with FWVGA display",	"~16:9",	"1.783",	"410880"			},
{"SVGA",	800,	600,	"SVGA",	"0.16875",	"1.333",	"480000"			},
{"QHD",	960,	540,	"QHD[3], Quarter FHD (AACS ICT), HRHD, Motorola Atrix 4G",	"0.672916667",	"1.778",	"518400"			},
{"",	832,	624,	"Apple Macintosh Half Megapixel[4]",	"0.16875",	"1.333",	"519168"			},
{"",	960,	544,	"PlayStation Vita",	"1.261805556",	"1.765",	"522240"			},
{"WSVGA",	1024,	576,	"WSVGA",	"0.672916667",	"1.778",	"589824"			},
{"",	1024,	600,	"Used in many netbooks",	"128:75",	"1.707",	"614400"			},
{"DVGA",	960,	640,	"DVGA, Apple iPhone 4[5][6],4th Generation iPod Touch[7]",	"0.126388889",	"1.5",	"614400"			},
{"",	1024,	640,	"(Norm. Laptops)",	"0.673611111",	"1.6",	"655360"			},
{"",	960,	720,	"Panasonic DVCPRO100 for 50/60Hz over 720p - SMPTE Resolution",	"0.16875",	"1.333",	"691200"			},
{"XGA",	1024,	768,	"XGA, Apple iPad",	"0.16875",	"1.333",	"786432"			},
{"",	1024,	800,	"Sun-1 monochrome",	"1.350694444",	"1.28",	"819200"			},
{"",	1152,	720,	"(unnamed)",	"0.673611111",	"1.6",	"829440"			},
{"",	1152,	768,	"Apple PowerBook G4 (original Titanium version)",	"0.126388889",	"1.5",	"884736"			},
{"720p",	1280,	720,	"720p (WXGA-H, min.)",	"0.672916667",	"1.778",	"921600"			},
{"WXGA-H",	1280,	720,	"720p (WXGA-H, min.)",	"0.672916667",	"1.778",	"921600"			},
{"",	1120,	832,	"NeXT MegaPixel Display",	"1.476388889",	"1.346",	"931840"			},
{"WXGA-",	1280,	768,	"WXGA, avg., BrightView",	"0.210416667",	"1.667",	"983040"			},
{"XGA+",	1152,	864,	"XGA+[8]",	"0.16875",	"1.333",	"995328"			},
{"WXGA",	1280,	800,	"WXGA, max.",	"0.673611111",	"1.6",	"1024000"			},
{"",	1152,	900,	"Sun-2 Prime Monochrome or Color Video, also common in Sun-3 and Sun-4 workstations",	"1.350694444",	"1.28",	"1036800"			},
{"",	1024,	1024,	"Network Computing Devices",	"0.042361111",	"1",	"1048576"			},
{"720p+",	1366,	768,	"standardized HDTV 720p/1080i displays, HD",	"~16:9",	"1.779",	"1049088"			},
{"HD",	1366,	768,	"standardized HDTV 720p/1080i displays, HD",	"~16:9",	"1.779",	"1049088"			},
{"",	1280,	854,	"Apple PowerBook G4",	"~3:2",	"1.499",	"1093120"			},
{"",	1600,	768,	"Sony VAIO P series",	"1.05",	"2.083",	"1228800"			},
{"SXGA-",	1280,	960,	"SXGA-",	"0.16875",	"1.333",	"1228800"			},
{"WSXGA",	1440,	900,	"WSXGA, WXGA+",	"0.673611111",	"1.6",	"1296000"			},
{"WXGA+",	1440,	900,	"WSXGA, WXGA+",	"0.673611111",	"1.6",	"1296000"			},
{"SXGA",	1280,	1024,	"SXGA",	"0.211111111",	"1.25",	"1310720"			},
{"",	1440,	960,	"Apple PowerBook G4",	"0.126388889",	"1.5",	"1382400"			},
{"900p",	1600,	900,	"900p, HD+",	"0.672916667",	"1.778",	"1440000"			},
{"HD+",	1600,	900,	"900p, HD+",	"0.672916667",	"1.778",	"1440000"			},
{"SXGA+",	1400,	1050,	"SXGA+",	"0.16875",	"1.333",	"1470000"			},
{"",	1440,	1024,	"(unnamed, similar to A4 paper format)",	"1.897222222",	"1.406",	"1474560"			},
{"",	1440,	1080,	"HDV 1080i",	"0.16875",	"1.333",	"1555200"			},
{"",	1600,	1024,	"SGI 1600SW",	"1.052777778",	"1.5625",	"1638400"			},
{"WSXGA+",	1680,	1050,	"WSXGA+",	"0.673611111",	"1.6",	"1764000"			},
{"UXGA",	1600,	1200,	"UXGA",	"0.16875",	"1.333",	"1920000"			},
//{"1080",	1920,	1080,	"HD 1080 (1080i, 1080p), FullHD",	"0.672916667",	"1.778",	"2073600"			},
{"1080i",	1920,	1080,	"HD 1080 (1080i, 1080p), FullHD",	"0.672916667",	"1.778",	"2073600"			},
{"1080p",	1920,	1080,	"HD 1080 (1080i, 1080p), FullHD",	"0.672916667",	"1.778",	"2073600"			},
{"WUXGA",	1920,	1200,	"WUXGA",	"0.673611111",	"1.6",	"2304000"			},
{"QWXGA",	2048,	1152,	"QWXGA, 2K",	"0.672916667",	"1.778",	"2359296"			},
{"2K",	2048,	1152,	"QWXGA, 2K",	"0.672916667",	"1.778",	"2359296"			},
{"",	1792,	1344,	"(unnamed; supported by some GPUs, monitors, and games)",	"0.16875",	"1.333",	"2408448"			},
{"",	1856,	1392,	"(unnamed; supported by some GPUs, monitors, and games)",	"0.16875",	"1.333",	"2583552"			},
{"",	2880,	900,	"NEC CRV43[9], Ostendo CRVD[10], Alienware Curved Display[11][12]",	"0.670138889",	"3.2",	"2592000"			},
{"",	1800,	1440,	"(unnamed; supported by some GPUs, monitors, and games)",	"0.211111111",	"1.25",	"2592000"			},
{"",	2048,	1280,	"(unnamed)",	"0.673611111",	"1.6",	"2621440"			},
{"TXGA",	1920,	1400,	"TXGA",	"2.024305556",	"1.371",	"2688000"			},
{"",	2538,	1080,	"Avielo Optix SuperWide 235 projector[13]",	"1.972222222",	"2.35",	"2741040"			},
{"",	2560,	1080,	"Cinema TV from Philips and Vizio, falsely marketed as '21:9'",	"2.685416667",	"2.37",	"2764800"			},
{"",	1920,	1440,	"(unnamed; supported by some GPUs, monitors, and games)",	"0.16875",	"1.333",	"2764800"			},
{"QXGA",	2048,	1536,	"QXGA",	"0.16875",	"1.333",	"3145728"			},
{"",	2304,	1440,	"(unnamed; maximum resolution of the Sony GDM-FW900 and Hewlett Packard A7217A)",	"0.673611111",	"1.6",	"3317760"			},
{"WQHD",	2560,	1440,	"WQHD (Dell UltraSharp U2711, Apple iMac)",	"0.672916667",	"1.778",	"3686400"			},
{"",	2304,	1728,	"(unnamed; selectable on some displays and graphics cards[14][15])",	"0.16875",	"1.333",	"3981312"			},
{"WQXGA",	2560,	1600,	"WQXGA",	"0.673611111",	"1.6",	"4096000"			},
{"",	2560,	1920,	"(unnamed; the maximum 4:3 resolution of CRT displays. Supported by the Viewsonic P225f and some graphics cards)",	"0.16875",	"1.333",	"4915200"			},
{"QSXGA",	2560,	2048,	"QSXGA",	"0.211111111",	"1.25",	"5242880"			},
{"QSXGA+",	2800,	2100,	"QSXGA+",	"0.16875",	"1.333",	"5880000"			},
{"WQSXGA",	3200,	2048,	"WQSXGA",	"1.052777778",	"1.5625",	"6553600"			},
{"QUXGA",	3200,	2400,	"QUXGA",	"0.16875",	"1.333",	"7680000"			},
{"QFHD",	3840,	2160,	"QFHD",	"0.672916667",	"1.778",	"8294400"			},
{"WQUXGA",	3840,	2400,	"WQUXGA",	"0.673611111",	"1.6",	"9216000"			},
{"4K",	4096,	2304,	"4K",	"0.672916667",	"1.778",	"9437184"			},
{"HXGA",	4096,	3072,	"HXGA",	"0.16875",	"1.333",	"12582912"			},
{"WHXGA",	5120,	3200,	"WHXGA",	"0.673611111",	"1.6",	"16384000"			},
{"HSXGA",	5120,	4096,	"HSXGA",	"0.211111111",	"1.25",	"20971520"			},
{"WHSXGA",	6400,	4096,	"WHSXGA",	"1.052777778",	"1.5625",	"26214400"			},
{"HUXGA",	6400,	4800,	"HUXGA",	"0.16875",	"1.333",	"30720000"			},
{"UHDTV",	7680,	4320,	"UHDTV",	"0.672916667",	"1.778",	"33177600"			},
{"8K",	8192,	4320,	"8K",	"256:135",	"1.896",	"35389440"			},
{"WHUXGA",	7680,	4800,	"WHUXGA",	"0.673611111",	"1.6",	"36864000"			},
{"",	28000,	9334,	"Red Epic 617",	"0.125694444",	"3",	"261352000"			}
};

static void usage() {
	printf(
"Usage: %s [options]\n"
"Options:\n",
		progname
	);

/*2345678901234567890123456789012345678901234567890123456789012345678901234567890*/
	printf(
"-w | --width      x   Set horizontal resolution to 'x' (min/max allowed)\n"
"-h | --height     y   Set vertical resolution to 'y'   (min/max allowed)\n"
"-b | --bits       bpp Set color depth (bits/pixel) to 'bpp' (min/max allowed)\n"
"-f | --frequency  hz  Set refresh frequency to 'hz'    (min/max allowed)\n"
"-o | --orientation  n Set display orientation to 'n' (0,90,180,270)\n"
"-x | --positionx    n Set display x position to 'x'\n"
"-y | --positiony    n Set display y position to 'y'\n"
"-e | --mode         n Set display mode to 'n'\n"
"-m | --monitor      n Set/get display settings for monitor number 'n'\n"
"-M | --monitor-name x Set/get display settings for monitor named 'x'\n"
"-i | --min            Use minimum supported value for width/height/bits/freq\n"
"-a | --max            Use maximum supported value for width/height/bits/freq\n"
"-q | --query          Show current display settings and exit\n"
"-l | --list           List available display modes\n"
"-L | --list-monitors  List the names of the monitors\n"

"-Z | --raw            List all graphics modes (CAUTION!)\n"
"-c | --current        Set/get current settings (not registry settings)\n"
"-s | --save           Settings are permanently changed for the current user\n"
"-g | --global         Settings are permanently changed for all users\n"
"-r | --reset          Settings are changed, even if they are the same\n"
"-R | --no-reset       Settings are stored, but will not take effect\n"
"-p | --primary        Selected monitor will become the primary monitor\n"
"-T | --temporary      Change is temporary in nature\n"
"-t | --test           Tests if the requested graphics mode could be set\n"

"-v | --version        Show version and copyright information and quit\n"
"-? | --help           Show this help message and quit\n"

/*
"\n"
"Examples:\n"
" %s -h 1024 -w 768 -d 32 -r 60\n"
" %s -h 1280 -w 1024       & rem depth/resolution will be minimum values\n"
" %s -max                  & rem use max resolution/depth/refresh available\n"
" %s -max -refresh min     & rem use max resolution/depth but min refresh\n"
" %s -t 1 -a -h 800 -w 600 & rem set monitor 1's resolution\n"
, progname, progname, progname, progname, progname
*/
);
}

/* per http://www.scit.wlv.ac.uk/cgi-bin/mansec?3C+basename */
static char* basename(char* s) {
	char* rv;

	if (!s || !*s)
		return ".";

	rv = s + strlen(s) - 1;

	do {
		if (*rv == '\\' || *rv == '/')
			return rv + 1;
		--rv;
	} while (rv >= s);

	return s;
}

struct _opt {
	int monitor;
	int height;
	int width;
	int depth;
	int refresh;
	int flags;
	int orientation;
	int positionx;
	int positiony;
	bool current;
	DWORD rawmode;
};

typedef struct _opt Opt;

static Opt opt = {0, 0,	0,	0,	0, 0, -1, -1, -1, false, 0};

static Opt cur;

static int monitors = 0;

static DISPLAY_DEVICE *monitor = NULL;

static char *get_monitor_name() {
	return monitor[opt.monitor].DeviceName;
}

static int get_mode_count() {
	int i = 0;
	int rv;
	DEVMODE devmode;
	ZeroMemory(&devmode, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);

	char *monitor_name = get_monitor_name();

	while (rv = EnumDisplaySettingsEx(monitor_name, i, &devmode, opt.rawmode))
		++i;

	return i;
}

static int get_orientation(DEVMODE *devmode) {
//	if ((devmode->dmFields & DM_DISPLAYORIENTATION) != DM_DISPLAYORIENTATION) {
//		return 0;
//	}
	switch (devmode->dmDisplayOrientation) {
		case DMDO_DEFAULT: 	// The display orientation is the natural orientation of the display device; it should be used as the default.
			return 0;
			break;
		case DMDO_90 : // The display orientation is rotated 90 degrees (measured clockwise) from DMDO_DEFAULT.
			return 90;
			break;
		case DMDO_180: // The display orientation is rotated 180 degrees (measured clockwise) from DMDO_DEFAULT.
			return 180;
			break;
		case DMDO_270: // The display orientation is rotated 270 degrees (measured clockwise) from DMDO_DEFAULT.
			return 270;
			break;
	}
	return 0;
}

static DWORD set_orientation(int orientation) {
	switch (orientation) {
		case 0:
			return DMDO_DEFAULT;
		case 90:
			return DMDO_90;
		case 180:
			return DMDO_180;
		case 270:
			return DMDO_270;
	}
	return -1;
}

static int get_current_settings() {
	int rv;
	DEVMODE devmode;
	ZeroMemory(&devmode, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);

	char *monitor_name = get_monitor_name();

	DWORD iModeNum = opt.current ? ENUM_CURRENT_SETTINGS : ENUM_REGISTRY_SETTINGS;

	rv = EnumDisplaySettings(monitor_name, iModeNum, &devmode);
	if (!rv)
		return rv;
	cur.width	= devmode.dmPelsWidth;
	cur.height	= devmode.dmPelsHeight,
	cur.depth	= devmode.dmBitsPerPel,
	cur.refresh	= devmode.dmDisplayFrequency;
	cur.orientation	= get_orientation(&devmode);
	cur.positionx	= devmode.dmPosition.x;
	cur.positiony	= devmode.dmPosition.y;
	return rv;
}

static int modes = 0;

static Opt *mode = NULL;

static int load_modes() {
	int i = 0;
	int rv;

	DEVMODE devmode;
	ZeroMemory(&devmode, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);

	char *monitor_name = get_monitor_name();

	modes = get_mode_count();

	mode = new Opt[modes];
	ZeroMemory(mode, modes * sizeof(Opt));

	while (rv = EnumDisplaySettingsEx(monitor_name, i, &devmode, opt.rawmode)) {
		mode[i].width	= devmode.dmPelsWidth;
		mode[i].height	= devmode.dmPelsHeight,
		mode[i].depth	= devmode.dmBitsPerPel,
		mode[i].refresh	= devmode.dmDisplayFrequency;
		mode[i].orientation	= get_orientation(&devmode);
		mode[i].positionx	= devmode.dmPosition.x;
		mode[i].positiony	= devmode.dmPosition.y;
		++i;
	}

	return i;
}

static int list_modes() {
	int i = 0;
	int rv;
	DEVMODE devmode;
	ZeroMemory(&devmode, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);

	char *monitor_name = get_monitor_name();

	printf("No.  Width Height  Depth Refresh Orientation\n");
	printf("--- ------ ------ ------ ------- -----------\n");

	while (rv = EnumDisplaySettingsEx(monitor_name, i, &devmode, opt.rawmode)) {
		printf("%3d %6d %6d %6d %7d %11d\n", i,
			devmode.dmPelsWidth,
			devmode.dmPelsHeight,
			devmode.dmBitsPerPel,
			devmode.dmDisplayFrequency,
			get_orientation(&devmode));
		++i;
	}

	return i;
}

static int get_monitor_count() {
	int i = 0;
	int rv;
	DISPLAY_DEVICE displayDevice;
	ZeroMemory(&displayDevice, sizeof(DISPLAY_DEVICE));
	displayDevice.cb = sizeof(DISPLAY_DEVICE);

	while (rv = EnumDisplayDevices(NULL, i, &displayDevice, 0))
		++i;

	return i;
}

static int load_monitors() {
	int i = 0;
	int rv;
	DISPLAY_DEVICE displayDevice;
	ZeroMemory(&displayDevice, sizeof(DISPLAY_DEVICE));
	displayDevice.cb = sizeof(DISPLAY_DEVICE);

	monitors = get_monitor_count();

	monitor = new DISPLAY_DEVICE[monitors];
	ZeroMemory(monitor, monitors * sizeof(DISPLAY_DEVICE));

	while (rv = EnumDisplayDevices(NULL, i, &displayDevice, 0)) {
		memcpy(&monitor[i], &displayDevice, sizeof(DISPLAY_DEVICE));
//		monitor[i] = new char[strlen(displayDevice.DeviceName) + 1];
//		strcpy(monitor[i], displayDevice.DeviceName);
		++i;
	}

	return i;
}

static int list_monitors() {
	int i = 0;
	int rv;
	DISPLAY_DEVICE displayDevice;
	ZeroMemory(&displayDevice, sizeof(DISPLAY_DEVICE));
	displayDevice.cb = sizeof(DISPLAY_DEVICE);

	printf("No. Monitor Name                   Attached Primary\n");
	printf("--- ------------------------------ -------- -------\n");

	while (rv = EnumDisplayDevices(NULL, i, &displayDevice, 0)) {
		printf("%3d %-30s %-7s %-8s\n", i, displayDevice.DeviceName,
			(displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) ? "Y" : "",
			(displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) ? "Y" : ""
		);
		++i;
	}

	return i;
}

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

#define	OPT_MIN (-1)
#define OPT_MAX (-2)

static void bad_mode(int i) {
	fprintf(stderr, "Mode %d is not between %d and %d\n", i, 0, modes);
	exit(2);
}

static void print_error(int rv) {
	switch (rv) {
		case DISP_CHANGE_SUCCESSFUL:
			fprintf(stderr, "Error %d: The settings change was successful.\n", DISP_CHANGE_SUCCESSFUL);
			exit(rv);

		case DISP_CHANGE_BADDUALVIEW:
			fprintf(stderr, "Error %d: The settings change was unsuccessful because the system is DualView capable.\n", DISP_CHANGE_BADDUALVIEW);
			exit(rv);

		case DISP_CHANGE_BADFLAGS:
			fprintf(stderr, "Error %d: An invalid set of flags was passed in.\n", DISP_CHANGE_BADFLAGS);
			exit(rv);

		case DISP_CHANGE_BADMODE:
			fprintf(stderr, "Error %d: The graphics mode is not supported.\n", DISP_CHANGE_BADMODE);
			exit(rv);

		case DISP_CHANGE_BADPARAM:
			fprintf(stderr, "Error %d: An invalid parameter was passed in.\n", DISP_CHANGE_BADPARAM);
			exit(rv);

		case DISP_CHANGE_FAILED:
			fprintf(stderr, "Error %d: The display driver failed the specified graphics mode.\n", DISP_CHANGE_FAILED);
			exit(rv);

		case DISP_CHANGE_NOTUPDATED:
			fprintf(stderr, "Error %d: Unable to write settings to the registry.\n", DISP_CHANGE_NOTUPDATED);
			exit(rv);

		case DISP_CHANGE_RESTART:
			fprintf(stderr, "Error %d: The computer must be restarted for the graphics mode to work.\n", DISP_CHANGE_RESTART);
			exit(rv);
	}
}

#define DISPLAY_FMT "Display %d %s %4d by %4d by %2dbpp @ %dhz (%d,%d @ %dø)"

static void set_res(int i) {
	if (i < 0 || i > modes)
		bad_mode(i);

	char *monitor_name = get_monitor_name();

	printf(DISPLAY_FMT " (mode %d)\n",
		opt.monitor,
		(opt.flags & CDS_TEST) ? "can be " : "will be",
		mode[i].width, mode[i].height, mode[i].depth, mode[i].refresh,
		opt.positionx,
		opt.positiony,
		opt.orientation,
		i);

	int rv;
	DEVMODE devmode;
	ZeroMemory(&devmode, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);

	rv = EnumDisplaySettingsEx(monitor_name, i, &devmode, opt.rawmode);
	if (!rv) {
		fprintf(stderr, "Failed to get settings for mode %d: %s\n", i, SystemMessageA(GetLastError()));
		exit(4);
	}

	devmode.dmPelsWidth			= mode[i].width;
	devmode.dmPelsHeight		= mode[i].height;
	devmode.dmBitsPerPel		= mode[i].depth;
	devmode.dmDisplayFrequency	= mode[i].refresh;

//	devmode.dmFields |= DM_BITSPERPEL;
//	devmode.dmFields |= DM_PELSWIDTH;
//	devmode.dmFields |= DM_PELSHEIGHT;
//	devmode.dmFields |= DM_DISPLAYFREQUENCY;

	if (!opt.current) {
		opt.flags = CDS_NORESET | CDS_UPDATEREGISTRY;
	}

	if (!(monitor[opt.monitor].StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
		if (devmode.dmPosition.x != opt.positionx || devmode.dmPosition.y != opt.positiony) {
			devmode.dmPosition.x = opt.positionx;
			devmode.dmPosition.y = opt.positiony;
			devmode.dmFields |= DM_POSITION;
		}
	}

	if (devmode.dmDisplayOrientation != set_orientation(opt.orientation)) {
		devmode.dmDisplayOrientation = set_orientation(opt.orientation);
		devmode.dmFields |= DM_DISPLAYORIENTATION;
	}

	rv = ChangeDisplaySettingsEx(monitor_name, &devmode, NULL, opt.flags, NULL);

	if (!rv && rv != DISP_CHANGE_SUCCESSFUL) {
		print_error(rv);
	}

	// A second call to ChangeDisplaySettings updates the monitor.
	if (!opt.current) {
		rv = ChangeDisplaySettings(NULL, 0);
		if (!rv && rv != DISP_CHANGE_SUCCESSFUL) {
			print_error(rv);
		}
	}

	exit(0);
}

int get_width(int height) {
	int rv = INT_MAX;

	for (int i = 0; i < modes; ++i) {
		if (height == mode[i].height && mode[i].width <= rv)
			rv = mode[i].width;
	}

	if (rv == INT_MAX)
		rv = OPT_MIN;

	return rv;
}

int get_height(int width) {
	int rv = INT_MAX;

	for (int i = 0; i < modes; ++i) {
		if (width == mode[i].width && mode[i].height <= rv)
			rv = mode[i].height;
	}

	if (rv == INT_MAX)
		rv = OPT_MIN;

	return rv;
}

static int set_mode(Opt opt) {
	int i;

	for (i = 0; i < modes; ++i) {
		if (opt.width	== mode[i].width &&
			opt.height	== mode[i].height &&
			opt.depth	== mode[i].depth &&
			opt.refresh	== mode[i].refresh &&
			opt.orientation == mode[i].orientation)
				set_res(i);
	}

	if (!opt.width && !opt.height) {
		opt.width = cur.width;
		opt.height = cur.height;
	}

	if (!opt.width) {
		opt.width = get_width(opt.height);
	}

	if (!opt.height) {
		opt.height = get_height(opt.width);
	}

	if (!opt.depth)
		opt.depth = OPT_MIN;

	if (!opt.refresh)
		opt.refresh = OPT_MIN;

	int wn	= opt.width		== OPT_MIN;
	int hn	= opt.height	== OPT_MIN;
	int dn	= opt.depth		== OPT_MIN;
	int rn	= opt.refresh	== OPT_MIN;

	int wp	= opt.width > 0;
	int hp	= opt.height > 0;
	int dp	= opt.depth > 0;
	int rp	= opt.refresh > 0;

	int wx	= opt.width		== OPT_MAX;
	int hx	= opt.height	== OPT_MAX;
	int dx	= opt.depth		== OPT_MAX;
	int rx	= opt.refresh	== OPT_MAX;

	// 2.	> 0		> 0		> 0		0 (min)
	int m = -1;
	int w = wx ? INT_MIN : INT_MAX;
	int h = hx ? INT_MIN : INT_MAX;
	int d = dx ? INT_MIN : INT_MAX;
	int r = rx ? INT_MIN : INT_MAX;
	for (i = 0; i < modes; ++i) {
		if (wn && mode[i].width		> w) {
//printf("wn %d && mode[%d].width	%d > w %d\n", wn, i, mode[i].width, w);
			continue;
		}
		if (wp && mode[i].width		!= opt.width) {
//printf("wp %d && mode[%d].width	%d != opt.width %d\n", wp, i, mode[i].width, opt.width);
			continue;
		}
		if (wx && mode[i].width		< w) {
//printf("wx %d && mode[%d].width	%d < w %d\n", wx, i, mode[i].width, w);
			continue;
		}
		if (hn && mode[i].height	> h) {
//printf("hn %d && mode[%d].height %d > h %d\n", hn, i, mode[i].height, h);
			continue;
		}
		if (hp && mode[i].height	!= opt.height) {
//printf("hp %d && mode[%d].height %d != opt.height %d\n", hp, i, mode[i].height, opt.height);
			continue;
		}
		if (hx && mode[i].height	< h) {
//printf("hx %d && mode[%d].height %d < h %d\n", hx, i, mode[i].height, h);
			continue;
		}
		if (dn && mode[i].depth		> d) {
//printf("dn %d && mode[%d].depth %d > d %d\n", dn, i, mode[i].depth, d);
			continue;
		}
		if (dp && mode[i].depth		!= opt.depth) {
//printf("dp %d && mode[%d].depth %d != opt.depth %d\n", dp, i, mode[i].depth, opt.depth);
			continue;
		}
		if (dx && mode[i].depth		< d) {
//printf("dx %d && mode[%d].depth %d < d %d\n", dx, i, mode[i].depth, i, d);
			continue;
		}
		if (rn && mode[i].refresh	> r) {
//printf("rn %d && mode[%d].refresh %d > r %d\n", rn, i, mode[i].refresh, r);
			continue;
		}
		if (rp && mode[i].refresh	!= opt.refresh) {
//printf("rp %d && mode[%d].refresh %d != opt.refresh %d\n", rp, i, mode[i].refresh, opt.refresh);
			continue;
		}
		if (rx && mode[i].refresh	< r) {
//printf("rx %d && mode[%d].refresh %d < r %d\n", rx, i, mode[i].refresh, i, r);
			continue;
		}

		if (opt.orientation != mode[i].orientation) {
//printf("opt.orientation != mode[i].orientation\n", opt.orientation, i, mode[i].orientation);
			continue;
		}

		m = i;
		w = mode[i].width;
		h = mode[i].height;
		d = mode[i].depth;
		r = mode[i].refresh;
	}

	set_res(m);

	return 0;
}

static int set_arg(int *i, char *optarg) {
	if (!optarg || !*optarg) {
		usage();
		exit(1);
	}

	if (_stricmp(optarg, "min") == 0 || _stricmp(optarg, "n") == 0) {
		*i = OPT_MIN;
		return *i;
	}

	if (_stricmp(optarg, "max") == 0 || _stricmp(optarg, "a") == 0) {
		*i = OPT_MAX;
		return *i;
	}

	*i = atoi(optarg);
	if (*i <= 0) {
		usage();
		exit(1);
	}

	return *i;
}

int main(int argc, char **argv) {
	int rv = 0;
	int mode_no = -1;

#ifdef _DEBUG_ALLOC
    InitAllocCheck();
#endif
	progname = basename(argv[0]);

	int len = strlen(progname);
	if (len > 4 && _stricmp(progname + len - 4, ".exe") == 0)
		progname[len - 4] = '\0';

	opterr = 0;
	optind = 1;
	optreset = 1;

	int n;
	int query = 0;

	monitors = get_monitor_count();

	load_monitors();

	modes = get_mode_count();

	while (1) {
		int c;
		int option_index = 0;

		if (optind < argc && argv[optind] && argv[optind][0] == '/')
			argv[optind][0] = '-';

		c = getopt_long(argc, argv, short_options, long_options, &option_index);

		if (opterr) {
			usage();
			exit(1);
		}

		if (c == -1)
			break;

		switch (c) {
			case 'a': // max
				set_arg(&opt.height,	"max");
				set_arg(&opt.width,		"max");
				set_arg(&opt.depth,		"max");
				set_arg(&opt.refresh,	"max");
				break;

			case 'b': // bits / bpp
			case 'd': // depth
				set_arg(&opt.depth, optarg);
				break;

			case 'c': // current
				opt.current = true;
				break;

			case 'e': // mode
				if (!optarg || !*optarg) {
					usage();
					exit(1);
				}
				mode_no = atoi(optarg);
				break;

			case 'f': // frequency
				set_arg(&opt.refresh, optarg);
				break;

			case 'h': // height
				set_arg(&opt.height, optarg);
				break;

			case 'i': // min
				set_arg(&opt.height,	"min");
				set_arg(&opt.width,		"min");
				set_arg(&opt.depth,		"min");
				set_arg(&opt.refresh,	"min");
				break;

			case 'l': // list
				list_modes();
				exit(0);
				break;

			case 'L': // list-monitors
				list_monitors();
				exit(0);
				break;

			case 'm': // monitor
				if (!optarg || !*optarg) {
					usage();
					exit(1);
				}
				n = atoi(optarg);
				if (n < 0 || n >= monitors) {
					fprintf(stderr, "Monitor %d is not between %d and %d\n", n, 0, monitors);
					exit(4);
				}

				opt.monitor = n;
				modes = get_mode_count();
				break;

			case 'M': // monitor-name
				if (!optarg || !*optarg) {
					usage();
					exit(1);
				}
				for (n = 0; n < monitors; ++n) {
					if (_stricmp(optarg, monitor[n].DeviceName) == 0) {
						opt.monitor = n;
						break;
					}
				}
				break;

			case 'o': // orientation
				if (!optarg || !*optarg) {
					usage();
					exit(1);
				}
				n = atoi(optarg);
				if (set_orientation(n) < 0) {
					usage();
					exit(1);
				}
				opt.orientation = n;
				break;

			case 'q': // query
				query = 1;
				break;

			case 'x': // positionx
				if (!optarg || !*optarg) {
					usage();
					exit(1);
				}
				opt.positionx = atoi(optarg);
				break;

			case 'y': // positiony
				if (!optarg || !*optarg) {
					usage();
					exit(1);
				}
				opt.positiony = atoi(optarg);
				break;

			case 'Z': // rawmode
				opt.rawmode = EDS_RAWMODE;
				modes = get_mode_count();
				break;

			case 'g': // global
				opt.flags = CDS_GLOBAL | CDS_UPDATEREGISTRY;
				break;
			case 'p': // primary
				opt.flags = CDS_SET_PRIMARY;
				break;
			case 'r': // reset
				opt.flags = CDS_RESET;
				break;
			case 'R': // no-reset
				opt.flags = CDS_NORESET | CDS_UPDATEREGISTRY;
				break;
			case 's': // save
				opt.flags = CDS_UPDATEREGISTRY;
				break;
			case 't': // test
				opt.flags = CDS_TEST;
				break;
			case 'T': // temporary
				opt.flags = CDS_FULLSCREEN;
				break;

			case 'v': // version
				printf("%s - Version %s - %s\n", APPNAME, APPVERSION, __DATE__);
				printf(APPCOPYRIGHT "\n\n");

				exit(0);
				break;

			case 'w': // width
				set_arg(&opt.width, optarg);
				break;

			case '?': // help
				usage();
				exit(0);
				break;

			case ':':
				fprintf(stderr, "Option -%c requires an operand\n", optopt);
				// fallthrough

			default:
				usage();
				exit(1);
		}
	}

	while (true) {
		if (optind < argc && argv[optind]) {
//			if (atoi(argv[optind]) == 0) {
				n = sizeof(resolutions) / sizeof(resolutions[0]);
				for (int i = 0; i < n; ++i) {
					if (resolutions[i].name != "" && _stricmp(argv[optind], resolutions[i].name) == 0) {
						opt.width = resolutions[i].width;
						opt.height = resolutions[i].height;
						break;
					}
				}
				++optind;
				continue;
//			}

			set_arg(&opt.width, argv[optind++]);
		}

		if (optind < argc && argv[optind]) {
			set_arg(&opt.height, argv[optind++]);
		}

		if (optind < argc && argv[optind]) {
			set_arg(&opt.depth, argv[optind++]);
		}

		if (optind < argc && argv[optind]) {
			set_arg(&opt.refresh, argv[optind++]);
		}

		break;
	}

	get_current_settings();

	printf(DISPLAY_FMT "\n",
		opt.monitor,
		"is now ",
		cur.width, cur.height, cur.depth, cur.refresh, cur.positionx, cur.positiony, cur.orientation);

	while (true) {
		if (query == 1)
			break;

		load_modes();

		if (opt.width == 0 && opt.height == 0 && opt.depth == 0 && opt.refresh == 0) {
			if (opt.positionx != -1 || opt.positiony != -1 || opt.orientation != -1) {
				opt.width = cur.width;
				opt.height = cur.height;
				opt.depth = cur.depth;
				opt.refresh = cur.refresh;
			}
		}

		if (opt.positionx == -1) {
			opt.positionx = cur.positionx;
		}

		if (opt.positiony == -1) {
			opt.positiony = cur.positiony;
		}

		if (opt.orientation == -1) {
			opt.orientation = cur.orientation;
		}

		if (mode_no != -1 && (mode_no < 0 || mode_no >= modes)) {
			fprintf(stderr, "mode number %d is not between %d and %d\n", mode_no, 0, modes);
			rv = 3;
			break;
		}

		if (mode_no != -1)
			set_res(mode_no);

		if (opt.width == 0 && opt.height == 0 && opt.depth == 0 && opt.refresh == 0) {
			if (!query)
				//usage();
			break;
		}

		set_mode(opt);
		break;
	}

	delete [] mode;
	delete [] monitor;

    return rv;
}
