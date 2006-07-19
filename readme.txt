res_set - Version 1.4 - Jul 19 2006
Copyright (c) 2005-2006 Ross Smith II (http://smithii.com) All Rights Reserved

------------------------------------------------------------------------------

Usage: res_set [options] [resolution]
Options:
-w | --width      x   Set horizontal resolution to 'x' (min/max allowed)
-h | --height     y   Set vertical resolution to 'y'   (min/max allowed)
-b | --bits       bpp Set color depth (bits/pixel) to 'bpp' (min/max allowed)
-f | --frequency  hz  Set refresh frequency to 'hz'    (min/max allowed)
-o | --orientation  n Set display orientation to 'n' (0,90,180,270)
-x | --positionx    n Set display x position to 'x'
-y | --positiony    n Set display y position to 'y'
-e | --mode         n Set display mode to 'n'
-m | --monitor      n Set/get display settings for monitor number 'n'
-M | --monitor-name x Set/get display settings for monitor named 'x'
-i | --min            Use minimum supported value for width/height/bits/freq
-a | --max            Use maximum supported value for width/height/bits/freq
-q | --query          Show current display settings and exit
-l | --list           List available display modes
-L | --list-monitors  List the names of the monitors
-Z | --raw            List all graphics modes (CAUTION!)
                      Even those not supported by the current monitor
-c | --current        Set/get current settings (not registry settings)
-s | --save           Settings are permanently changed for the current user
-g | --global         Settings are permanently changed for all users
-r | --reset          Settings are changed, even if they are the same
-R | --no-reset       Settings are stored, but will not take effect
-p | --primary        Selected monitor will become the primary monitor
-T | --temporary      Change is temporary in nature
-t | --test           Tests if the requested graphics mode could be set
-v | --version        Show version and copyright information and quit
-? | --help           Show this help message and quit

Resolution can be entered as 'width height', or one of the following terms:

	WUXGA  (1920x1200)
	SUXGA  (1800x1440} (Sorta Ultra XGA :)
	WSXGA+ (1680x1050)
	UXGA   (1600x1200)
	WSXGA  (1600x1024)
	SXGA+  (1400x1050)
	SXGA   (1280x1024)
	WXGA   (1280x720)
	XGA    (1024x768)
	SVGA   (800x600)
	VGA    (640x480)
	EGA    (640x400)
	CGA    (320x200)

Not all of the above modes may be available.

Examples:
 res_set -l 21                 & rem list the display modes
 res_set -e 21                 & rem set display to mode 21
 res_set xga                   & rem set resolution to 1024 x 768, depth/refresh to minimum for this resolution
 res_set -max xga              & rem set resolution to 1024 x 768, depth/refresh to maximum for this resolution
 res_set -w 800 -h 600         & rem set resolution to 800x600, depth/refresh to minimum for this resolution
 res_set 800 600               & rem same as above
 res_set -w 800 -h 600 -d 32 -r 60
 res_set -max                  & rem use highest supported resolution/depth/refresh values available
 res_set -max -f min           & rem use highest supported resolution/depth but the lowest refresh rate
 res_set -m 2 800 600          & rem set monitor 2's resolution to 800x600
 res_set -m 2 -x 200 -y -600 800 600 & rem set monitor 2's resolution to 800x600, and position to 200,-600

Also
--refresh alias for --frequency
--freq    alias for --frequency
--bpp     alias for --bits
--depth   alias for --bits
-d        alias for --bits

You can use / instead of - to preface single letter options, //width will *not* work.

------------------------------------------------------------------------------

res_set will only allow you to set parameters to those that are reported by
Windows to be supported by your video card (or you use the --raw option).

Unfortunately, your monitor may not be able to support all of these modes.

Therefore, you should test each mode manually, before using res_set.

Failure to do so, may permanently damage your monitor, computer, and/or mind.

To avoid this, res_set will always select the mininum values for the color
depth and refresh rate, if you do not specify them.

If you do not specify the resolution, res_set will default to the current
resolution for the current monitor, or the monitor you have specified.

If you only specify the width/height, res_set will use the minimum value 
for the height/width supposed for the width/height you used.

------------------------------------------------------------------------------

Sources: http://www.vaughns-1-pagers.com/computer/video-resolution.htm
http://www1.us.dell.com/content/topics/global.aspx/vectors/en/2002_lcd?c=us&l=en&s=corp
http://www.videotechnology.com/0904/formats.html
http://www.berghell.com/whitepapers/Display%20Resolutions%20and%20Nomenclature%20-%20Display%20Sizes%20in%20Pixels%20-%20%20Horizontal%20by%20Vertical.pdf

------------------------------------------------------------------------------

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

$Id$
