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

#ifndef itkNdArray_h
#define itkNdArray_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"

#include "itkArray.h"

namespace itk
{
    /**
     *\class NdArray
     *  \brief N-dimensional array class with size defined at construction time.
     *
     * This class derives from the LightObject class.
     * Its size is assigned at construction time (run time) and can
     * not be changed afterwards.
     * 
     * Elements in the array are accessed externally with a list of parameters
     * corresponding to the desired index along each dimension.
     *
     * The class is templated over the type of the elements.
     *
     * Template parameters for class NdArray:
     *
     * - TInternalData = Element type stored at each location in the array.
     *
     */
    template <typename TInternalData>
    class NdArray : public itk::LightObject
    {
    public:
        using Self = NdArray;
        using Superclass = itk::LightObject;
        using Pointer = itk::SmartPointer<Self>;
        itkNewMacro(Self);

        /** The element type stored at each location in the NdArray. */
        using InternalDataType = TInternalData;

        /** List of whole number values for properties such as 
         *  array side lengths and index accessor values. */
        using LengthType = itk::Array<itk::SizeValueType>;

        itkGetConstReferenceMacro(Dimension, itk::SizeValueType);
        itkGetConstReferenceMacro(DataSize, itk::SizeValueType);
        itkGetConstReferenceMacro(DataLength, LengthType);
        itkGetMacro(Data, InternalDataType*);   // TODO make const

        /** Destructively initialize the array to the given dimensions. */
        void Initialize(const LengthType& arrayDimensions);

        /** Set value at the given index of n dimension. */
        void SetElement(const LengthType& ndIndex, InternalDataType value);

        /** Retrieve value at the given index of n dimension. */
        InternalDataType GetElement(const LengthType& ndIndex) const;

    protected:
        /** Default constructor creates empty 0-d array and must be
         * manually initialized  later. */
        NdArray();
        /** Overloaded constructor initializes n-d array from given side lengths. */
        NdArray(const LengthType& arrayDimensions);
        /** Default destructor. */
        ~NdArray() override;

    private:
        itk::SizeValueType Get1DIndexFromIndexList(const LengthType& ndIndex) const;

    protected:
        itk::SizeValueType m_Dimension;     // Number of array dimensions
        itk::SizeValueType m_DataSize;    // Total number of elements in array
        LengthType m_DataLength;        // Length of each array dimension
        InternalDataType* m_Data;     // n-dimensional array to store exhaustive values
    };

} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNdArray.hxx"
#endif

#endif // itkNdArray_h