itk_module_test()

set(OptimizationMonitorTests
  itkExhaustiveMonitorTest.cxx
  )

CreateTestDriver(OptimizationMonitor "${OptimizationMonitor-Test_LIBRARIES}" "${OptimizationMonitorTests}")

# fixme
itk_add_test(NAME itkExhaustiveMonitorTest
  COMMAND OptimizationMonitorTestDriver itkExhaustiveMonitorTest 
  "C:/repos/ex-b/src/Numerics/Optimizers/ExhaustiveOptimizer/apple.jpg"
  "C:/repos/ex-b/src/Numerics/Optimizers/ExhaustiveOptimizer/orange.jpg"
  )
