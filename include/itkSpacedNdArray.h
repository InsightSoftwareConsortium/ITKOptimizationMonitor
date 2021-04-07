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

#ifndef itkSpacedNdArray_h
#define itkSpacedNdArray_h

#include "itkNdArray.h"
#include "itkArray.h"

namespace itk
{
    template <typename TInternalData>
    class SpacedNdArray : public itk::NdArray<TInternalData>
    {
    public:
        using Self = SpacedNdArray;
        using Superclass = itk::NdArray<TInternalData>;
        using Pointer = itk::SmartPointer<Self>;
        itkNewMacro(Self);

        using InternalDataType = Superclass::InternalDataType;

        using LengthType = Superclass::LengthType;
        using PositionType = itk::Array<double>;

        itkGetConstReferenceMacro(StepSize, PositionType);
        itkGetConstReferenceMacro(Anchor, PositionType);

        // Initialize array in memory
        void Initialize(const LengthType& arrayDimensions,
            const PositionType& stepSize,
            const PositionType& anchor);

        bool IsValidPosition(const PositionType& position) const;

        void SetElement(const PositionType& position, const InternalDataType value);
        InternalDataType GetElement(const PositionType& position) const;

    protected:
        SpacedNdArray();
        ~SpacedNdArray() override;

    private:
        void GetDiscreteIndexFromPosition(const PositionType& position, LengthType& index) const;

    private:
        PositionType m_StepSize;
        PositionType m_Anchor;        // Position in continuous space of array element at [0; 0; ... 0]
    };
} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSpacedNdArray.hxx"
#endif

#endif // itkSpacedNdArray_h