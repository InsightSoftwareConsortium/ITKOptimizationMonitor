# the top-level README is used for describing this module, just
# re-used it for documentation here
get_filename_component(MY_CURRENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(READ "${MY_CURRENT_DIR}/README.rst" DOCUMENTATION)

# itk_module() defines the module dependencies in OptimizationMonitor
# OptimizationMonitor depends on ITKCommon
# The testing module in OptimizationMonitor depends on ITKTestKernel
# and ITKMetaIO(besides OptimizationMonitor and ITKCore)
# By convention those modules outside of ITK are not prefixed with
# ITK.

# define the dependencies of the include module and the tests
itk_module(OptimizationMonitor
  DEPENDS
    ITKCommon
    ITKOptimizersv4
  COMPILE_DEPENDS
    ITKImageSources
  TEST_DEPENDS
    ITKTestKernel
    ITKMetaIO
    ITKMetricsv4
    ITKRegistrationMethodsv4
  DESCRIPTION
    "${DOCUMENTATION}"
  EXCLUDE_FROM_DEFAULT
)
