set(gammaray_shared_test_srcs
    variantpropertyobject.cpp
    changingpropertyobject.cpp
    propertytestobject.cpp
)

add_library(gammaray_shared_test_data STATIC ${gammaray_shared_test_srcs})
target_link_libraries(gammaray_shared_test_data ${QT_QTCORE_LIBRARIES})
