set(widget_layouting_srcs
  widget-layouting.cpp
)
qt4_wrap_ui(widget_layouting_srcs
  contactform.ui
)

add_executable(example-widget-layouting ${widget_layouting_srcs})
target_link_libraries(example-widget-layouting Qt5::Widgets)
