## 1.10 (unreleased)

  * Update Makefile
  
## 1.9 (28-Mar-2015)

  * Fix segfault due to linker issue (#1)

## 1.8 (27-Mar-2015)

  * Executables signed with StartSSL code signing certificate
  * Release zip files are gpg signed and sha256 hashed
  * Standardize build scripts
  * Update documentation
  
## 1.7 (22-Feb-2015)

  * Improve command line option processing
  * Updated to Visual Studio 2013
  * Switch to MIT license

## 1.6 (22-Oct-2011)

  * Added additional resolutions per http://en.wikipedia.org/wiki/List_of_common_resolutions
  * Added shared lib to source zip file (unzip into lsdsk/../shared)
  * Updated to Visual Studio 2010 Express

## 1.5 (03-Mar-2007)

  * Added additional resolutions per http://en.wikipedia.org/wiki/List_of_common_resolutions
  * Removed SUXGA (1800x1440} as it doesn't seem to be used anywhere

## 1.4 (12-Jul-2006)

  * Standardized version information across all programs

## 1.3 (22-May-2006)

  * Added -Z | --raw           List all graphics modes (CAUTION!)
  * Fixed command line parsing issue
  * Updated documentation

## 1.2 (21-May-2006)

  * Added -o | --orientation n  Set display orientation to 'n' (0,90,180,270)
  * Added -x | --positionx   n  Set display x position to 'x'
  * Added -y | --positiony   n  Set display y position to 'y'
  * Added -c | --current        Set/get current settings (not registry settings)
  * Changed -d | --depth         to -b | --bits (--depth still works)
  * Changed -E | --no-reset      to -R | --no-reset
  * Changed -e | --reset         to -r | --reset
  * Changed -i | --list-monitors to -L | --list-monitors
  * Changed -m | --monitor       to -M | --monitor-name
  * Changed -n | --min           to -i | --min
  * Changed -o | --mode          to -e | --mode
  * Changed -r | --refresh       to -f | --frequency (--refresh still works)
  * Changed -u | --full-screen   to -T | --temporary (--full-screen still works)
  * Changed -z | --monitor-no    to -m | --monitor (--monitor-no still works)

  * Added ability to specify resolution using one of the following abbreviations:
  ````
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
    ````
    
## 1.1 (08-Apr-2006)

  * Added -s | --save          Settings will be permanently changed for the current user
  * Added -g | --global        Settings will be permanently changed for all users
  * Added -u | --full-screen   Change is temporary in nature
  * Added -e | --reset         Settings will be changed, even if they are the same
  * Added -E | --no-reset      Settings will be stored, but will not take effect
  * Added -p | --primary       Device will become the primary device
  * Added -t | --test          Tests if the requested graphics mode could be set
  ````
	These options set each of the following ChangeDisplaySettingsEx() flag options:

	CDS_UPDATEREGISTRY The graphics mode for the current screen will be changed dynamically and the graphics mode will be updated in the registry.
	   The mode information is stored in the USER profile.
	CDS_GLOBAL 	       The settings will be saved in the global settings area so that they will affect all users on the machine. 
	   Otherwise, only the settings for the user are modified. 
	   This flag is only valid when specified with the CDS_UPDATEREGISTRY flag.
	CDS_FULLSCREEN 	   The mode is temporary in nature. 
	   Windows NT/2000/XP: If you change to and from another desktop, this mode will not be reset.
	CDS_NORESET        The settings will be saved in the registry, but will not take effect.
	   This flag is only valid when specified with the CDS_UPDATEREGISTRY flag.
	CDS_RESET 	       The settings should be changed, even if the requested settings are the same as the current settings.
	CDS_SET_PRIMARY    This device will become the primary device.
	CDS_TEST 	       The system tests if the requested graphics mode could be set.
  ````
  
  * Added .rc and .ico files
  * Improved build scripts
  * Updated documentation

## 1.0 (06-Feb-2006)

  * Initial release
