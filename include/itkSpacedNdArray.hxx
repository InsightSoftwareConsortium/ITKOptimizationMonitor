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

#ifndef itkSpacedNdArray_hxx
#define itkSpacedNdArray_hxx

#include <math.h>

#include "itkSpacedNdArray.h"
#include "itkMacro.h""

namespace itk
{
    // TODO overload constructor to call Initialize()
    template <typename TInternalData>
    SpacedNdArray<TInternalData>::SpacedNdArray() = default;

    template <typename TInternalData>
    SpacedNdArray<TInternalData>::~SpacedNdArray() = default;

    template <typename TInternalData>
    void
        SpacedNdArray<TInternalData>::Initialize(const LengthType& arrayDimensions,
            const PositionType& stepSize,
            const PositionType& anchor)
    {
        Superclass::Initialize(arrayDimensions);

        itkAssertInDebugAndIgnoreInReleaseMacro(stepSize.GetSize() == m_Dimension);
        itkAssertInDebugAndIgnoreInReleaseMacro(anchor.GetSize() == m_Dimension);

        m_StepSize = stepSize;
        m_Anchor = anchor;
    }

    // Verify whether a given position is contained inside the set of valid accessors
    template <typename TInternalData>
    bool
        SpacedNdArray<TInternalData>::IsValidPosition(const PositionType& position) const
    {
        if (position.GetSize() != m_Dimension) {
            return false;
        }

        for (int i = 0; i < m_Dimension; i++) {
            // If the position is not a multiple of the step size in the given dimension then
            // it cannot be used to index into the array
            if (fmod(position[i] - m_Anchor[i], m_StepSize[i]) != 0) {
                return false;
            }

            // If the position is outside the bounded region then it cannot be used to index into the array
            if (position[i] < m_Anchor[i]) {
                return false;
            }
            if(position[i] >= m_Anchor[i] + m_StepSize[i] * m_DataLength[i]) {
                return false;
            }
        }

        return true;
    }

    template <typename TInternalData>
    void
        SpacedNdArray<TInternalData>::SetElement(const PositionType& position, const InternalDataType value)
    {
        LengthType arrayIndex;
        GetDiscreteIndexFromPosition(position, arrayIndex);

        if (arrayIndex.Size() != 0) {
            Superclass::SetElement(arrayIndex, value);
        }
        else {
            itkExceptionMacro(<< "Could not set element at invalid position!");
        }

    }

    template <typename TInternalData>
    typename SpacedNdArray<TInternalData>::InternalDataType
        SpacedNdArray<TInternalData>::GetElement(const PositionType& position) const
    {
        LengthType arrayIndex;
        GetDiscreteIndexFromPosition(position, arrayIndex);

        if (arrayIndex.Size() != 0) {
            return Superclass::GetElement(arrayIndex);
        }
        else {
            itkExceptionMacro(<< "Could not set element at invalid position!");
        }
    }

    template <typename TInternalData>
    void
        SpacedNdArray<TInternalData>::GetDiscreteIndexFromPosition(const PositionType& position, LengthType& index) const
    {
        if (!IsValidPosition(position)) return;

        index.SetSize(GetDimension());
        for (int dim = 0; dim < GetDimension(); dim++) {
            index[dim] = (position[dim] - m_Anchor[dim]) / m_StepSize[dim];
        }
    }
} // namespace

#endif // itkSpacedNdArray_hxx