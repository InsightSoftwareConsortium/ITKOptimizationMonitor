/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
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

#include "itkArray2D.h"
#include "itkImageFileReader.h"
#include "itkEuler2DTransform.h"
#include "itkExhaustiveOptimizerv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkCenteredTransformInitializer.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkImage.h"

int itkCommandExhaustiveLogTest(int argc, char* argv[])
{
    // Derived from ExhaustiveOptimizer example documentation
    if (argc < 2)
    {
        std::cout << "Usage: OptimizationMonitorTestDriver 0 fixedImage movingImage" << std::endl;
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
    std::cout << "Init position: " << transform->GetParameters() << std::endl;

    // Initialize registration
    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetFixedImage(fixedImage);
    registration->SetMovingImage(movingImage);
    registration->SetInitialTransform(transform);
    registration->SetNumberOfLevels(1);

    // Create the Command observer and register it with the optimizer.
    //
    using ObserverType = itk::CommandExhaustiveLog<double>;
    ObserverType::Pointer observer = ObserverType::New();
    observer->SetCenter(transform->GetParameters());
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
    catch (itk::ExceptionObject& err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    // Verify observer parameters were initialized correctly
    ITK_TEST_EXPECT_EQUAL(observer->GetDimension(), 3);

    ITK_TEST_EXPECT_EQUAL(observer->GetNumberOfSteps(0), 10);
    ITK_TEST_EXPECT_EQUAL(observer->GetNumberOfSteps(1), 10);
    ITK_TEST_EXPECT_EQUAL(observer->GetNumberOfSteps(2), 1);

    ITK_TEST_EXPECT_EQUAL(observer->GetStepSize()[0], 0.1);
    ITK_TEST_EXPECT_EQUAL(observer->GetStepSize()[1], 1.0);
    ITK_TEST_EXPECT_EQUAL(observer->GetStepSize()[2], 1.0);

    ITK_TEST_EXPECT_EQUAL(observer->GetDataSize(), 1323);

    // Verify zero index is at smallest domain location value
    ////FIXME
    itk::Array<double> position;
    position.SetSize(3);
    //position.SetElement(0, -1);
    //position.SetElement(1, -10);
    //position.SetElement(2, -1);
    //ITK_TEST_EXPECT_EQUAL(observer->GetData()[0], observer->GetDataAtPosition(position));

    //// Move one step in third dimension
    //position.SetElement(0, -1);
    //position.SetElement(1, -10);
    //position.SetElement(2, 0);
    //ITK_TEST_EXPECT_EQUAL(observer->GetData()[1], observer->GetDataAtPosition(position));

    //// Move one step in second dimension
    //position.SetElement(0, -1);
    //position.SetElement(1, -9);
    //position.SetElement(2, -1);
    //ITK_TEST_EXPECT_EQUAL(observer->GetData()[3], observer->GetDataAtPosition(position));

    //// Move one step in first dimension
    //position.SetElement(0, -0.9);
    //position.SetElement(1, -10);
    //position.SetElement(2, -1);
    //ITK_TEST_EXPECT_EQUAL(observer->GetData()[63], observer->GetDataAtPosition(position));

    //// Verify largest index is at maximum domain location value
    //position.SetSize(3);
    //position.SetElement(0, 1);
    //position.SetElement(1, 10);
    //position.SetElement(2, 1);
    //ITK_TEST_EXPECT_EQUAL(observer->GetData()[1322], observer->GetDataAtPosition(position));

    // Minimum position aligns with expectation
    position.SetElement(0, 0);
    position.SetElement(1, 0);
    position.SetElement(2, 1);
    ITK_TEST_EXPECT_EQUAL(optimizer->GetMinimumMetricValue(), observer->GetDataAtPosition(position));

    // Maximum position aligns with expectation
    position.SetElement(0, 0.6);
    position.SetElement(1, 10);
    position.SetElement(2, -1);
    ITK_TEST_EXPECT_EQUAL(optimizer->GetMaximumMetricValue(), observer->GetDataAtPosition(position));

    // Get a 2D data slice
    const double FIXED_POSITION_0 = 0.6;
    std::vector<bool> dimIsVariable;
    dimIsVariable.push_back(false);
    position[0] = FIXED_POSITION_0;
    dimIsVariable.push_back(true);
    dimIsVariable.push_back(true);

    itk::Array2D<double> arr;
    observer->GetDataSlice2D(position, dimIsVariable, arr);

    // Verify slice size matches expectation
    size_t expectedSize = (observer->GetDataLength(1) * observer->GetDataLength(2));
    ITK_TEST_EXPECT_EQUAL(arr.size(), expectedSize);

    // Verify known maximum is at expected location in the slice
    ITK_TEST_EXPECT_EQUAL(optimizer->GetMaximumMetricValue(), arr.GetElement(20, 0));

    // Verify slice elements match respective positions in observer data 
    int rowSteps = steps[1];
    int colSteps = steps[2];
    for (int i = -1 * rowSteps; i <= rowSteps; i++) {
        for (int j = -1 * colSteps ; j <= colSteps; j++) {
            position[0] = FIXED_POSITION_0;
            position[1] = observer->GetCenter()[1] + i * scales[1];
            position[2] = observer->GetCenter()[2] + j * scales[2];
            ITK_TEST_EXPECT_EQUAL(observer->GetDataAtPosition(position), arr.GetElement(i + rowSteps, j + colSteps));
        }
    }

    return EXIT_SUCCESS;
}
