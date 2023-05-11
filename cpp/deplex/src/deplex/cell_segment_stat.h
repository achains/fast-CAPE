/**
 * Copyright (c) 2022, Arthur Saliou, Anastasiia Kornilova
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <Eigen/Core>

namespace deplex {
class CellSegmentStat {
 public:
  CellSegmentStat();

  explicit CellSegmentStat(Eigen::MatrixX3f const& points);

  CellSegmentStat& operator+=(CellSegmentStat const& other);

  Eigen::Vector3f const& getNormal() const;

  Eigen::Vector3f const& getMean() const;

  float getScore() const;

  float getMSE() const;

  float getD() const;

  void fitPlane();

 private:
  float d_;
  float score_;
  float mse_;
  int32_t nr_pts_;
  Eigen::Vector3f coord_sum_;
  Eigen::Matrix3f variance_;
  Eigen::Vector3f mean_;
  Eigen::Vector3f normal_;
};

}  // namespace deplex