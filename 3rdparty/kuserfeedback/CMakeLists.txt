set(gammaray_userfeedback_srcs
    common/surveytargetexpression.cpp
    common/surveytargetexpressionevaluator.cpp
    common/surveytargetexpressionlexer.cpp
    common/surveytargetexpressionparser.cpp
    common/surveytargetexpressionparser_p.cpp
    core/abstractdatasource.cpp
    core/applicationversionsource.cpp
    core/compilerinfosource.cpp
    core/feedbackconfiguicontroller.cpp
    core/openglinfosource.cpp
    core/platforminfosource.cpp
    core/provider.cpp
    core/qtversionsource.cpp
    core/selectionratiosource.cpp
    core/startcountsource.cpp
    core/surveyinfo.cpp
    core/usagetimesource.cpp
    widgets/feedbackconfigdialog.cpp
    widgets/feedbackconfigwidget.cpp
    widgets/notificationpopup.cpp
)

qt4_wrap_ui(gammaray_userfeedback_srcs
    widgets/feedbackconfigdialog.ui
    widgets/feedbackconfigwidget.ui
    widgets/notificationpopup.ui
)

add_library(gammaray_kuserfeedback SHARED ${gammaray_userfeedback_srcs})
# Disable warnings for generated stuff
if(MSVC)
    set_source_files_properties(common/surveytargetexpressionlexer.cpp PROPERTIES COMPILE_FLAGS "/we4018 /we4996")
else()
    set_source_files_properties(common/surveytargetexpressionlexer.cpp PROPERTIES COMPILE_FLAGS "-Wno-sign-compare -Wno-unneeded-internal-declaration")
endif()
if(NOT HAVE_UNISTD_H)
    target_compile_definitions(gammaray_kuserfeedback PRIVATE YY_NO_UNISTD_H)
endif()
target_include_directories(gammaray_kuserfeedback
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}/core>"
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)
set_target_properties(gammaray_kuserfeedback PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES})
generate_export_header(gammaray_kuserfeedback)

if(Qt5Core_FOUND)
    target_link_libraries(gammaray_kuserfeedback PUBLIC Qt5::Widgets PRIVATE Qt5::Gui Qt5::Network)
else()
    target_link_libraries(gammaray_kuserfeedback PUBLIC ${QT_QTGUI_LIBRARIES} PRIVATE ${QT_QTNETWORK_LIBRARIES})
endif()

install(TARGETS gammaray_kuserfeedback EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
