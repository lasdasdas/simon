set (simonprocessopenedconditionplugin_SRCS 
  processopenedcondition.cpp 
  createprocessopenedconditionwidget.cpp
)

kde4_add_ui_files(simonprocessopenedconditionplugin_SRCS
  createprocessopenedconditionwidget.ui
)

kde4_add_plugin(simonprocessopenedconditionplugin ${simonprocessopenedconditionplugin_SRCS})

target_link_libraries(simonprocessopenedconditionplugin ${KDE4_KDEUI_LIBS}
  ${QT_QTXML_LIBRARY} simoncontextdetection simonuicomponents )

install(TARGETS simonprocessopenedconditionplugin DESTINATION ${PLUGIN_INSTALL_DIR} COMPONENT simonprocessopenedconditionplugin)
install(FILES simonprocessopenedconditionplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR} COMPONENT simonprocessopenedconditionplugin)
