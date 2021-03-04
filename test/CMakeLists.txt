itk_module_test()

set(OptimizationMonitorTests
  itkExhaustiveMonitorTest.cxx
  )

CreateTestDriver(OptimizationMonitor "${OptimizationMonitor-Test_LIBRARIES}" "${OptimizationMonitorTests}")

itk_add_test(NAME itkExhaustiveMonitorTest
  COMMAND OptimizationMonitorTestDriver itkExhaustiveMonitorTest
  )
