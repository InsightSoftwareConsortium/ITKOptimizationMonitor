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
    template <typename TInternalData, unsigned int TImageDimension>
    CommandExhaustiveLog<TInternalData, TImageDimension>::CommandExhaustiveLog()
    {
        m_Center = PointType();
        m_DataImage = nullptr;
    };

    template <typename TInternalData, unsigned int TImageDimension>
    CommandExhaustiveLog<TInternalData, TImageDimension>::~CommandExhaustiveLog() = default;


    template <typename TInternalData, unsigned int TImageDimension>
    void
        CommandExhaustiveLog<TInternalData, TImageDimension>::Execute(itk::Object* caller, const itk::EventObject& event)
    {
        Execute((const itk::Object*)caller, event);
    }

    template <typename TInternalData, unsigned int TImageDimension>
    void
        CommandExhaustiveLog<TInternalData, TImageDimension>::Execute(const itk::Object* caller, const itk::EventObject& event)
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
                SetData(index, value);
            }
        }
    }


    template <typename TInternalData, unsigned int TImageDimension>
    void
        CommandExhaustiveLog<TInternalData, TImageDimension>::Initialize(const OptimizerType* optimizer)
    {
        SizeType size;
        PointType origin;
        SpacingType spacing;

        for (int dim = 0; dim < Dimension; dim++) {
            // Compute region size along given dimension
            size[dim] = optimizer->GetNumberOfSteps()[dim] * 2 + 1;

            // Compute origin position in given dimension
            origin[dim] = m_Center[dim] - optimizer->GetNumberOfSteps()[dim] * optimizer->GetScales()[dim];

            // Copy spacing to expected type
            spacing[dim] = optimizer->GetScales()[dim];
        }

        m_DataImage = ImageType::New();
        m_DataImage->SetRegions(size);
        m_DataImage->SetSpacing(spacing);
        m_DataImage->SetOrigin(origin);

        m_DataImage->Allocate();
    }

    template <typename TInternalData, unsigned int TImageDimension>
    const TInternalData
        CommandExhaustiveLog<TInternalData, TImageDimension>::GetData(const IndexType& index) const
    {
        return m_DataImage->GetPixel(index);
    }

    template <typename TInternalData, unsigned int TImageDimension>
    const TInternalData
        CommandExhaustiveLog<TInternalData, TImageDimension>::GetData(const PointType& point) const
    {
        IndexType index = m_DataImage->TransformPhysicalPointToIndex(point);
        return GetData(index);
    }

    template <typename TInternalData, unsigned int TImageDimension>
    const TInternalData
        CommandExhaustiveLog<TInternalData, TImageDimension>::GetData(const ParametersType& parameters) const
    {
        PointType point;

        for (unsigned int dim = 0; dim < Dimension; dim++) 
        {
            point[dim] = parameters[dim];
        }

        return GetData(point);
    }

    template <typename TInternalData, unsigned int TImageDimension>
    void
        CommandExhaustiveLog<TInternalData, TImageDimension>::SetData(const IndexType& index, const InternalDataType& value)
    {
        m_DataImage->SetPixel(index, value);
    }

    template <typename TInternalData, unsigned int TImageDimension>
    void 
        CommandExhaustiveLog<TInternalData, TImageDimension>::SetData(const ParametersType& index, const InternalDataType& value)
    {
        IndexType baseIndex;
        for (unsigned int i = 0; i < index.Size(); i++)
        {
            baseIndex[i] = index[i];
        }

        SetData(baseIndex, value);
    }

} // namespace

#endif // itkCommandExhaustiveLog_hxx
