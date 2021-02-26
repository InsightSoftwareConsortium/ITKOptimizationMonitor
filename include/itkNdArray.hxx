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

#ifndef itkNdArray_hxx
#define itkNdArray_hxx

#include "itkNdArray.h"

#include "itkIntTypes.h"

namespace itk
{
    template <typename TInternalData>
    NdArray<TInternalData>::NdArray()
    {
        m_Dimension = 0;
        m_DataSize = 0;
        m_DataLength = LengthType();
        m_Data = nullptr;
    };

    template <typename TInternalData>
    NdArray<TInternalData>::NdArray(const LengthType& arrayDimensions)
    {
        Initialize(arrayDimensions);
    }

    template <typename TInternalData>
    NdArray<TInternalData>::~NdArray() = default;

    /** Allow array to be re-initialized at any time. 
     * Previous memory allocation and any data contained therein are discarded. */
    template <typename TInternalData>
    void
        NdArray<TInternalData>::Initialize(const LengthType& arrayDimensions) 
    {
        m_Dimension = arrayDimensions.GetSize();
        m_DataLength.SetSize(m_Dimension);
        m_DataLength = arrayDimensions;
        
        SizeValueType arraySize = arrayDimensions[0];
        for (unsigned int dim = 1; dim < m_Dimension; dim++) {
            arraySize *= arrayDimensions[dim];
        }
        m_DataSize = arraySize;

        // Deallocate memory if previously initialized
        if (m_Data) {
            delete m_Data;
        }

        m_Data = new InternalDataType[m_DataSize]();
    };

    template <typename TInternalData>
    void
        NdArray<TInternalData>::SetElement(const LengthType& ndIndex, InternalDataType value)
    {
        SizeValueType dataIndex = Get1DIndexFromIndexList(ndIndex);
        m_Data[dataIndex] = value;
    };

    template <typename TInternalData>
    typename NdArray<TInternalData>::InternalDataType
        NdArray<TInternalData>::GetElement(const LengthType& ndIndex) const
    {
        SizeValueType dataIndex = Get1DIndexFromIndexList(ndIndex);
        return m_Data[dataIndex];
    };


    /** An n-dimensional array of size n1 x n2 x ... x ni
     *  accessed at position [a1][a2]...[ai]
     * can be represented as a 1D array of length n1 * n2 * ... * ni
     * accessed at position ai + a(i-1) * ni + a(i-2) * ni * n(i-1) + ... + a1 * [ni * n(i-1) * ... * n2]
     */
    template <typename TInternalData>
    typename itk::SizeValueType
        NdArray<TInternalData>::Get1DIndexFromIndexList(const LengthType& ndIndex) const
    {
        SizeValueType area = 1;
        SizeValueType dataIndex = 0;

        for (int idx = ndIndex.GetSize() - 1; idx >= 0; idx--)
        {
            // Verify steps are within bounds
            if (ndIndex[idx] < m_DataLength[idx])
            {
                // Update index into 1D data array
                dataIndex += ndIndex[idx] * area;
                // Update multiplier for next term
                area *= m_DataLength[idx];
            }
            else
            {
                itkExceptionMacro(<< " attempted to access data outside the array region!");
            }
        }

        return dataIndex;
    }



} // namespace

#endif // itkNdArray_hxx