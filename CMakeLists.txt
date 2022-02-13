cmake_minimum_required(VERSION 3.5.2)

set (PROJECT_NAME "multibootusb")
project (${PROJECT_NAME})
set (PROJECT_VERSION "1.0")

add_subdirectory (po)
add_subdirectory (src)
install (DIRECTORY doc/ DESTINATION doc/${PROJECT_NAME}-${PROJECT_VERSION})
option (GTK3 "Enable gtk+3.")

add_custom_target (uninstall)
add_custom_command (TARGET uninstall COMMAND xargs rm -f < ${CMAKE_BINARY_DIR}/install_manifest.txt)
add_custom_command (TARGET uninstall COMMAND rmdir ${CMAKE_INSTALL_PREFIX}/doc/${PROJECT_NAME}-${PROJECT_VERSION})
add_custom_command (TARGET uninstall COMMAND rmdir ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})
