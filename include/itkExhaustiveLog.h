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
#ifndef itkExhaustiveLog_h
#define itkExhaustiveLog_h

#include "itkCommand.h"
#include "itkImage.h"
//#include "itkExhaustiveOptimizerv4.h"

namespace itk
{
    class ExhaustiveLog : public itk::Command
    {
    public:
        //using ImageType = itk::Image<float, 2>;
        using Self = ExhaustiveLog;
        using Superclass = itk::Command;
        using Pointer = itk::SmartPointer<Self>;
        itkNewMacro(Self);

        //using OptimizerType = itk::ExhaustiveOptimizerv4<double>;

        void
            Execute(itk::Object* caller, const itk::EventObject& event) override;

        void
            Execute(const itk::Object* caller, const itk::EventObject& event) override;

        // Set/Get the image representation of exhaustive optimization over a region.
        //itkGetConstMacro(ExhaustiveImage, double);
        //itkSetMacro(ExhaustiveImage, double);

    protected:
        ExhaustiveLog();
        ~ExhaustiveLog() override;
    private:
        //ImageType m_ExhaustiveImage;
    };
} // namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExhaustiveLog.hxx"
#endif

#endif // itkExhaustiveLog_hxx
