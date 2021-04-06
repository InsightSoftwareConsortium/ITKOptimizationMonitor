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
    template <typename TInternalData>
    class NdArray : public itk::LightObject
    {
    public:
        using Self = NdArray;
        using Superclass = itk::LightObject;
        using Pointer = itk::SmartPointer<Self>;
        itkNewMacro(Self);

        using InternalDataType = TInternalData;

        using LengthType = itk::Array<itk::SizeValueType>;

        itkGetConstReferenceMacro(Dimension, itk::SizeValueType);
        itkGetConstReferenceMacro(DataSize, itk::SizeValueType);
        itkGetConstReferenceMacro(DataLength, LengthType);
        itkGetMacro(Data, InternalDataType*);   // TODO make const

        // Initialize array in memory
        void Initialize(const LengthType& arrayDimensions);

        void SetElement(const LengthType& ndIndex, InternalDataType value);
        InternalDataType& GetElement(const LengthType& ndIndex) const;

    protected:
        NdArray();
        ~NdArray() override;

    private:
        itk::SizeValueType Get1DIndexFromIndexList(const LengthType& ndIndex) const;

    private:
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