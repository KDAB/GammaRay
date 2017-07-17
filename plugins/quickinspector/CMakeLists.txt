
# check if we can build the Quick inspector
# TODO 5.1.0 and below require private V8 headers which do not provide CMake variables,
# so we would need to find those manually
if(Qt5Quick_FOUND AND HAVE_PRIVATE_Qt5Quick_HEADERS AND NOT ${Qt5Quick_VERSION} VERSION_LESS 5.2.0)
  set(HAVE_QUICK_INSPECTOR TRUE)

  # shared stuff
  set(gammaray_quickinspector_shared_srcs
        quickinspectorinterface.cpp
        quickitemgeometry.cpp
        quickdecorationsdrawer.cpp
        materialextension/materialextensioninterface.cpp
      )

  # Find if glReadPixels can be used with this configuration
  # If yes, find based on which platform GL/GLESv2
  if (TARGET Qt5::Gui_GLESv2)
      include(CheckCXXSourceCompiles)
      set (CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} Qt5::Gui_GLESv2)
      CHECK_CXX_SOURCE_COMPILES("int main() {return 0;}" CAN_LINK_GUI_GLESV2)
      if (CAN_LINK_GUI_GLESV2)
          set(HAVE_GLESv2 TRUE)
          add_definitions(-DENABLE_GL_READPIXELS)
      endif ()
  endif()

  find_package(OpenGL)
  if (OpenGL_FOUND)
      add_definitions(-DENABLE_GL_READPIXELS)
  endif()

  add_library(gammaray_quickinspector_shared STATIC ${gammaray_quickinspector_shared_srcs})
  target_link_libraries(gammaray_quickinspector_shared gammaray_common Qt5::Gui Qt5::Quick)

  # on embedded devices link against GLESv2 for glReadpixels
  if (HAVE_GLESv2)
    target_link_libraries(gammaray_quickinspector_shared Qt5::Gui_GLESv2)
  endif()

  set_target_properties(gammaray_quickinspector_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)

  set(gammaray_quickinspector_srcs
    quickinspector.cpp
    quickitemmodel.cpp
    quickscenegraphmodel.cpp
    quickpaintanalyzerextension.cpp
    quickoverlay.cpp
    materialextension/materialextension.cpp
    materialextension/materialshadermodel.cpp
    materialextension/qquickopenglshadereffectmaterialadaptor.cpp
    geometryextension/sggeometryextension.cpp
    geometryextension/sggeometrymodel.cpp
    textureextension/textureextension.cpp
    textureextension/qsgtexturegrabber.cpp
  )

  gammaray_add_plugin(gammaray_quickinspector
    JSON gammaray_quickinspector.json
    SOURCES ${gammaray_quickinspector_srcs}
  )

  target_include_directories(gammaray_quickinspector SYSTEM PRIVATE ${Qt5Quick_PRIVATE_INCLUDE_DIRS})
  target_link_libraries(gammaray_quickinspector
    gammaray_quickinspector_shared
    gammaray_core Qt5::Quick
    gammaray_kitemmodels
  )

  if(GAMMARAY_BUILD_UI)
    # ui plugin
    set(gammaray_quickinspector_ui_srcs
      quickinspectorwidget.cpp
      quickinspectorclient.cpp
      quickclientitemmodel.cpp
      quickitemdelegate.cpp
      quickitemtreewatcher.cpp
      quickscenepreviewwidget.cpp
      quickscenecontrolwidget.cpp
      quickoverlaylegend.cpp
      gridsettingswidget.cpp

      materialextension/materialextensionclient.cpp
      materialextension/materialtab.cpp
      geometryextension/sggeometrytab.cpp
      geometryextension/sgwireframewidget.cpp
      textureextension/texturetab.cpp
      textureextension/textureviewwidget.cpp
    )

    gammaray_add_plugin(gammaray_quickinspector_ui
      JSON gammaray_quickinspector.json
      SOURCES ${gammaray_quickinspector_ui_srcs}
    )

    target_link_libraries(gammaray_quickinspector_ui
      gammaray_quickinspector_shared
      gammaray_ui
      Qt5::Quick
    )
  endif()
endif()

add_feature_info(
  "QtQuick 2 inspector"
  HAVE_QUICK_INSPECTOR
  "Requires private Qt5Quick headers and Qt >= 5.2.0 to be available."
)
