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
#include "itkImage.h"
#include "itkExhaustiveOptimizerv4.h"

namespace itk
{
    /**
     *\class CommandExhaustiveLog
     *  \brief Command class to collect data from itk::ExhaustiveOptimizerv4 class.
     *
     * This class accepts events fired by a Registration class making use of an
     * itk::ExhaustiveOptimizerv4 object and stores data in a corresponding
     * itk::Image for later access.
     *
     * The class is templated over the image pixel data type and image dimension.
     *
     * Template parameters for class CommandExhaustiveLog:
     *
     * - TInternalData = Element type stored at each location in the data image.
     * - TImageDimension = Dimension of image equal to number of transform parameters
     *
     */
    template <typename TInternalData, unsigned int TImageDimension>
    class CommandExhaustiveLog : public itk::Command
    {
    public:
        using Self = CommandExhaustiveLog;
        using Superclass = itk::Command;
        using Pointer = itk::SmartPointer<Self>;
        itkNewMacro(Self);

        /** Data type for pixel values in data image */
        using InternalDataType = TInternalData;
        /** Image dimension */
        static constexpr unsigned int Dimension = TImageDimension;

        // Only valid when used with ExhaustiveOptimizerv4 for now
        // TODO allow different ExhaustiveOptimzerv4 templates
        // TODO extend for other optimizers
        using OptimizerType = itk::ExhaustiveOptimizerv4<double>;
        // TODO add default fill
        //using constexpr FillValue = 0;

        /** N-dimensional array with positional aliasing. */
        using ImageType = itk::Image<InternalDataType, Dimension>;
        using ImagePointer = typename ImageType::Pointer;

        using SizeType = typename ImageType::SizeType;              // aka LengthType
        using SizeValueType = typename ImageType::SizeValueType;

        using IndexType = typename ImageType::IndexType;            // aka PositionType
        using SpacingType = typename ImageType::SpacingType;        // aka PositionType
        using PointType = typename ImageType::PointType;            // center/origin type

        using ParametersType = typename OptimizerType::ParametersType;

        void Execute(itk::Object* caller, const itk::EventObject& event) override;

        void Execute(const itk::Object* caller, const itk::EventObject& event) override;

        /** Retrieve a const reference to data point recorded during optimizer iteration. */
        const TInternalData GetData(const IndexType& index) const;
        const TInternalData GetData(const PointType& point) const;
        const TInternalData GetData(const ParametersType& parameters) const;

        /** Center of exhaustive region is used to compute image origin at initialization */
        itkSetMacro(Center, PointType);
        itkGetMacro(Center, PointType);
        itkGetConstMacro(DataImage, ImageType*);
        
        /** Provide const methods to access underlying image parameters */
        const SizeType GetDataSize() const { return m_DataImage->GetLargestPossibleRegion().GetSize(); }
        const SpacingType GetStepSize() const { return m_DataImage->GetSpacing(); }
        const PointType GetOrigin() const { return m_DataImage->GetOrigin(); }

        /** Get the actual length of the data array in the given dimension.  */
        SizeValueType GetDataSize(const int dim) const {
            return (dim < Dimension) ? m_DataImage->GetLargestPossibleRegion().GetSize()[dim] : 0;
        }

        /** Get the number of steps away from the center taken by the optimizer. */
        SizeValueType GetNumberOfSteps(const int dim) const {
            return (dim < Dimension) ? ((GetDataSize()[dim] - 1) / 2) : 0;
        }

    protected:
        CommandExhaustiveLog();
        ~CommandExhaustiveLog() override;

    private:
        /** Initialize members and data array on registration StartEvent. */
        void Initialize(const OptimizerType* optimizer);

        /** Set data at given position. */
        void SetData(const IndexType& index, const InternalDataType& value);
        void SetData(const ParametersType& index, const InternalDataType& value);

    private:
        /** Coordinates at center of optimization region; ex. (2.1, -1.05).
        *   Referenced at initialization to set image origin. */
        PointType m_Center;
        /** n-dimensional array with spacing to store exhaustive values */
        ImagePointer m_DataImage;
    };
} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCommandExhaustiveLog.hxx"
#endif

#endif // itkCommandExhaustiveLog_hxx
