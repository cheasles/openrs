include(GNUInstallDirs)

set(CACHE_DIR "/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}" CACHE STRING "Cache directory")
set(CONFIG_DIR "/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}" CACHE STRING "Config directory")

message("* Cache directory        : ${CACHE_DIR}")
message("* Config directory       : ${CONFIG_DIR}")

configure_file(
	${CMAKE_SOURCE_DIR}/res/options.h.in
	${CMAKE_BINARY_DIR}/options.h
)
