include (FindPkgConfig)
if (GTK3)
pkg_check_modules (DEPLIBS REQUIRED gtk+-3.0)
else (GTK3)
pkg_check_modules (DEPLIBS REQUIRED gtk+-3.0)
endif (GTK3)

set (LOCALE_DIR ${CMAKE_INSTALL_PREFIX}/share/locale)
set (UI_FILE ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/${PROJECT_NAME}.ui)
set (APP_ICON ${CMAKE_INSTALL_PREFIX}/share/pixmaps/${PROJECT_NAME}.png)
configure_file (config.h.cmake config.h)
include_directories (${${PROJECT_NAME}_BINARY_DIR}/src)

add_executable (${PROJECT_NAME} ${PROJECT_NAME}.c)
include_directories (${DEPLIBS_INCLUDE_DIRS}) 
target_link_libraries (${PROJECT_NAME} ${DEPLIBS_LDFLAGS})

install (TARGETS ${PROJECT_NAME} DESTINATION sbin)
install (FILES ${PROJECT_NAME}.ui DESTINATION share/${PROJECT_NAME})
install (FILES ${PROJECT_NAME}.desktop DESTINATION share/applications)
install (FILES ${PROJECT_NAME}-kde.desktop DESTINATION share/applications)
install (FILES ${PROJECT_NAME}.png DESTINATION share/pixmaps)
install (FILES init DESTINATION share/${PROJECT_NAME})

install (PROGRAMS multibootusb-location-detection.sh DESTINATION sbin)
install (PROGRAMS multiple_iso_on_usb.sh DESTINATION sbin)

install (PROGRAMS sli-locale_list-detection.sh DESTINATION sbin)
install (PROGRAMS get_current_keybtype.sh DESTINATION sbin)
install (PROGRAMS get_current_keymap.sh DESTINATION sbin)

install (PROGRAMS get_current_zone.sh DESTINATION sbin)
install (PROGRAMS images/slackel.png DESTINATION share/${PROJECT_NAME})
install (PROGRAMS images/salix.png DESTINATION share/${PROJECT_NAME})
