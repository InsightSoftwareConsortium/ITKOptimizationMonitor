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
template <typename TValue, unsigned int TImageDimension>
CommandExhaustiveLog<TValue, TImageDimension>::CommandExhaustiveLog()
{
  m_Center = ParametersType();
  m_TransformToImageDimension = std::vector<int>();
  m_DataImage = nullptr;
};

template <typename TValue, unsigned int TImageDimension>
CommandExhaustiveLog<TValue, TImageDimension>::~CommandExhaustiveLog() = default;


template <typename TValue, unsigned int TImageDimension>
void
CommandExhaustiveLog<TValue, TImageDimension>::Execute(itk::Object * caller, const itk::EventObject & event)
{
  Execute((const itk::Object *)caller, event);
}

template <typename TValue, unsigned int TImageDimension>
void
CommandExhaustiveLog<TValue, TImageDimension>::Execute(const itk::Object *      caller,
                                                              const itk::EventObject & event)
{
  // Do nothing if event is not recognized
  if (!itk::StartEvent().CheckEvent(&event) && !itk::IterationEvent().CheckEvent(&event))
  {
    return;
  }
  else
  {

    auto optimizer = static_cast<const OptimizerType *>(caller);
    if (!optimizer)
    {
      return;
    }

    if (itk::StartEvent().CheckEvent(&event))
    {
      // Initialize at start via optimizer parameters
      Initialize(optimizer);
    }
    else
    {
      // Update iteration value
      OptimizerType::ParametersType index = optimizer->GetCurrentIndex();
      OptimizerType::MeasureType    value = optimizer->GetCurrentValue();
      SetValue(index, value);
    }
  }
}


template <typename TValue, unsigned int TImageDimension>
void
CommandExhaustiveLog<TValue, TImageDimension>::Initialize(const OptimizerType * optimizer)
{
  SizeType    size;
  PointType   origin;
  SpacingType spacing;

  // If fixed dimensions are not set then try initializing with no fixed dimensions
  SizeValueType TransformDimensionCount = optimizer->GetNumberOfSteps().GetSize();
  if (m_TransformToImageDimension.size() == 0)
  {
    ParametersType fixedDimensionMask = ParametersType();
    fixedDimensionMask.SetSize(TransformDimensionCount);
    for (int idx = 0; idx < TransformDimensionCount; idx++)
    {
      fixedDimensionMask[idx] = 0;
    }
    SetFixedDimensions(fixedDimensionMask);
  }

  if (m_TransformToImageDimension.size() != TransformDimensionCount)
  {
    throw "Dimension mismatch: fixed parameters not set properly!";
  }

  for (int transformDimension = 0; transformDimension < TransformDimensionCount; transformDimension++)
  {
    int imageDimension = TransformDimensionToImageDimension(transformDimension);
    if (imageDimension != -1)
    {
      // Variable dimensions
      // Compute region size along given dimension
      size[imageDimension] = optimizer->GetNumberOfSteps()[transformDimension] * 2 + 1;

      // Compute origin position in given dimension
      origin[imageDimension] = m_Center[transformDimension] -
                       optimizer->GetNumberOfSteps()[transformDimension] * optimizer->GetScales()[transformDimension];

      // Copy spacing to expected type
      spacing[imageDimension] = optimizer->GetScales()[transformDimension];

      imageDimension++;
    }
    else
    {
        // Fixed dimensions
      itkAssertInDebugAndIgnoreInReleaseMacro(optimizer->GetNumberOfSteps()[transformDimension] == 0);
    }
  }

  m_DataImage = ImageType::New();
  m_DataImage->SetRegions(size);
  m_DataImage->SetSpacing(spacing);
  m_DataImage->SetOrigin(origin);

  m_DataImage->Allocate();
}

template <typename TValue, unsigned int TImageDimension>
const TValue
CommandExhaustiveLog<TValue, TImageDimension>::GetValue(const IndexType & index) const
{
  return m_DataImage->GetPixel(index);
}

