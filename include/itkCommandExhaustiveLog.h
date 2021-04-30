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
 *  \brief Monitors ExhaustiveOptimizerv4 and provides the search space as an image.
 *
 * This class is intended to assist a user in evaluating optimization performance
 * over some transform search space. An itk::CommandExhaustiveLog object may be added
 * to consume event data from an itk::ExhaustiveOptimizerv4 object as the optimizer
 * iterates over a pre-set domain of transform parameters and returns a metric
 * value at each point. The CommandExhaustiveLog observer object logs each
 * data point internally in an itk::Image object so that the parametric surface
 * is available after optimization concludes.
 *
 * A CommandExhaustiveLog object paired with an ExhaustiveOptimizerv4 may be
 * useful for obtaining data to plan future optimization attempts, such as whether
 * a region appears rough or curved in a certain way that may render gradient descent
 * less effective. Exhaustive data may also help inform the user in regards to
 * the performance of ongoing optimization, such as overlaying optimizer steps
 * onto an image of the exhaustive parametric region in order to determine whether
 * the optimizer learning rate is appropriate for the region.
 *
 * Wrapping for this class is limited to available wrappings for the internal
 * itk::Image data array. Users wishing to make use of transforms with large
 * parameter lists may find it necessary to build ITK with custom wrappings to
 * meet their needs.
 *
 * Template parameters for class CommandExhaustiveLog:
 *
 * - TValue = Element type stored at each location in the data image.
 * - TImageDimension = Dimension of image equal to number of transform parameters
 *
 * \ingroup ITKOptimizationMonitor
 */
template <typename TValue, unsigned int TImageDimension>
class CommandExhaustiveLog : public itk::Command
{
public:
  using Self = CommandExhaustiveLog;
  using Superclass = itk::Command;
  using Pointer = itk::SmartPointer<Self>;
  itkNewMacro(Self);

  /** Data type for pixel values in data image */
  using InternalDataType = TValue;
  /** Image dimension */
  static constexpr unsigned int Dimension = TImageDimension;

  // Only valid when used with ExhaustiveOptimizerv4 for now
  // TODO allow different ExhaustiveOptimzerv4 templates
  // TODO extend for other optimizers
  using OptimizerType = itk::ExhaustiveOptimizerv4<double>;
  using StepsType = typename OptimizerType::StepsType;
  using MeasureType = typename OptimizerType::MeasureType;
  /** Default optimizer geometric output type. */
  using ParametersType = typename OptimizerType::ParametersType;
  using ScalesType = typename OptimizerType::ScalesType;

  /** Image used as n-dimensional array with positional aliasing. */
  using ImageType = itk::Image<InternalDataType, Dimension>;
  using ImagePointer = typename ImageType::Pointer;
  /** Type to represent size of array along each dimension. */
  using SizeType = typename ImageType::SizeType;
  using SizeValueType = typename ImageType::SizeValueType;
  /** Discrete values to access pixel location in data image. */
  using IndexType = typename ImageType::IndexType;
  /** Geometric distance between optimizer samples. */
  using SpacingType = typename ImageType::SpacingType;
  /** Geometric coordinates of a given sample index. */
  using PointType = typename ImageType::PointType;

  /** Observe an event fired by calling object. */
  void
  Execute(itk::Object * caller, const itk::EventObject & event) override;

  /** Observe an event fired by calling object. */
  void
  Execute(const itk::Object * caller, const itk::EventObject & event) override;

  /** Retrieve a const reference to data point recorded during optimizer iteration. */
  const TValue
  GetValue(const IndexType & index) const;
  const TValue
  GetValue(const PointType & point) const;
  const TValue
  GetValue(const ParametersType & parameters) const;

  /** Center of exhaustive region is used to compute image origin at initialization */
  itkSetMacro(Center, ParametersType);
  itkGetMacro(Center, ParametersType);
  itkGetConstMacro(TransformToImageDimension, std::vector<int>);
  itkGetConstMacro(ImageToTransformDimension, IndexType);
  itkGetConstMacro(DataImage, ImageType *);

  void
  SetFixedDimensions(const StepsType& value);

  /** Provide const methods to access underlying image parameters */
  const SizeType
  GetSize() const
  {
    return m_DataImage->GetLargestPossibleRegion().GetSize();
  }
  const SpacingType
  GetStepSize() const
  {
    return m_DataImage->GetSpacing();
  }
  const PointType
  GetOrigin() const
  {
    return m_DataImage->GetOrigin();
  }
  const ImagePointer
  GetImage() const
  {
    return m_DataImage;
  }

  /** Get the actual length of the data array in the given dimension.  */
  SizeValueType
  GetSize(const int dim) const
  {
    return (dim < Dimension) ? m_DataImage->GetLargestPossibleRegion().GetSize()[dim] : 0;
  }

  /** Get the number of steps away from the center taken by the optimizer. */
  SizeValueType
  GetNumberOfSteps(const int dim) const
  {
    return (dim < Dimension) ? ((GetSize()[dim] - 1) / 2) : 0;
  }

protected:
  CommandExhaustiveLog();
  ~CommandExhaustiveLog() override;

private:
  /** Initialize members and data array on registration StartEvent. */
  void
  Initialize(const OptimizerType * optimizer);

  /** Set data at given position. */
  void
  SetValue(const IndexType & index, const InternalDataType & value);
  void
  SetValue(const ParametersType & index, const InternalDataType & value);

  int
  TransformDimensionToImageDimension(const int dim) const;
  int
  ImageDimensionToTransformDimension(const int dim) const;

private:
  /** Coordinates at center of optimization region; ex. (2.1, -1.05).
   *   Referenced at initialization to set image origin. */
  ParametersType m_Center;
  /** Mask representing variable (0) and fixed (1) dimensions.
   *   Defaults to all dimensions variable. */
  std::vector<int> m_TransformToImageDimension;
  /** Array cache with each element representing the corresponding
   *   transform parameter index. */
  IndexType m_ImageToTransformDimension;
  /** n-dimensional array with spacing to store exhaustive values */
  ImagePointer m_DataImage;
};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkCommandExhaustiveLog.hxx"
#endif

#endif // itkCommandExhaustiveLog_hxx
