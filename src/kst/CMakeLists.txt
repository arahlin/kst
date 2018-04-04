kst_init(${kst_binary_name} "")

kst_files_ignore(main-vld)

kst_files_find(src/kst)

if(WIN32)
	if(MINGW)
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/kst_icon.o
						   COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_BINARY_DIR}
						   -i${kst_dir}/src/kst/kst_icon.rc
						   -o ${CMAKE_CURRENT_BINARY_DIR}/kst_icon.o)
		set(rc_file ${CMAKE_CURRENT_BINARY_DIR}/kst_icon.o)
	else()
		set(rc_file ${kst_dir}/src/kst/kst_icon.rc)
	endif()
else()
	if(APPLE)
		set(MACOSX_BUNDLE_INFO_STRING ${kst_version})
		set(MACOSX_BUNDLE_ICON_FILE kst.icns)
		set(rc_file ${kst_dir}/src/images/kst.icns)
		set_source_files_properties(${rc_file} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	endif()
endif()

kst_add_resources(src/images/icons.qrc)

kst_find_info_files(KstCMakeFiles ${kst_dir}/cmake/*)
kst_find_info_files(KstCMakeFiles ${kst_dir}/cmake/modules/*)

kst_add_info_files(KstInfoFiles
	${kst_dir}/INSTALL
	${kst_dir}/NEWS
	${kst_dir}/README)

kst_include_directories(app core)

kst_add_executable(${kst_win32} ${rc_file})

kst_link(${libcore} ${libmath} ${libapp} ${libwidgets})

kst_install_executable()

if(UNIX AND NOT APPLE)
	set(man ${CMAKE_BINARY_DIR}/${kst_binary_name}.1)
	configure_file(${kst_dir}/src/kst/kst.1.txt ${man})
	install(CODE "execute_process(COMMAND gzip ${man})")
	set_source_files_properties(${man}.gz PROPERTIES GENERATED TRUE)
	install(FILES ${man}.gz DESTINATION share/man/man1)

	set(k ${kst_dir}/src/kst)
#	install(FILES ${k}/kstplugin.desktop
#	              ${k}/kstdatasourceplugin.desktop
#	              ${k}/kstdataobjectplugin.desktop
#	              ${k}/kstbasicplugin.desktop
#	              ${k}/kstfilter.desktop
#	              ${k}/kstextension.desktop
#	                                 DESTINATION share/servicetypes/kst)
	install(FILES ${k}/x-kst.xml      DESTINATION share/mime/packages)
	install(FILES ${k}/kst2.desktop   DESTINATION share/applications)

#       icons
		install(FILES ${kst_dir}/src/images/32x32/kst.png DESTINATION share/icons/hicolor/32x32/apps)
		install(FILES ${kst_dir}/src/images/48x48/kst.png DESTINATION share/icons/hicolor/48x48/apps)
		install(FILES ${kst_dir}/src/images/64x64/kst.png DESTINATION share/icons/hicolor/64x64/apps)
		install(FILES ${kst_dir}/src/images/128x128/kst.png DESTINATION share/icons/hicolor/128x128/apps)
		install(FILES ${kst_dir}/src/images/svg_icons/kst.svg DESTINATION share/icons/hicolor/scalable/apps)

		install(FILES ${kst_dir}/src/images/32x32/application-x-kst.png DESTINATION share/icons/hicolor/32x32/mimetypes)
		install(FILES ${kst_dir}/src/images/48x48/application-x-kst.png DESTINATION share/icons/hicolor/48x48/mimetypes)
		install(FILES ${kst_dir}/src/images/64x64/application-x-kst.png DESTINATION share/icons/hicolor/64x64/mimetypes)
		install(FILES ${kst_dir}/src/images/128x128/application-x-kst.png DESTINATION share/icons/hicolor/128x128/mimetypes)
		install(FILES ${kst_dir}/src/images/svg_icons/application-x-kst.svg DESTINATION share/icons/hicolor/scalable/mimetypes)

endif()


if(UNIX AND kst_deploy)
	set(app "${CMAKE_INSTALL_PREFIX}/bin/${kst_binary_name}")
	set(dirs ${QT_LIBRARY_DIR} ${kst_build_dir}/lib)
	install(CODE "
		include(DeployQt4)
		fixup_qt4_executable(${app} \"\" \"\" \"${dirs}\")"
		COMPONENT RUNTIME)

	set(kst_renamed kst${kst_version})
	set(start_script ${CMAKE_BINARY_DIR}/kst2_start.sh)
	configure_file(${kst_dir}/src/kst/kst2.sh.cmake ${start_script} @ONLY)
	file(WRITE ${CMAKE_BINARY_DIR}/delpoy.sh "#!/bin/bash
	mv ${CMAKE_INSTALL_PREFIX}/bin/*.so* ${CMAKE_INSTALL_PREFIX}/lib
	mv ${CMAKE_INSTALL_PREFIX}/bin/${kst_binary_name} ${CMAKE_INSTALL_PREFIX}/bin/${kst_renamed}
	cp ${start_script} ${CMAKE_INSTALL_PREFIX}/bin/${kst_binary_name}
	")
	install(CODE "
		execute_process(COMMAND bash ${CMAKE_BINARY_DIR}/delpoy.sh) "
		COMPONENT RUNTIME)
endif()


if(APPLE)

	# We start from build/bin/kst2.app, all is build into the final bundle folder
	# On the mac the paths to needed shared libraries are hardcoded in the binary/library
	# which could be changed by the macro 'fixup_bundle'
	# libs which are hardcoded are resolved automatically
	# but the plugins must be passed as extra argument because they are not hardcoded in the binary

	set(kst_app ${kst_binary_name}.app)
	set(plugins_path "plugins")
	set(plugins_dir ${kst_app}/Contents/${plugins_path})
	set(conf_dir ${kst_app}/Contents/Resources)

	install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats"
		DESTINATION ${plugins_dir} COMPONENT Runtime)
	if(kst_qt5)
		install(DIRECTORY "${QT_PLUGINS_DIR}/platforms"
			DESTINATION ${plugins_dir} COMPONENT Runtime)
	endif()

	set(qtconf "\${CMAKE_INSTALL_PREFIX}/${conf_dir}/qt.conf")
	INSTALL(CODE "
		file(WRITE \"${qtconf}\" \"[Paths]\\nPlugins = ${plugins_path}\\n\")
		" COMPONENT Runtime)

	set(plugins_install_dir "\${CMAKE_INSTALL_PREFIX}/${plugins_dir}")
	install(CODE "
		file(GLOB_RECURSE qt_plugins \"${plugins_install_dir}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		file(GLOB kst_plugins \"${plugins_install_dir}/*${CMAKE_SHARED_MODULE_SUFFIX}\")
		set(libs \${qt_plugins} \${kst_plugins})
		set(dirs ${kst_build_dir}/lib)
		set(BU_CHMOD_BUNDLE_ITEMS 1)
		include(BundleUtilities)
		fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${kst_app}\" \"\${libs}\" \"\${dirs}\") "
		COMPONENT Runtime
	)

endif()
