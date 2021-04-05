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
#ifndef itkCommandExhaustiveLog_h
#define itkCommandExhaustiveLog_h

#include "itkMacro.h"
#include "itkCommand.h"
#include "itkTransform.h"
#include "itkExhaustiveOptimizerv4.h"

namespace itk
{
    template <typename TInternalData>
    class CommandExhaustiveLog : public itk::Command
    {
    public:
        using Self = CommandExhaustiveLog;
        using Superclass = itk::Command;
        using Pointer = itk::SmartPointer<Self>;
        itkNewMacro(Self);

        using InternalDataType = TInternalData;

        // Only valid when used with ExhaustiveOptimizerv4 for now
        // TODO allow different ExhaustiveOptimzerv4 templates
        using OptimizerType = itk::ExhaustiveOptimizerv4<double>;
        // TODO extend for other optimizers with a fill value
        //using constexpr FillValue = 0;

        using StepsSizeType = itk::Array<InternalDataType>;
        using StepsType = OptimizerType::StepsType;

        void Execute(itk::Object* caller, const itk::EventObject& event) override;

        void Execute(const itk::Object* caller, const itk::EventObject& event) override;

        void Initialize(const OptimizerType* optimizer);

        itkGetConstReferenceMacro(Dimension, size_t);
        itkGetConstReferenceMacro(StepSize, StepsSizeType);
        itkGetConstReferenceMacro(NumberOfSteps, StepsType);
        itkGetConstReferenceMacro(Origin, StepsSizeType);
        itkGetConstReferenceMacro(DataSize, itk::SizeValueType);
        itkGetMacro(Data, InternalDataType*);

    protected:
        CommandExhaustiveLog();
        ~CommandExhaustiveLog() override;
    private:
        size_t m_Dimension;      // # of parameters in transform to vary

        StepsSizeType m_StepSize; // size of a single step in a given dimension
        StepsType m_NumberOfSteps; // number of steps to include in a given direction
        StepsSizeType m_Origin;   // position of origin in array coordinates

        itk::SizeValueType m_DataSize;    // Total number of elements in array
        InternalDataType* m_Data;     // n-dimensional array to store exhaustive values
    };
} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCommandExhaustiveLog.hxx"
#endif

#endif // itkCommandExhaustiveLog_hxx