template <typename TValue, unsigned int TImageDimension>
const TValue
CommandExhaustiveLog<TValue, TImageDimension>::GetValue(const PointType & point) const
{
  IndexType index = m_DataImage->TransformPhysicalPointToIndex(point);
  return GetValue(index);
}

template <typename TValue, unsigned int TImageDimension>
const TValue
CommandExhaustiveLog<TValue, TImageDimension>::GetValue(const ParametersType & parameters) const
{
  PointType point;

  for (unsigned int transformDimension = 0; transformDimension < parameters.Size(); transformDimension++)
  {
    int imageDimension = TransformDimensionToImageDimension(transformDimension);
    if (imageDimension != -1)
    {
      point[imageDimension] = parameters[transformDimension];
    }
  }

  return GetValue(point);
}

template <typename TValue, unsigned int TImageDimension>
void
CommandExhaustiveLog<TValue, TImageDimension>::SetValue(const IndexType & index, const InternalDataType & value)
{
  m_DataImage->SetPixel(index, value);
}

template <typename TValue, unsigned int TImageDimension>
void
CommandExhaustiveLog<TValue, TImageDimension>::SetValue(const ParametersType &   index,
                                                              const InternalDataType & value)
{
  IndexType baseIndex;

  for (unsigned int transformDimension = 0; transformDimension < index.Size(); transformDimension++)
  {
    int imageDimension = TransformDimensionToImageDimension(transformDimension);
    if (imageDimension != -1)
    {
      baseIndex[imageDimension] = index[transformDimension];
    }
  }

  SetValue(baseIndex, value);
}

template <typename TValue, unsigned int TImageDimension>
void
CommandExhaustiveLog<TValue, TImageDimension>::SetFixedDimensions(const StepsType& value)
{
  const int VARIABLE_MASK_VALUE = 0;
  const int FIXED_MASK_VALUE = 1;

  // Verify the mask has the correct number of variable/fixed dimensions
  int expectedFixedCount = value.Size() - Dimension;

  // If dimensions match exactly then set all dimensions as variable
  if (expectedFixedCount == 0)
  {
    m_TransformToImageDimension.reserve(value.Size());
      for (int idx = 0; idx < value.Size(); idx++)
    {
        m_TransformToImageDimension.push_back(idx);
        m_ImageToTransformDimension[idx] = idx;
    }
    return;
  }

  // Verify number of variable/fixed dimensions in mask matches expectations
  for (auto step : value)
  {
    if (value[step] == FIXED_MASK_VALUE)
      step++;
  }
  itkAssertInDebugAndIgnoreInReleaseMacro(fixedCount == expectedFixedCount);

  m_TransformToImageDimension.reserve(value.Size());

  int imageDimension = 0;
  for (int transformDimension = 0; transformDimension < value.Size(); transformDimension++)
  {
    if (value[transformDimension] == 0) {
      m_ImageToTransformDimension[imageDimension] = transformDimension;
      m_TransformToImageDimension.push_back(imageDimension);
        imageDimension++;

        // Verify variable dimensions do not exceed internal image size
        if (imageDimension > Dimension)
        {
          throw ExceptionObject("Not enough fixed dimensions for observer of dimension " + std::to_string(Dimension));
        }
    }
    else
    {
      m_TransformToImageDimension.push_back(-1);
    }
  }
  // Verify variable dimensions exactly match internal image size
  if (imageDimension != Dimension)
  {
    throw ExceptionObject("Error: Too many fixed dimensions for observer of dimension " + std::to_string(Dimension));
  }
}


template <typename TValue, unsigned int TImageDimension>
int
CommandExhaustiveLog<TValue, TImageDimension>::TransformDimensionToImageDimension(const int dim) const
{
  if (dim >= m_TransformToImageDimension.size())
    return -1;

  return m_TransformToImageDimension[dim];
}

template <typename TValue, unsigned int TImageDimension>
int
CommandExhaustiveLog<TValue, TImageDimension>::ImageDimensionToTransformDimension(const int dim) const
{
  if (dim >= Dimension)
    return -1;

  return m_ImageToTransformDimension[dim];
}

} // namespace itk

#endif // itkCommandExhaustiveLog_hxx
