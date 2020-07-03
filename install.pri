#
#	mda tester installer definitions.
#

### START of INSTALL section
!build_pass {
	
	#
	# Location where program will be installed
	#
	unix: {
		APP_BIN_PREFIX = /usr/bin
		APP_LIB_PREFIX = /usr/lib
		APP_INSTALL_PREFIX = /usr/share/qcrop
	}
	
	win32: {
		APP_BIN_PREFIX = ./qcrop-$${VERSION}
		APP_LIB_PREFIX = ./qcrop-$${VERSION}
		APP_INSTALL_PREFIX = ./qcrop-$${VERSION}
	}
	#
	# Necessary files
	#
	unix:target.files += ./qcrop
	win32:{target.files += ./qcrop.exe}
	target.path = $${APP_BIN_PREFIX}
	
	unix:libr.files += libqcrop.*
	win32:{libr.files += *.dll}
	libr.path = $${APP_LIB_PREFIX}
	
	unix:plugins.files += plugins/*
	win32:{plugins.files += plugins/*.dll}
	
	plugins.path = $${APP_INSTALL_PREFIX}/plugins
	
	
	INSTALLS += target libr plugins
	
	#
	# Miscanellous files
	#
	
	styles.files += qss/*
	styles.path = $${APP_INSTALL_PREFIX}/qss

	lang.files += lang/*.qm
	lang.path = $${APP_INSTALL_PREFIX}/lang

	sgn.files += qcrop.sgn
	sgn.path = $${APP_INSTALL_PREFIX}
	
	INSTALLS += styles \
		lang \
		sgn
	unix: {
		#
		# This copies the .desktop file (equivalent of Window$' .lnk) to :
		#	1 ) KDE menu
		#	2 ) Gnome menu
		#	3 ) User desktop [disabled : too invasive...]
		#
		# Looks like KDE is so smart that he finds both of them ;-)
		# But do not understand that they represent the same app...
		#
		kde.files = $$DESKTOP
		kde.path = /usr/share/applications/kde
		INSTALLS += kde
		
		DESKTOP = qcrop.desktop
		
		gnome.files = $$DESKTOP
		gnome.path = /usr/share/applications
		
		INSTALLS += gnome
		
		user.files = $$DESKTOP
		user.path = ~/Desktop
		
		#INSTALLS += user
		
		#
		# This puts the icons into "standard" location for both KDE and Gnome
		#
		# Bonus hint : it's automatic!!! just add an icon size or a desktop to
		# the variables below and it will work (provided that you create folder
		# /path/to/edyuk/icons/$$size, put an edyuk.png icon in it and that the
		# wonderful new desktop you use stores its icons in :
		# /usr/share/icons/$$desktop/$$size/apps)
		#
		
		DESK = default.kde default.kde4 gnome
		
		ICON = 16x16 \
			22x22 \
			32x32 \
			48x48 \
			64x64 \
			128x128
		
		for(d, DESK) {
			for(i, ICON) {
				eval ( icons_$${d}_$${i}.files += images/install/$${i}/* )
				eval ( icons_$${d}_$${i}.path += /usr/share/icons/$${d}/$${i}/apps )
				INSTALLS += icons_$${d}_$${i}
			}
		}
		
		#
		# Places static files to state IS terminal's MIME abilities
		#
		
		db.files += x-qs.desktop
		db.path = /usr/share/mimelnk/text
		
		mimes.files += qcrop.xml
		mimes.path = /usr/share/mime/packages
		
		INSTALLS += db mimes
		
		#
		#	SDK : qcrop plugin development kit
		#
		sdk.path = $$[QT_INSTALL_HEADERS]/qcrop
		SUB = lib/updater lib/qsettingsmodel lib/qchart lib/qextserialport lib/qcodeedit2 lib/qcodemodel2 include
		
		for(s,SUB) {
		    eval ( sdk.files *= $${s}/*.h )
		}
		
		#INSTALLS += sdk
	}
### END of INSTALL section
}
