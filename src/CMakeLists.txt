add_executable(iSS
  main.cpp
  arsenal.cpp
  ParameterReader.cpp
  RandomVariable1DArray.cpp
  RandomVariable2DArray.cpp
  RandomVariable.cpp
  NBD.cpp
  TableFunction.cpp
  emissionfunction.cpp
  Table.cpp
  readindata.cpp
  )

install (TARGETS iSS DESTINATION ${CMAKE_HOME_DIRECTORY})
