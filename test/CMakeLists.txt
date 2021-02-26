itk_module_test()

set(OptimizationMonitorTests
  itkCommandExhaustiveLogTest.cxx
  itkNdArrayTest.cxx
  itkSpacedNdArrayTest.cxx
  )

CreateTestDriver(OptimizationMonitor "${OptimizationMonitor-Test_LIBRARIES}" "${OptimizationMonitorTests}")

itk_add_test(NAME itkCommandExhaustiveLogTest
  COMMAND OptimizationMonitorTestDriver itkCommandExhaustiveLogTest 
  DATA{Input/apple.jpg}
  DATA{Input/orange.jpg}
  ${CMAKE_CURRENT_BINARY_DIR}/registration_output.nrrd
  )

itk_add_test(NAME itkNdArrayTest
  COMMAND OptimizationMonitorTestDriver itkNdArrayTest
  )

itk_add_test(NAME itkSpacedNdArrayTest
  COMMAND OptimizationMonitorTestDriver itkSpacedNdArrayTest
  )
