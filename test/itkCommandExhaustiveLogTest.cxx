/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkCommandExhaustiveLog.h"
#include "itkTestingMacros.h"
#include "itkMacro.h"
#include "itkCommand.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkEuler2DTransform.h"
#include "itkExhaustiveOptimizerv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkCenteredTransformInitializer.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkExtractImageFilter.h"

int
itkCommandExhaustiveLogTest(int argc, char * argv[])
{
  // Derived from ExhaustiveOptimizer example documentation
  if (argc < 3)
  {
    std::cout << "Usage: OptimizationMonitorTestDriver 0 fixedImage movingImage outputImage" << std::endl;
    return EXIT_FAILURE;
  }

  using FixedImageType = itk::Image<double, 2>;
  using MovingImageType = itk::Image<double, 2>;
  using FixedImageReaderType = itk::ImageFileReader<FixedImageType>;
  using MovingImageReaderType = itk::ImageFileReader<MovingImageType>;
  using TransformType = itk::Euler2DTransform<double>;
  using OptimizerType = itk::ExhaustiveOptimizerv4<double>;
  using MetricType = itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType>;
  using TransformInitializerType = itk::CenteredTransformInitializer<TransformType, FixedImageType, MovingImageType>;
  using RegistrationType = itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, TransformType>;

  FixedImageReaderType::Pointer     fixedImageReader = FixedImageReaderType::New();
  MovingImageReaderType::Pointer    movingImageReader = MovingImageReaderType::New();
  FixedImageType::Pointer           fixedImage = FixedImageType::New();
  MovingImageType::Pointer          movingImage = MovingImageType::New();
  TransformType::Pointer            transform = TransformType::New();
  MetricType::Pointer               metric = MetricType::New();
  OptimizerType::Pointer            optimizer = OptimizerType::New();
  RegistrationType::Pointer         registration = RegistrationType::New();
  TransformInitializerType::Pointer initializer = TransformInitializerType::New();

  fixedImageReader->SetFileName(argv[1]);
  fixedImageReader->Update();
  fixedImage = fixedImageReader->GetOutput();

  movingImageReader->SetFileName(argv[2]);
  movingImageReader->Update();
  movingImage = movingImageReader->GetOutput();

  // Exhaustively iterate over domain [-1:1, -10:10, -1:1] in increments [0.1, 1.0, 1.0]
  OptimizerType::StepsType steps(transform->GetNumberOfParameters());
  steps[0] = 10;
  steps[1] = 10;
  steps[2] = 1;
  optimizer->SetNumberOfSteps(steps);

  OptimizerType::ScalesType scales(transform->GetNumberOfParameters());
  scales[0] = 0.1;
  scales[1] = 1.0;
  scales[2] = 1.0;

  optimizer->SetScales(scales);

  initializer->SetTransform(transform);
  initializer->SetFixedImage(fixedImage);
  initializer->SetMovingImage(movingImage);
  initializer->InitializeTransform();

  // Verify initial transform initialized at 0 for this example
  ITK_TEST_EXPECT_EQUAL(transform->GetParameters()[0], 0);
  ITK_TEST_EXPECT_EQUAL(transform->GetParameters()[1], 0);
  ITK_TEST_EXPECT_EQUAL(transform->GetParameters()[2], 0);

  // Initialize registration
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  registration->SetFixedImage(fixedImage);
  registration->SetMovingImage(movingImage);
  registration->SetInitialTransform(transform);
  registration->SetNumberOfLevels(1);

  // Create the Command observer and register it with the optimizer.
  using ObserverType = itk::CommandExhaustiveLog<double, TransformType::ParametersDimension>;
  ObserverType::Pointer observer = ObserverType::New();

  // Set center
  ObserverType::PointType center = typename ObserverType::PointType();
  for (int i = 0; i < transform->GetNumberOfParameters(); i++)
  {
    center[i] = transform->GetParameters()[i];
  }
  observer->SetCenter(center);
  optimizer->AddObserver(itk::StartEvent(), observer);
  optimizer->AddObserver(itk::IterationEvent(), observer);

  try
  {
    registration->Update();
    std::cout << "  MinimumMetricValue: " << optimizer->GetMinimumMetricValue() << std::endl;
    std::cout << "  MaximumMetricValue: " << optimizer->GetMaximumMetricValue() << std::endl;
    std::cout << "  MinimumMetricValuePosition: " << optimizer->GetMinimumMetricValuePosition() << std::endl;
    std::cout << "  MaximumMetricValuePosition: " << optimizer->GetMaximumMetricValuePosition() << std::endl;
    std::cout << "  StopConditionDescription: " << optimizer->GetStopConditionDescription() << std::endl;
  }
  catch (itk::ExceptionObject & err)
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  // Verify observer parameters were initialized correctly
  ITK_TEST_EXPECT_EQUAL(ObserverType::Dimension, 3);

  ITK_TEST_EXPECT_EQUAL(observer->GetNumberOfSteps(0), 10);
  ITK_TEST_EXPECT_EQUAL(observer->GetNumberOfSteps(1), 10);
  ITK_TEST_EXPECT_EQUAL(observer->GetNumberOfSteps(2), 1);

  ITK_TEST_EXPECT_EQUAL(observer->GetSize(0), 21);
  ITK_TEST_EXPECT_EQUAL(observer->GetSize(1), 21);
  ITK_TEST_EXPECT_EQUAL(observer->GetSize(2), 3);

  ITK_TEST_EXPECT_EQUAL(observer->GetStepSize()[0], 0.1);
  ITK_TEST_EXPECT_EQUAL(observer->GetStepSize()[1], 1.0);
  ITK_TEST_EXPECT_EQUAL(observer->GetStepSize()[2], 1.0);

  // Verify zero index is at smallest domain location value
  ObserverType::ParametersType position;
  position.SetSize(3);
  position[0] = -1;
  position[1] = -10;
  position[2] = -1;

  ObserverType::IndexType index;
  index.Fill(0);
  ITK_TEST_EXPECT_EQUAL(observer->GetValue(index), observer->GetValue(position));

  // Move one step in each dimension

  index[0] = 1;
  position[0] = -0.9;
  ITK_TEST_EXPECT_EQUAL(observer->GetValue(index), observer->GetValue(position));

  index[1] = 1;
  position[1] = -9;
  ITK_TEST_EXPECT_EQUAL(observer->GetValue(index), observer->GetValue(position));

  index[2] = 1;
  position[2] = 0;
  ITK_TEST_EXPECT_EQUAL(observer->GetValue(index), observer->GetValue(position));

  // Verify largest index is at maximum domain location value
  position[0] = 1;
  position[1] = 10;
  position[2] = 1;
  index[0] = 20;
  index[1] = 20;
  index[2] = 2;
  ITK_TEST_EXPECT_EQUAL(observer->GetValue(index), observer->GetValue(position));

  // Minimum position aligns with expectation
  position[0] = 0;
  position[1] = 0;
  position[2] = 1;
  ITK_TEST_EXPECT_EQUAL(optimizer->GetMinimumMetricValue(), observer->GetValue(position));

  // Maximum position aligns with expectation
  position[0] = 0.6;
  position[1] = 10;
  position[2] = -1;
  ITK_TEST_EXPECT_EQUAL(optimizer->GetMaximumMetricValue(), observer->GetValue(position));


  // Write out a 2D data slice for visualization and baseline comparison
  ObserverType::ImagePointer image = observer->GetImage();

  using InputImageType = typename ObserverType::ImageType;
  using OutputImageType = typename itk::Image<float, 2>;
  using InputPointType = typename InputImageType::PointType;
  using InputRegionType = InputImageType::RegionType;
  using FilterType = itk::ExtractImageFilter<InputImageType, OutputImageType>;

  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(observer->GetImage());
  filter->SetDirectionCollapseToSubmatrix();

  // Get slice containing maximum position
  InputPointType point = InputPointType();
  point[0] = 0.6;
  point[1] = 10;
  point[2] = -1;
  index = observer->GetImage()->TransformPhysicalPointToIndex(point);

  InputRegionType desiredRegion = observer->GetImage()->GetBufferedRegion();
  desiredRegion.GetModifiableSize()[2] = 0;         // Extract along third dimension
  desiredRegion.GetModifiableIndex()[0] = index[2]; // Fix 0th dimension coordinate
  ITK_TEST_EXPECT_EQUAL(desiredRegion.GetSize()[2], 0);
  ITK_TEST_EXPECT_EQUAL(desiredRegion.GetIndex()[2], 0);
  filter->SetExtractionRegion(desiredRegion);

  using OutputWriterType = itk::ImageFileWriter<OutputImageType>;

  OutputWriterType::Pointer outputImageWriter = OutputWriterType::New();
  outputImageWriter->SetInput(filter->GetOutput());
  outputImageWriter->SetFileName(argv[3]);
  outputImageWriter->Update();
  std::cout << "Wrote output file " << argv[3] << std::endl;

  return EXIT_SUCCESS;
}
