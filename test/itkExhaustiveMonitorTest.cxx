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

#include "itkImageFileReader.h"
#include "itkEuler2DTransform.h"
#include "itkExhaustiveOptimizerv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkCenteredTransformInitializer.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkImage.h"

int itkExhaustiveMonitorTest(int argc, char* argv[])
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

    // Create the Command observer and register it with the optimizer.
    //
    itk::CommandExhaustiveLog<double>::Pointer observer = itk::CommandExhaustiveLog<double>::New();
    optimizer->AddObserver(itk::StartEvent(), observer);
    optimizer->AddObserver(itk::IterationEvent(), observer);

    unsigned int             angles = 12;
    OptimizerType::StepsType steps(transform->GetNumberOfParameters());
    steps[0] = int(angles / 2);
    steps[1] = 0;
    steps[2] = 0;
    optimizer->SetNumberOfSteps(steps);

    OptimizerType::ScalesType scales(transform->GetNumberOfParameters());
    scales[0] = 2.0 * itk::Math::pi / angles;
    scales[1] = 1.0;
    scales[2] = 1.0;

    optimizer->SetScales(scales);

    initializer->SetTransform(transform);
    initializer->SetFixedImage(fixedImage);
    initializer->SetMovingImage(movingImage);
    initializer->InitializeTransform();

    // Initialize registration
    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetFixedImage(fixedImage);
    registration->SetMovingImage(movingImage);
    registration->SetInitialTransform(transform);
    registration->SetNumberOfLevels(1);
    try
    {
        registration->Update();
    }
    catch (itk::ExceptionObject& err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Got dimension " << observer->GetDimension() << std::endl;
    std::cout << "Got number of steps 0 " << observer->GetNumberOfSteps()[0] << std::endl;
    std::cout << "Got step size 0 " << observer->GetStepSize()[0] << std::endl;
    std::cout << "Got " << observer->GetDataSize() << " data elements " << std::endl;

    return EXIT_SUCCESS;
}
