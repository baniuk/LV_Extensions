# LV_Extension install script

# name of the installer
OutFile "LV_Extensions_ver$%vervar%.exe"

# define installation directory
InstallDir $PROGRAMFILES\LVExtensions

# For removing Start Menu shortcut in Windows 7
RequestExecutionLevel user

DirText "This will install LVExtensions on your computer. Choose a directory"
 
SetCompressor /SOLID lzma

Section
	
	# set the installation directory as the destination for the following actions
    SetOutPath $INSTDIR
 
    # create the uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"
 
	CreateDirectory "$SMPROGRAMS\LVExtensions"
    # create a shortcut named "new shortcut" in the start menu programs directory
    # point the new shortcut at the program uninstaller
    CreateShortCut "$SMPROGRAMS\LVExtensions\Uninstall.lnk" "$INSTDIR\uninstall.exe"
	
	# Sets the location where the files are copied. All file copy statements after this line will be copied to this location, until it is changed
	SetOutPath $INSTDIR
	File *.dll
	File Changes.txt
	# dependencies
	SetOutPath $WINDIR
	File *.dll
	
	
	CreateShortCut "$SMPROGRAMS\LVExtensions\Changes.lnk" "$INSTDIR\Changes.txt"
SectionEnd

# uninstaller section start
Section "uninstall"
 
    # first, delete the uninstaller
    Delete "$INSTDIR\uninstall.exe"
	# delete all files in install dir
	RMDir /r $INSTDIR
    # second, remove the link from the start menu
    Delete "$SMPROGRAMS\LVExtensions\Uninstall.lnk"
	Delete "$SMPROGRAMS\LVExtensions\Changes.lnk"
	# remove startmenu
	RMDir /r "$SMPROGRAMS\LVExtensions"
	# remove libtif
	Delete "$WINDIR\zlib1.dll"
	Delete "$WINDIR\libpng13.dll"
	Delete "$WINDIR\libimage.dll"
	Delete "$WINDIR\jpeg62.dll"
	Delete "$WINDIR\glut32.dll"
	Delete "$WINDIR\libtiff3.dll"
	Delete "$INSTDIR\Changes.txt"
 
# uninstaller section end
SectionEnd