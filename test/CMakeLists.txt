itk_module_test()

set(OptimizationMonitorTests
  itkExhaustiveMonitorTest.cxx
  )

CreateTestDriver(OptimizationMonitor "${OptimizationMonitor-Test_LIBRARIES}" "${OptimizationMonitorTests}")

# fixme
itk_add_test(NAME itkExhaustiveMonitorTest
  COMMAND OptimizationMonitorTestDriver itkExhaustiveMonitorTest 
  DATA{Input/apple.jpg}
  DATA{Input/orange.jpg}
  )
