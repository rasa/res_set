# res_set [![Flattr this][flatter_png]][flatter]

Display or change the monitor resolution, color depth, or refresh rate.

## Usage

````
res_set [options] [resolution]

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
````

## Examples

````batch
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
````

Also
````
--refresh alias for --frequency
--freq    alias for --frequency
--bpp     alias for --bits
--depth   alias for --bits
-d        alias for --bits
````

You can use / instead of - to preface single letter options, //width will *not* work.

Resolution can be entered as 'width height', or one of the following terms:

````
       Term      W    H Description
------ ---- ---- -------------------------
CGA     320  200 CGA 4-color, Atari ST 16 color, Commodore 64 VIC-II Hires, Amiga OCS NTSC Lowres, Apple IIGS LoRes, MCGA
QVGA    320  240 QVGA, Mega Drive, Nintendo 3DS (lower screen)
WQVGA   432  240 WQVGA
HVGA    640  240 HVGA, Handheld PC
EGA     640  350 EGA
MDA     720  350 MDA
D1      720  480 D1
D1P     720  576 D1P
VGA     640  480 VGA, MCGA (in monochome), Sun-1 color
WVGA    768  480 WVGA
WGA     800  480 WVGA or WGA, List of mobile phones with WVGA display, Nokia 770/800N/N810/N900, Asus Eee PC 700 and 701 series, Pandora
FWVGA   854  480 FWVGA, List of mobile phones with FWVGA display
SVGA    800  600 SVGA
QHD     960  540 QHD[3], Quarter FHD (AACS ICT), HRHD, Motorola Atrix 4G
WSVGA  1024  576 WSVGA
DVGA    960  640 DVGA, Apple iPhone 4[5][6],4th Generation iPod Touch[7]
XGA    1024  768 XGA, Apple iPad
720p   1280  720 720p (WXGA-H, min.)
WXGA-H 1280  720 720p (WXGA-H, min.)
WXGA-  1280  768 WXGA, avg., BrightView
XGA+   1152  864 XGA+[8]
WXGA   1280  800 WXGA, max.
720p+  1366  768 standardized HDTV 720p/1080i displays, HD
HD     1366  768 standardized HDTV 720p/1080i displays, HD
SXGA-  1280  960 SXGA-
WSXGA  1440  900 WSXGA, WXGA+
WXGA+  1440  900 WSXGA, WXGA+
SXGA   1280 1024 SXGA
900p   1600  900 900p, HD+
HD+    1600  900 900p, HD+
SXGA+  1400 1050 SXGA+
WSXGA+ 1680 1050 WSXGA+
UXGA   1600 1200 UXGA
1080i  1920 1080 HD 1080 (1080i, 1080p), FullHD
1080p  1920 1080 HD 1080 (1080i, 1080p), FullHD
WUXGA  1920 1200 WUXGA
QWXGA  2048 1152 QWXGA, 2K
2K     2048 1152 QWXGA, 2K
TXGA   1920 1400 TXGA
QXGA   2048 1536 QXGA
WQHD   2560 1440 WQHD (Dell UltraSharp U2711, Apple iMac)
WQXGA  2560 1600 WQXGA
QSXGA  2560 2048 QSXGA
QSXGA+ 2800 2100 QSXGA+
WQSXGA 3200 2048 WQSXGA
QUXGA  3200 2400 QUXGA
QFHD   3840 2160 QFHD
WQUXGA 3840 2400 WQUXGA
4K     4096 2304 4K
HXGA   4096 3072 HXGA
WHXGA  5120 3200 WHXGA
HSXGA  5120 4096 HSXGA
WHSXGA 6400 4096 WHSXGA
HUXGA  6400 4800 HUXGA
UHDTV  7680 4320 UHDTV
8K     8192 4320 8K
WHUXGA 7680 4800 WHUXGA
````

Not all of the above modes will be available.

    Source:  http://en.wikipedia.org/wiki/List_of_common_resolutions

## Notes

res_set will only allow you to set parameters to those that are reported by
Windows to be supported by your video card (unless you use the --raw option).

Unfortunately, your monitor may not be able to support all of these modes.

Therefore, you should test each mode manually, before using res_set.

Failure to do so, may permanently damage your monitor, computer, and/or mind.

To avoid this, res_set will always select the mininum values for the color
depth and refresh rate, if you do not specify them.

If you do not specify the resolution, res_set will default to the current
resolution for the current monitor, or the monitor you have specified.

If you only specify the width/height, res_set will use the minimum value
for the height/width supposed for the width/height you used.

## Sources

* http://www.vaughns-1-pagers.com/computer/video-resolution.htm
* http://www1.us.dell.com/content/topics/global.aspx/vectors/en/2002_lcd?c=us&l=en&s=corp
* http://www.videotechnology.com/0904/formats.html
* http://www.berghell.com/whitepapers/Display%20Resolutions%20and%20Nomenclature%20-%20Display%20Sizes%20in%20Pixels%20-%20%20Horizontal%20by%20Vertical.pdf

## Verify a Release

To verify a release, download the .zip, .sha256, and .asc files for the release 
(replacing res_set-1.8-win32.zip with the release you are verifying):

````
$ wget https://github.com/rasa/res_set/releases/download/v1.8/res_set-1.8-win32.zip{,.sha256,.asc}
````

Next, check that sha256sum reports "OK":
````
$ sha256sum -c res_set-1.8-win32.zip.sha256
res_set-1.8-win32.zip: OK
````

Lastly, check that GPG reports "Good signature":

````
$ gpg --keyserver hkps.pool.sks-keyservers.net --recv-key 0x105a5225b6ab4b22
$ gpg --verify res_set-1.8-win32.zip.asc res_set-1.8-win32.zip
gpg:                using RSA key 0xFF914F74B4BB6EF3
gpg: Good signature from "Ross Smith II <ross@smithii.com>" [ultimate]
...
````

## Contributing

To contribute to this project, please see [CONTRIBUTING.md](CONTRIBUTING.md).

## Bugs

To view existing bugs, or report a new bug, please see [issues](../../issues).

## Changelog

To view the version history for this project, please see [CHANGELOG.md](CHANGELOG.md).

## License

This project is [MIT licensed](LICENSE).

## Contact

This project was created and is maintained by [Ross Smith II][] [![endorse][endorse_png]][endorse]

Feedback, suggestions, and enhancements are welcome.

[Ross Smith II]: mailto:ross@smithii.com "ross@smithii.com"
[flatter]: https://flattr.com/submit/auto?user_id=rasa&url=https%3A%2F%2Fgithub.com%2Frasa%2Fres_set
[flatter_png]: http://button.flattr.com/flattr-badge-large.png "Flattr this"
[endorse]: https://coderwall.com/rasa
[endorse_png]: https://api.coderwall.com/rasa/endorsecount.png "endorse"

