itk_module_test()

set(OptimizationMonitorTests
  itkCommandExhaustiveLogTest.cxx
  itkNdArrayTest.cxx
  )

CreateTestDriver(OptimizationMonitor "${OptimizationMonitor-Test_LIBRARIES}" "${OptimizationMonitorTests}")

# fixme
itk_add_test(NAME itkCommandExhaustiveLogTest
  COMMAND OptimizationMonitorTestDriver itkCommandExhaustiveLogTest 
  DATA{Input/apple.jpg}
  DATA{Input/orange.jpg}
  )

itk_add_test(NAME itkNdArrayTest
  COMMAND OptimizationMonitorTestDriver itkNdArrayTest
  )
