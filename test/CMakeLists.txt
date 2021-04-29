itk_module_test()

set(OptimizationMonitorTests
  itkCommandExhaustiveLogTest.cxx
  )

CreateTestDriver(OptimizationMonitor "${OptimizationMonitor-Test_LIBRARIES}" "${OptimizationMonitorTests}")

itk_add_test(NAME itkCommandExhaustiveLogTest
  COMMAND OptimizationMonitorTestDriver itkCommandExhaustiveLogTest 
  DATA{Input/apple.jpg}
  DATA{Input/orange.jpg}
  ${CMAKE_CURRENT_BINARY_DIR}/registration_output.nrrd
  )
