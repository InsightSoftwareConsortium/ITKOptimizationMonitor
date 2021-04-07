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

#include "itkSpacedNdArray.h"
#include "itkTestingMacros.h"
#include "itkMacro.h"


int itkSpacedNdArrayTest(int argc, char* argv[])
{
    using InternalDataType = double;
    using ArrayType = itk::SpacedNdArray<InternalDataType>;
    using LengthType = typename ArrayType::LengthType;
    using PositionType = typename ArrayType::PositionType;

    // Create 3-dimensional array
    int Dimension = 3;

    LengthType arrayLen;
    arrayLen.SetSize(Dimension);
    arrayLen[0] = 5;
    arrayLen[1] = 2;
    arrayLen[2] = 10;

    // Set spacing along each axis
    PositionType steps;
    steps.SetSize(Dimension);
    steps[0] = 1.0;
    steps[1] = 0.5;
    steps[2] = 2.0;
    
    // Set position of first element in space
    PositionType anchor;
    anchor.SetSize(Dimension);
    anchor[0] = 5.0;
    anchor[1] = 10.0;
    anchor[2] = 15.0;

    // Create array and verify size
    ArrayType::Pointer arr = ArrayType::New();
    arr->Initialize(arrayLen, steps, anchor);
    ITK_TEST_EXPECT_EQUAL(arr->GetDataSize(), 100);

    // Test random insertion
    PositionType pos;
    pos.SetSize(Dimension);
    pos[0] = 6.0;
    pos[1] = 10.5;
    pos[2] = 21.0;
    ITK_TRY_EXPECT_NO_EXCEPTION(arr->SetElement(pos, 3.2));
    ITK_TEST_EXPECT_EQUAL(arr->GetElement(pos), 3.2);

    // Test out of bounds insertion
    pos.Fill(0);
    ITK_TRY_EXPECT_EXCEPTION(arr->SetElement(pos, 5.1));

    return EXIT_SUCCESS;
}