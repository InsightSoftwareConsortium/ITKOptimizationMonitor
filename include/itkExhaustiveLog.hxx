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

#ifndef itkExhaustiveLog_hxx
#define itkExhaustiveLog_hxx

#include "itkExhaustiveLog.h"

#include "itkCommand.h"
#include "itkExhaustiveOptimizerv4.h"

/*
namespace itk
{
    template <typename ImageType>
    void
    ExhaustiveLog<ImageType>::Execute(itk::Object* caller, const itk::EventObject& event) override
    {
        Execute((const itk::Object*)caller, event);
    }

    template <typename ImageType>
    void 
    ExhaustiveLog<ImageType>::Execute(const itk::Object* caller, const itk::EventObject& event) override
    {
        if (!itk::ProgressEvent().CheckEvent(&event))
        {
            return;
        }
        auto optimizer = static_cast<const OptimizerType*>(object);
        if (!filter)
        {
            return;
        }

        OptimizerType::ParametersType index = optimizer->GetCurrentIndex();
        OptimizerType::MeasureType measure = optimizer->GetCurrentValue();
    }
} // namespace*/

#endif // itkExhaustiveLog_hxx