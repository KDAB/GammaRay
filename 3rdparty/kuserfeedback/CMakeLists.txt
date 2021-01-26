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
list(APPEND gammaray_userfeedback_srcs
    core/auditloguicontroller.cpp
    widgets/auditlogbrowserdialog.cpp
)

add_library(gammaray_kuserfeedback SHARED ${gammaray_userfeedback_srcs})
# Disable warnings for generated stuff
if(MSVC)
    set_source_files_properties(common/surveytargetexpressionlexer.cpp PROPERTIES COMPILE_FLAGS "/wd4018 /wd4996")
  else()
    check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant HAVE_NO_CXX_ZERO_AS_NULL_POINTER_CON)
    check_cxx_compiler_flag(-Wno-unneeded-internal-declaration HAVE_NO_CXX_UNNEEDED_INTERNAL_DEC)
    check_cxx_compiler_flag(-Wno-null-conversion HAVE_NO_CXX_NULL_CONVERSION)
    set(CXX_FLAGS_QUIET_LEX_GEN_WARNINGS "-Wno-sign-compare")
    if(HAVE_NO_CXX_ZERO_AS_NULL_POINTER_CON)
      set(CXX_FLAGS_QUIET_LEX_GEN_WARNINGS "${CXX_FLAGS_QUIET_LEX_GEN_WARNINGS} -Wno-zero-as-null-pointer-constant")
    endif()
    if(HAVE_NO_CXX_UNNEEDED_INTERNAL_DEC)
      set(CXX_FLAGS_QUIET_LEX_GEN_WARNINGS "${CXX_FLAGS_QUIET_LEX_GEN_WARNINGS} -Wno-unneeded-internal-declaration")
    endif()
    if(HAVE_NO_CXX_NULL_CONVERSION)
      set(CXX_FLAGS_QUIET_LEX_GEN_WARNINGS "${CXX_FLAGS_QUIET_LEX_GEN_WARNINGS} -Wno-null-conversion")
    endif()
    set_source_files_properties(common/surveytargetexpressionlexer.cpp PROPERTIES COMPILE_FLAGS "${CXX_FLAGS_QUIET_LEX_GEN_WARNINGS}")
endif()
if(NOT HAVE_UNISTD_H)
    target_compile_definitions(gammaray_kuserfeedback PRIVATE YY_NO_UNISTD_H)
endif()
target_include_directories(gammaray_kuserfeedback
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}/core>"
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)
set_target_properties(gammaray_kuserfeedback PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES})
gammaray_set_rpath(gammaray_kuserfeedback ${LIB_INSTALL_DIR})
generate_export_header(gammaray_kuserfeedback)

target_link_libraries(gammaray_kuserfeedback PUBLIC Qt::Widgets PRIVATE Qt::Gui Qt::Network)

install(TARGETS gammaray_kuserfeedback EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
