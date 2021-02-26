itk_module_test()

set(OptimizationMonitorTests
  itkExhaustiveMonitorTest.cxx
  )
message (STATUS "${OptimizationMonitor-Test_LIBRARIES}")

CreateTestDriver(OptimizationMonitor "${OptimizationMonitor-Test_LIBRARIES}" "${OptimizationMonitorTests}")

#itk_add_test(NAME itkMinimalStandardRandomVariateGeneratorTest
#  COMMAND OptimizationMonitorTestDriver itkMinimalStandardRandomVariateGeneratorTest
#  )
