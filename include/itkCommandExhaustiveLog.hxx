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
#include "itkExhaustiveOptimizerv4.h"


namespace itk
{
    template <typename TInternalData>
    CommandExhaustiveLog<TInternalData>::CommandExhaustiveLog() = default;

    template <typename TInternalData>
    CommandExhaustiveLog<TInternalData>::~CommandExhaustiveLog() = default;

    template <typename TInternalData>
    void 
        CommandExhaustiveLog<TInternalData>::Initialize(const OptimizerType* optimizer)
    {
        m_Dimension = optimizer->GetNumberOfSteps().GetSize();

        m_NumberOfSteps = optimizer->GetNumberOfSteps();
        m_StepSize = optimizer->GetScales();
        //m_Origin = itk::Array<TInternalData>(0); // TODO set origin location in data

        // Compute the size of the data array and initialize
        unsigned int elementCount = 1;
        for (auto stepCount : m_NumberOfSteps) {
            elementCount *= (stepCount * 2 + 1);
        }
        m_DataSize = elementCount;
        m_Data = new TInternalData[elementCount]();

    }

    template <typename TInternalData>
    void
        CommandExhaustiveLog<TInternalData>::SetDataElement(const StepsSizeType& position, const InternalDataType& value) 
    {
        auto dataIndex = GetInternalIndex(position);
        m_Data[dataIndex] = value;
    }
    template <typename TInternalData>
    const typename CommandExhaustiveLog<TInternalData>::StepsType&
        CommandExhaustiveLog<TInternalData>::GetIndexFromPosition(const StepsSizeType position) {
        // TODO account for bad positions, division by zero, etc
        StepsType indices;
        indices.SetSize(m_Dimension);

        for (unsigned int i = 0; i < position.GetSize(); i++) {
            std::cout << position.GetSize() << m_Center.GetSize() << m_StepSize.GetSize() << m_NumberOfSteps.GetSize();
            indices.SetElement(i, (position.GetElement(i) - m_Center.GetElement(i)) / m_StepSize.GetElement(i) + m_NumberOfSteps.GetElement(i));
        }

        return indices;
    }

    // An n-dimensional array of size n1 x n2 x ... x ni
    // accessed at position [a1][a2]...[ai]
    // can be represented as a 1D array of length n1 * n2 * ... * ni
    // accessed at position ai + a(i-1) * ni + a(i-2) * ni * n(i-1) + ... + a1 * [ni * n(i-1) * ... * n2]
    template <typename TInternalData>
    const size_t
        CommandExhaustiveLog<TInternalData>::GetInternalIndex(const StepsSizeType& position) const {
        size_t area = 1;
        size_t dataIndex = 0;

        for (int idx = position.GetSize() - 1; idx >= 0; idx--) {
            dataIndex += position.GetElement(idx) * area;
            area *= (this->GetNumberOfSteps().GetElement(idx) + 1);
        }

        return dataIndex;
    }

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
} // namespace

#endif // itkCommandExhaustiveLog_hxx
