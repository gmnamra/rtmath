include (common)

set(genparams-qt_mocheaders frmmain.h frmtarget.h)
set(genparams-qt_uis frmmain.ui frmTarget.ui)
#set(genparams-qt_rcs frmmain.qrc)

find_package( Qt4 COMPONENTS QtCore QtGui REQUIRED )
#find_package( Qt4 REQUIRED QtCore QtGui )
#message("${QT_LIBRARIES}")
#message("${QT_INCLUDES}")

#QT4_ADD_RESOURCES( genparams-qt_RC_SRCS ${genparams-qt_rcs} )

QT4_WRAP_UI ( genparams-qt_UI_HDRS ${genparams-qt_uis} )

QT4_WRAP_CPP ( genparams-qt_MOC_SRCS ${genparams-qt_mocheaders} )

include_directories (
	${CMAKE_BINARY_DIR}
)

#include_directories ( BEFORE SYSTEM 
#	${QT_INCLUDE_DIR}
#)

add_executable (rtmath-ddscat-genparams-qt 
	main.cpp stdafx.cpp frmmain.cpp frmtarget.cpp
	${genparams-qt_MOC_SRCS}
#	${genparams-qt_RC_SRCS}
	${genparams-qt_UI_HDRS}
	)

addapp(rtmath-ddscat-genparams-qt)
INCLUDE( ${QT_USE_FILE} )
TARGET_LINK_LIBRARIES( rtmath-ddscat-genparams-qt ${QT_LIBRARIES} )

