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
        using ArrayPointer = typename ArrayType::Pointer;
        using LengthType = typename ArrayType::LengthType;
        using PositionType = typename ArrayType::PositionType;

        void Execute(itk::Object* caller, const itk::EventObject& event) override;

        void Execute(const itk::Object* caller, const itk::EventObject& event) override;

        void Initialize(const OptimizerType* optimizer);

        TInternalData GetDataAtPosition(const PositionType& position);

        // TODO
        // Get itk::Array as a copy of a slice of the collected data
        void GetDataSlice2D(const PositionType& position, const std::vector<bool>& dimIsVariable, itk::Array2D<InternalDataType>& slice);

        // TODO refactor to reference array
        itkSetMacro(Center, PositionType);
        itkGetMacro(Center, PositionType);

        const ArrayType* GetData() const { return m_DataArray.Get(); }
        const itk::SizeValueType GetDimension() const { return m_DataArray->GetDimension(); }
        const itk::SizeValueType GetDataSize() const { return m_DataArray->GetDataSize(); }
        const LengthType GetDataLength() const { return m_DataArray->GetDataLength(); }
        const PositionType GetStepSize() const { return m_DataArray->GetStepSize(); }
        const PositionType GetAnchor() const { return m_DataArray->GetAnchor(); }

        itk::SizeValueType GetDataLength(const int dim) const {
            return (dim < GetDimension()) ? GetDataLength()[dim] : 0;
        }

        itk::SizeValueType GetNumberOfSteps(const int dim) const {
            return (dim < GetDimension()) ? ((GetDataLength()[dim] - 1) / 2) : 0;
        }

    protected:
        CommandExhaustiveLog();
        ~CommandExhaustiveLog() override;

    private:
        // Set data at discrete index
        void SetDataElement(const PositionType& position, const InternalDataType& value);

        //// TODO
        //// Translate from image space into discrete nd array indices
        //const void GetNdIndexFromPosition(const PositionType& position, LengthType& ndIndex);

        //// TODO
        //// Translate vector of indices into index for 1D data array
        //const size_t GetInternalIndex(const LengthType& indices) const;

    private:
        PositionType m_Center;     // coordinates at center of optimization region; ex. (2.1, -1.05)
        ArrayPointer m_DataArray;      // n-dimensional array with spacing to store exhaustive values
    };
} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCommandExhaustiveLog.hxx"
#endif

#endif // itkCommandExhaustiveLog_hxx
