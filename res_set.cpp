/*

$Id$

Copyright (c) 2005-2006 Ross Smith II (http://smithii.com). All rights reserved.

This program is free software; you can redistribute it and/or modify it
under the terms of version 2 of the GNU General Public License
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

*/

/*
to do:
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/devcons_3tfc.asp

-? | --prompt	Prompt for user to press a key, then wait for user to press y or Y
                if y/Y not pressed in 15 seconds, reverts back to original mode
-s | --safe     Only change to modes that have already been used previously
                Modes are stored in the registry
*/

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif

#define _WIN32_WINNT 0x501

#include <windows.h>
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
	char *name;
	int width;
	int height;
};

typedef struct _resolution Resolution;

static Resolution resolutions[] = {
	{"WUXGA",	1920,   1200},
	{"SUXGA",	1800,   1440}, /* Sorta Ultra XGA */
	{"WSXGA+",	1680,   1050},
	{"UXGA",	1600,   1200},
	{"WSXGA",	1600,   1024},
	{"SXGA+",	1400,   1050},
	{"SXGA",	1280,   1024},
	{"WXGA",	1280 ,   720},
	{"XGA",		1024 ,   768},
	{"SVGA",	800 ,   600},
	{"VGA",		640 ,   480},
	{"EGA",		640 ,   400},
	{"CGA",		320 ,   200}
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

static get_current_settings() {
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
					if (stricmp(optarg, monitor[n].DeviceName) == 0) {
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

				printf(
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n");

				exit(0);
				break;

			case 'w': // width
				set_arg(&opt.width, optarg);
				break;

			case '?': // help
				usage();
				exit(0);
				break;

			default:
				usage();
				exit(1);
		}
	}

	while (true) {
		if (optind < argc && argv[optind]) {
			if (atoi(argv[optind]) == 0) {
				n = sizeof(resolutions) / sizeof(resolutions[0]);
				for (int i = 0; i < n; ++i) {
					if (_stricmp(argv[optind], resolutions[i].name) == 0) {
						opt.width = resolutions[i].width;
						opt.height = resolutions[i].height;
						break;
					}
				}
				++optind;
				continue;
			}

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
				usage();
			break;
		}

		set_mode(opt);
		break;
	}

	delete [] mode;
	delete [] monitor;

    return rv;
}
