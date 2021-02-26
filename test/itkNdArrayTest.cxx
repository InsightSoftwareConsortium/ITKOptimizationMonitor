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

#include "itkNdArray.h"
#include "itkTestingMacros.h"
#include "itkMacro.h"


int itkNdArrayTest(int argc, char* argv[])
{
    using InternalDataType = double;
    using ArrayType = itk::NdArray<InternalDataType>;
    using LengthType = typename ArrayType::LengthType;

    // Create 5-dimensional array
    LengthType arrayDims;
    arrayDims.SetSize(5);
    arrayDims[0] = 5;
    arrayDims[1] = 2;
    arrayDims[2] = 10;
    arrayDims[3] = 1;
    arrayDims[4] = 4;

    ArrayType::Pointer arr = ArrayType::New();
    arr->Initialize(arrayDims);

    ITK_TEST_EXPECT_EQUAL(arr->GetDimension(), 5);
    ITK_TEST_EXPECT_EQUAL(arr->GetDataLength()[0], 5);
    ITK_TEST_EXPECT_EQUAL(arr->GetDataLength()[3], 1);
    ITK_TEST_EXPECT_EQUAL(arr->GetDataSize(), 400);

    // Test random insertion
    LengthType position;
    position.SetSize(arr->GetDimension());
    position[0] = 3;
    position[1] = 1;
    position[2] = 7;
    position[3] = 0;
    position[4] = 2;
    arr->SetElement(position, 15.3);
    ITK_TEST_EXPECT_EQUAL(arr->GetElement(position), 15.3);

    // Test insertion at extremes
    position[0] = 4;
    position[1] = 1;
    position[2] = 9;
    position[3] = 0;
    position[4] = 3;
    arr->SetElement(position, 102.323);
    ITK_TEST_EXPECT_EQUAL(arr->GetElement(position), 102.323);

    position.Fill(0);
    arr->SetElement(position, 33.2);
    ITK_TEST_EXPECT_EQUAL(arr->GetElement(position), 33.2);

    // Test insertion out of bounds
    position[0] = 5;
    ITK_TRY_EXPECT_EXCEPTION(arr->SetElement(position, 11.1));
    ITK_TRY_EXPECT_EXCEPTION(arr->GetElement(position));

    return EXIT_SUCCESS;
}