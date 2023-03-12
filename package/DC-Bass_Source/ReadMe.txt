======================================================================
         DC-Bass Source - DirectShow Audio Decoder Filter

         Version 1.3.0

         (C) 2003-2010 Milenko Mitrovic
         mail: dcoder@dsp-worx.de 
         web : http://www.dsp-worx.de
======================================================================


ABOUT
----------------------------------------------------------------------
The DC-Bass Source Filter is a BASS based DirectShow Audio Decoder.
 
Currently supported Formats are:

Audio: 
 AAC, ALAC, APE, FLAC, MP3, MPC, TTA, WV

Shoutcast:
 MP3, AAC, OGG

Tracker:
 MO3, IT, XM, S3M, MTM, MOD, UMX

Shoutcast Streams can be saved from within the PropertyPage or by using
the IDCBassSource Interface.

Shoutcast Tags can be read using the IAMMediaContant or IDCBassSource
Interface.


INSTALL
----------------------------------------------------------------------
 run register.bat or use the installer executable.


UNINSTALL
----------------------------------------------------------------------
 run unregister.bat or, if you have used the installer, uninstall it
 from the Windows Add/Remove Software Page.


LICENSE
----------------------------------------------------------------------
 This software is released together with the Source Code under the 
 GNU GENERAL PUBLIC LICENSE VERSION 2

 For more Information about this License read the attached 
 "License.txt" File or enter the following Link into your Web Browser:

 http://www.gnu.org/copyleft/gpl.html


LINKS
----------------------------------------------------------------------

 BASS decoding Library
 http://www.un4seen.com/

 Extended Tag support for Windows Media Player
 http://wmptagext.sourceforge.net/


CHANGES
----------------------------------------------------------------------

v1.3.0
-fixed: updated to BASS and plugins version 2.4.6
-added: support for OptimFROG
-added: support for OGG Vorbis
-fixed: plugins can be loaded from unicode directories
-added: support for unicode URL's

v1.2.1
-fixed: updated to BASS and plugins version 2.4.2

v1.2.0
-fixed: updated to BASS and plugins version 2.4.1
-fixed: Unicode filenames were not working

v1.1.1
-fixed: debug log files will not be created anymore

v1.1.0
-added: tag detection for ICY/HTTP headers
-fixed: wrong Memoryallocation for Metadata
-added: option for Shoutcast buffersize
-added: tag based stream writing
-added: only extension with available plugins will be registered
-added: support for ICYX:// protocol
-fixed: no connect to waveout renderer with 8 channel flac
-added: ini file to choose what extensions to register

v1.0.0
-initial Release
