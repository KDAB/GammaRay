add_executable(
    example-qt3d-geometry
    mycylinder.cpp qt3d-geometry.cpp
)
target_link_libraries(
    example-qt3d-geometry Qt::3DExtras
)
if(QNXNTO)
    target_link_libraries(example-qt3d-geometry m)
endif()
