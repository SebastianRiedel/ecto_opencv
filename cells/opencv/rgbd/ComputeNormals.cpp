/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2009, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <ecto/ecto.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/rgbd/rgbd.hpp>

using ecto::tendrils;
namespace rgbd
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  struct ComputeNormals
  {
    static void
    declare_io(const tendrils& params, tendrils& inputs, tendrils& outputs)
    {
      inputs.declare(&ComputeNormals::points3d_, "points3d", "The 3d points from a depth image").required(true);
      inputs.declare(&ComputeNormals::K_, "K", "The calibration matrix").required(true);

      outputs.declare(&ComputeNormals::normals_, "normals", "The normals");
    }

    int
    process(const tendrils& inputs, const tendrils& outputs)
    {
      if (normals_computer_.empty())
        normals_computer_ = cv::Ptr<cv::RgbdNormals>(
            new cv::RgbdNormals(points3d_->rows, points3d_->cols, points3d_->depth(), *K_));
      *normals_ = (*normals_computer_)(*points3d_);

      return ecto::OK;
    }
    cv::Ptr<cv::RgbdNormals> normals_computer_;
    ecto::spore<cv::Mat> points3d_, normals_, K_;
  };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  struct DrawNormals
  {
    static void
    declare_params(tendrils& params)
    {
      params.declare(&DrawNormals::step_, "step", "The step at which to display normals in pixels.", 40);
    }

    static void
    declare_io(const tendrils& params, tendrils& inputs, tendrils& outputs)
    {
      inputs.declare(&DrawNormals::image_in_, "image", "The input image").required(true);
      inputs.declare(&DrawNormals::K_, "K", "The intrinsic matrix").required(true);
      inputs.declare(&DrawNormals::points3d_, "points3d", "The 3d points").required(true);
      inputs.declare(&DrawNormals::normals_, "normals", "The normals").required(true);

      outputs.declare(&DrawNormals::image_out_, "image", "The output image");
    }

    int
    process(const tendrils& inputs, const tendrils& outputs)
    {
      image_in_->copyTo(*image_out_);

      // Draw the normals at given steps
      for (int y = *step_; y < (image_in_->rows - (*step_)); y += *step_)
        for (int x = *step_; x < (image_in_->cols - (*step_)); x += *step_)
        {
          cv::Vec3f normal;
          if (normals_->depth() == CV_32F)
            normal = normals_->at<cv::Vec3f>(y, x);
          else
            normal = normals_->at<cv::Vec3d>(y, x);
          normal = normal / cv::norm(normal);

          cv::Vec3f point3d;
          if (points3d_->depth() == CV_32F)
            point3d = points3d_->at<cv::Vec3f>(y, x);
          else
            point3d = points3d_->at<cv::Vec3d>(y, x);

          if (cvIsNaN(normal[0]))
            continue;
          int len = (*step_) / 2 - 2;

          std::vector<cv::Point3f> points(
              1, cv::Point3f(point3d[0] + len * normal[0], point3d[1] + len * normal[1], point3d[2] + len * normal[2]));
          std::vector<cv::Point2f> points2d;
          cv::Vec3f rvec(0, 0, 0), tvec(0, 0, 0);
          cv::projectPoints(points, rvec, tvec, *K_, cv::Mat(), points2d);
          cv::line(*image_out_, cv::Point2i(x, y), points2d[0], cv::Scalar(255, 0, 0));
        }

      return ecto::OK;
    }

    ecto::spore<int> step_;
    ecto::spore<cv::Mat> image_in_, image_out_, normals_, points3d_, K_;
  };
}

ECTO_CELL(rgbd, rgbd::ComputeNormals, "ComputeNormals", "Compute the normals in a depth image.")
ECTO_CELL(rgbd, rgbd::DrawNormals, "DrawNormals", "Display 3d normals in a depth image.")