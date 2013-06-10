set (LOCAL_NAME iSS)

add_executable(${LOCAL_NAME}
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

# install compiled binary and its dependencies
install (TARGETS ${LOCAL_NAME} DESTINATION ${LOCAL_NAME})
install (FILES parameters.dat OSCAR_header.txt DESTINATION ${LOCAL_NAME})
install (DIRECTORY EOS results tables DESTINATION ${LOCAL_NAME})
