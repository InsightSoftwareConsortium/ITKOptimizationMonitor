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
    /**
     *\class CommandExhaustiveLog
     *  \brief Command class to collect data from itk::ExhaustiveOptimizerv4 class.
     *
     * This class accepts events fired by a Registration class making use of an
     * itk::ExhaustiveOptimizerv4 object and stores data in a corresponding
     * SpacedNdArray for later access. SpacedNdArray parameters are as follows:
     * 
     * - Dimension is equal to the length of the Transform parameter vector in order
     *   to reconstruct the parametric surface as the optimizer iteratively steps
     *   over transform values;
     * - Array length along each dimension is equal to the number of twice the number
     *   of optimization steps in each dimension plus one. This corresponds to the
     *   ExhaustiveOptimizer's behavior in computing the metric value at the number
     *   of steps away from the initial transform in each direction.
     * - Step sizes (spacings) are set equivalent to those in the optimizer.
     * - The position of the anchor point at the first element in the array is
     *   computed from the initial transform representing the domain center of
     *   optimization, the number of steps, and the step size.
     *
     * The class is templated over the type of the elements.
     *
     * Template parameters for class CommandExhaustiveLog:
     *
     * - TInternalData = Element type stored at each location in the SpacedNdArray.
     *
     */
    template <typename TInternalData>
    class CommandExhaustiveLog : public itk::Command
    {
    public:
        using Self = CommandExhaustiveLog;
        using Superclass = itk::Command;
        using Pointer = itk::SmartPointer<Self>;
        itkNewMacro(Self);

        /** Data type for values stored in SpacedNdArray */
        using InternalDataType = TInternalData;

        // Only valid when used with ExhaustiveOptimizerv4 for now
        // TODO allow different ExhaustiveOptimzerv4 templates
        using OptimizerType = itk::ExhaustiveOptimizerv4<double>;
        // TODO extend for other optimizers with a fill value
        //using constexpr FillValue = 0;

        /** N-dimensional array with positional aliasing. */
        using ArrayType = itk::SpacedNdArray<InternalDataType>;
        using ArrayPointer = typename ArrayType::Pointer;
        using LengthType = typename ArrayType::LengthType;
        using PositionType = typename ArrayType::PositionType;

        void Execute(itk::Object* caller, const itk::EventObject& event) override;

        void Execute(const itk::Object* caller, const itk::EventObject& event) override;

        /** Retrieve a const reference to data recorded during optimizer iteration. */
        TInternalData GetDataAtPosition(const PositionType& position);

        /** Get itk::Array as a copy of a slice of the collected data. */
        void GetDataSlice2D(const PositionType& position, const std::vector<bool>& dimIsVariable, itk::Array2D<InternalDataType>& slice);

        // TODO refactor to reference array
        // Center is not const exposed in ExhaustiveOptimizerv4 so have to set before initialization
        itkSetMacro(Center, PositionType);
        itkGetMacro(Center, PositionType);
        
        // Provide const methods to access underlying SpacedNdArray parameters
        const ArrayType* GetData() const { return m_DataArray.Get(); }
        const itk::SizeValueType GetDimension() const { return m_DataArray->GetDimension(); }
        const itk::SizeValueType GetDataSize() const { return m_DataArray->GetDataSize(); }
        const LengthType GetDataLength() const { return m_DataArray->GetDataLength(); }
        const PositionType GetStepSize() const { return m_DataArray->GetStepSize(); }
        const PositionType GetAnchor() const { return m_DataArray->GetAnchor(); }

        /** Get the actual length of the data array in the given dimension.  */
        itk::SizeValueType GetDataLength(const int dim) const {
            return (dim < GetDimension()) ? GetDataLength()[dim] : 0;
        }

        /** Get the number of steps away from the center taken by the optimizer. */
        itk::SizeValueType GetNumberOfSteps(const int dim) const {
            return (dim < GetDimension()) ? ((GetDataLength()[dim] - 1) / 2) : 0;
        }

    protected:
        CommandExhaustiveLog();
        ~CommandExhaustiveLog() override;

    private:
        /** Initialize members and data array on registration StartEvent. */
        void Initialize(const OptimizerType* optimizer);

        /** Set data at given position. */
        void SetDataElement(const PositionType& position, const InternalDataType& value);

    private:
        /** coordinates at center of optimization region; ex. (2.1, -1.05) */
        PositionType m_Center;
        /** n-dimensional array with spacing to store exhaustive values */
        ArrayPointer m_DataArray;
    };
} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCommandExhaustiveLog.hxx"
#endif

#endif // itkCommandExhaustiveLog_hxx
