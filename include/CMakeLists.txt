set(GAMMARAY_HEADERS
  metatypedeclarations.h
  objectmodel.h
  objectmodelbase.h
  objecttypefilterproxymodel.h
  probeinterface.h
  singlecolumnobjectproxymodel.h
  toolfactory.h

  # Deprecated, not added:
  #util.h
)

install(FILES ${GAMMARAY_HEADERS} DESTINATION include/gammaray/)
