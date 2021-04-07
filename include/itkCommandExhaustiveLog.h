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
#include "itkArray2D.h"
#include "itkTransform.h"
#include "itkExhaustiveOptimizerv4.h"

#include "itkSpacedNdArray.h"

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

        using ArrayType = itk::SpacedNdArray<InternalDataType>; // n-dimensional array with positional aliases for indices
        using LengthType = ArrayType::LengthType;
        using PositionType = ArrayType::PositionType;

        using StepsSizeType = itk::Array<InternalDataType>; // compatible with floating point positions
        using StepsType = Array<SizeValueType>;             // integer-valued array indices

        void Execute(itk::Object* caller, const itk::EventObject& event) override;

        void Execute(const itk::Object* caller, const itk::EventObject& event) override;

        void Initialize(const OptimizerType* optimizer);

        TInternalData GetDataAtPosition(const StepsSizeType& position);

        // Get itk::Array as a copy of a slice of the collected data
        void GetDataSlice2D(const StepsSizeType& position, const std::vector<bool>& dimIsVariable, itk::Array2D<InternalDataType>& slice);

        // Get length of given side of n-dimensional data array
        size_t GetDataLength(const int dim) const {
            return (dim > m_NumberOfSteps.GetSize()) ? 0 : m_NumberOfSteps[dim] * 2 + 1;
        }

        itkGetConstReferenceMacro(Dimension, size_t);
        itkGetConstReferenceMacro(StepSize, StepsSizeType);
        itkGetConstReferenceMacro(NumberOfSteps, StepsType);
        itkSetMacro(Center, StepsSizeType);
        itkGetMacro(Center, StepsSizeType);
        itkGetConstReferenceMacro(DataSize, itk::SizeValueType);
        itkGetMacro(Data, InternalDataType*);

    protected:
        CommandExhaustiveLog();
        ~CommandExhaustiveLog() override;

    private:
        // Set data at discrete index
        void SetDataElement(const StepsType& indices, const InternalDataType& value);  

        // Translate from image space into discrete nd array indices
        const void GetNdIndexFromPosition(const StepsSizeType position, StepsType& ndIndex);  

        // Translate vector of indices into index for 1D data array
        const size_t GetInternalIndex(const StepsType& indices) const;      

    private:
        size_t m_Dimension;      // # of parameters in transform to vary

        StepsSizeType m_StepSize; // size of a single step in a given dimension; ex. {1, 5, 0.1}
        StepsType m_NumberOfSteps; // number of steps to include in a given direction; ex. {10, 0, 5}
        StepsSizeType m_Center;     // coordinates at center of optimization region; ex. (2.1, -1.05)

        itk::SizeValueType m_DataSize;    // Total number of elements in array
        InternalDataType* m_Data;     // n-dimensional array to store exhaustive values
    };
} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCommandExhaustiveLog.hxx"
#endif

#endif // itkCommandExhaustiveLog_hxx
