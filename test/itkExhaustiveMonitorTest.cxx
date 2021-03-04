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

#include "itkImage.h"
#include "itkExhaustiveLog.h"
#include "itkTestingMacros.h"

int itkExhaustiveMonitorTest(int argc, char* argv[])
{
    using PixelType = unsigned char;
    constexpr unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using CommandType = itk::ExhaustiveLog<ImageType>;

    ImageType::Pointer img = ImageType::New();
    CommandType::Pointer log = CommandType::New();

    // Empty test to verify build success
    return EXIT_SUCCESS;
}
