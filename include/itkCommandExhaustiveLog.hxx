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
    CommandExhaustiveLog<TInternalData>::~CommandExhaustiveLog()
    {
        delete m_Data;  // TODO does default handle this?
    }

    template <typename TInternalData>
    void 
        CommandExhaustiveLog<TInternalData>::Initialize(const OptimizerType* optimizer) 
    {
        m_Dimension = optimizer->GetNumberOfSteps().GetSize();

        m_NumberOfSteps = optimizer->GetNumberOfSteps();
        m_StepSize = optimizer->GetScales();    // TODO are scales == step size or inverted? check this math
        //m_Origin = itk::Array<TInternalData>(0); // TODO set origin location in data

        // Compute the size of the data array and initialize
        unsigned int elementCount = 1;
        for (auto stepCount : m_NumberOfSteps) {
            elementCount *= (stepCount + 1);
        }
        m_DataSize = elementCount;
        m_Data = new TInternalData[elementCount];
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
        if (!itk::StartEvent().CheckEvent(&event) && !itk::ProgressEvent().CheckEvent(&event))
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
                Initialize(optimizer);
            }
            else {
                //FIXME
                OptimizerType::ParametersType index = optimizer->GetCurrentIndex();
                OptimizerType::MeasureType measure = optimizer->GetCurrentValue();
            }
        }
    }
} // namespace

#endif // itkCommandExhaustiveLog_hxx
