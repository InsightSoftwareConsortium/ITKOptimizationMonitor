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
    /**
     *\class SpacedNdArray
     *  \brief N-dimensional class with data spacing.
     *
     * This class extends the NdArray class to define array access at
     * discrete rational numbers. The first element in the array is "anchored"
     * at a given point in space and access to subsequent elements are defined
     * via whole number multiples of fixed step size. 
     *
     * Elements in the array are accessed externally with a list of parameters
     * corresponding to the desired discrete position in space. Indices must
     * precisely correspond to a discrete index position for access to succeed.
     *
     * The class is templated over the type of the elements.
     *
     * Template parameters for class SpacedNdArray:
     *
     * - TInternalData = Element type stored at each location in the array.
     *
     */
    template <typename TInternalData>
    class SpacedNdArray : public itk::NdArray<TInternalData>
    {
    public:
        using Self = SpacedNdArray;
        using Superclass = itk::NdArray<TInternalData>;
        using Pointer = itk::SmartPointer<Self>;
        itkNewMacro(Self);

        /** The element type stored at each location in the NdArray. */
        using InternalDataType = Superclass::InternalDataType;

        /** List of whole number values for properties such as
         *  array side lengths. */
        using LengthType = Superclass::LengthType;

        /** List of rational number values for properties such as
         *  position accessor values. */
        using PositionType = itk::Array<double>;

        itkGetConstReferenceMacro(StepSize, PositionType);
        itkGetConstReferenceMacro(Anchor, PositionType);

        /** Destructively initialize the array to the given dimensions. */
        void Initialize(const LengthType& arrayDimensions,
            const PositionType& stepSize,
            const PositionType& anchor);

        /** Check whether a given position is a valid accessor for the array.
         *  The discrete set of valid array accessors is defined as those
         *  corresponding to steps between the minimum and maximum array values, 
         *  i.e. S = {a[i] + n * d[i] for 0 <= n < steps[i] for 0 <= i <= length[i]}
         */
        bool IsValidPosition(const PositionType& position) const;

        /** Set value at the given position of n dimension. */
        void SetElement(const PositionType& position, const InternalDataType value);

        /** Retrieve value at the given index of n dimension. */
        InternalDataType GetElement(const PositionType& position) const;

    protected:
        /** Default constructor creates empty 0-d array and must be
         * manually initialized  later. */
        SpacedNdArray();
        /** Overloaded constructor initializes n-d array from given side lengths. */
        SpacedNdArray(const LengthType& arrayDimensions,
            const PositionType& stepSize,
            const PositionType& anchor);
        /** Default destructor. */
        ~SpacedNdArray() override;

    private:
        void GetNdArrayIndexFromPosition(const PositionType& position, LengthType& index) const;

    private:
        PositionType m_StepSize;
        PositionType m_Anchor;        // Position in continuous space of array element at [0; 0; ... 0]
    };
} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSpacedNdArray.hxx"
#endif

#endif // itkSpacedNdArray_h