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

#ifndef itkCommandExhaustiveLog_hxx
#define itkCommandExhaustiveLog_hxx

#include "itkCommandExhaustiveLog.h"

#include "itkCommand.h"
#include "itkArray2D.h"
#include "itkExhaustiveOptimizerv4.h"


namespace itk
{
    template <typename TInternalData>
    CommandExhaustiveLog<TInternalData>::CommandExhaustiveLog() = default;

    template <typename TInternalData>
    CommandExhaustiveLog<TInternalData>::~CommandExhaustiveLog() = default;


    template <typename TInternalData>
    void
        CommandExhaustiveLog<TInternalData>::Execute(itk::Object* caller, const itk::EventObject& event)
    {
        Execute((const itk::Object*)caller, event);
    }

    template <typename TInternalData>
    void
        CommandExhaustiveLog<TInternalData>::Execute(const itk::Object* caller, const itk::EventObject& event)
    {
        // Do nothing if event is not recognized
        if (!itk::StartEvent().CheckEvent(&event) && !itk::IterationEvent().CheckEvent(&event))
        {
            return;
        }
        else {

            auto optimizer = static_cast<const OptimizerType*>(caller);
            if (!optimizer)
            {
                return;
            }

            if (itk::StartEvent().CheckEvent(&event)) {
                // Initialize at start via optimizer parameters
                Initialize(optimizer);
            }
            else {
                // Update iteration value
                OptimizerType::ParametersType index = optimizer->GetCurrentIndex();
                OptimizerType::MeasureType value = optimizer->GetCurrentValue();
                SetDataElement(index, value);
            }
        }
    }


    template <typename TInternalData>
    void
        CommandExhaustiveLog<TInternalData>::Initialize(const OptimizerType* optimizer)
    {
        m_Dimension = optimizer->GetNumberOfSteps().GetSize();
        m_NumberOfSteps = optimizer->GetNumberOfSteps();
        m_StepSize = optimizer->GetScales();
        //m_Origin = itk::Array<TInternalData>(0); // TODO set origin location in data

        // Compute the size of the data array and initialize
        unsigned int elementCount = GetDataLength(0);
        for (unsigned int dim = 1; dim < m_Dimension; dim++) {
            elementCount *= GetDataLength(dim);
        }
        m_DataSize = elementCount;
        m_Data = new TInternalData[elementCount]();
    }

    template <typename TInternalData>
    TInternalData
        CommandExhaustiveLog<TInternalData>::GetDataAtPosition(const StepsSizeType& position)
    {
        StepsType ndIndex;
        GetNdIndexFromPosition(position, ndIndex);
        auto internalIndex = GetInternalIndex(ndIndex);

        return m_Data[internalIndex];
    }



    // Copy fragmented data across dimensions into a single contiguous array
    template <typename TInternalData>
    void 
        CommandExhaustiveLog<TInternalData>::GetDataSlice2D(const StepsSizeType& position, 
            const std::vector<bool>& dimIsVariable, 
            itk::Array2D<InternalDataType>& slice)
    {
        StepsSizeType pos;
        pos.SetSize(position.GetSize());

        std::vector<int> variableDims;
        for (int i = 0; i < dimIsVariable.size(); i++) {
            if (dimIsVariable[i]) {
                variableDims.push_back(i);
                pos[i] = m_Center[i];
            }
            else {
                pos[i] = position[i];
            }
        }

        // Verify 2D slice
        if (variableDims.size() > 2) {
            itkExceptionMacro(<< "Tried to vary more than 2 dimensions along slice!");
        }

        // Initialize output
        int rowDim = variableDims[0];
        int colDim = variableDims[1];

        size_t rowLength = GetDataLength(rowDim);
        size_t colLength = GetDataLength(colDim);
        slice.SetSize(rowLength, colLength);

        // Iteratively update output
        // TODO update position rather than ndIndex
        for (int r = 0; r < rowLength; r++) {
            for (int c = 0; c < colLength; c++) {
                StepsType ndIndex;
                GetNdIndexFromPosition(pos, ndIndex);

                ndIndex[rowDim] = r;
                ndIndex[colDim] = c;

                int dataIndex = GetInternalIndex(ndIndex);
                slice.SetElement(r, c, m_Data[dataIndex]);
            }
        }
    }



    template <typename TInternalData>
    void
        CommandExhaustiveLog<TInternalData>::SetDataElement(const StepsType& indices, const InternalDataType& value)
    {
        auto dataIndex = GetInternalIndex(indices);
        m_Data[dataIndex] = value;
    }

    template <typename TInternalData>
    const void
        CommandExhaustiveLog<TInternalData>::GetNdIndexFromPosition(const StepsSizeType position, StepsType& ndIndex) {

        // Verify position is of same dimension as data
        if (position.GetSize() != m_Dimension) {
            itkExceptionMacro(<< "Attempted to access data of dimension " << m_Dimension << " at " << position.GetSize() << "D location");
        }

        // Verify center is initialized (must be set prior to Initialize() call)
        if (m_Center.GetSize() != m_Dimension) {
            itkExceptionMacro(<< "Could not get nd index because data center is not initialized!");

        }

        // Verify step size is positive to guard against divide-by-zero errors
        for (auto stepSize : m_StepSize) {
            if (stepSize <= 0) {
                itkExceptionMacro(<< "At least one step size is not positive");
            }
        }

        // Get index in nth dimension as n[i] + (x[i] - c[i]) / d[i]
        ndIndex.SetSize(m_Dimension);
        for (unsigned int i = 0; i < position.GetSize(); i++) {
            ndIndex.SetElement(i, (position[i] - m_Center[i]) / m_StepSize[i] + m_NumberOfSteps[i]);
        }
    }

    // An n-dimensional array of size n1 x n2 x ... x ni
    // accessed at position [a1][a2]...[ai]
    // can be represented as a 1D array of length n1 * n2 * ... * ni
    // accessed at position ai + a(i-1) * ni + a(i-2) * ni * n(i-1) + ... + a1 * [ni * n(i-1) * ... * n2]
    template <typename TInternalData>
    const size_t
        CommandExhaustiveLog<TInternalData>::GetInternalIndex(const StepsType& ndIndex) const {
        size_t area = 1;
        size_t dataIndex = 0;

        for (int idx = ndIndex.GetSize() - 1; idx >= 0; idx--) 
        {
            // Verify steps are within bounds
            if (ndIndex[idx] < GetDataLength(idx)) 
            {
                // Update index into 1D data array
                dataIndex += ndIndex[idx] * area;
                // Update multiplier for next term
                area *= (m_NumberOfSteps[idx] * 2 + 1);
            }
            else 
            {
                itkExceptionMacro(<< " attempted to access data outside the exhaustive region!");
            }
        }

        return dataIndex;
    }
} // namespace

#endif // itkCommandExhaustiveLog_hxx
