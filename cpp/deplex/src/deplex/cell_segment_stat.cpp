#include "cell_segment_stat.h"

#include <Eigen/Eigenvalues>

namespace deplex {
CellSegmentStat::CellSegmentStat() : mse_(-1) {}

CellSegmentStat::CellSegmentStat(Eigen::MatrixXf const& points)
    : nr_pts_(points.rows()),
      coord_sum_(points.colwise().sum()),
      variance_(points.transpose() * points),
      mean_(coord_sum_ / nr_pts_) {}

CellSegmentStat& CellSegmentStat::operator+=(CellSegmentStat const& other) {
  nr_pts_ += other.nr_pts_;
  coord_sum_ += other.coord_sum_;
  variance_ += other.variance_;
  mean_ = coord_sum_ / nr_pts_;
  return *this;
}

Eigen::Vector3f const& CellSegmentStat::getNormal() const { return normal_; };

Eigen::Vector3f const& CellSegmentStat::getMean() const { return mean_; };

float CellSegmentStat::getScore() const { return score_; };

float CellSegmentStat::getMSE() const { return mse_; };

float CellSegmentStat::getD() const { return d_; };

void CellSegmentStat::fitPlane() {
  Eigen::Matrix3f cov = variance_ - coord_sum_ * coord_sum_.transpose() / nr_pts_;
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> es(cov);
  Eigen::VectorXf v = es.eigenvectors().col(0);

  d_ = -mean_.dot(v);
  // Enforce normal orientation
  normal_ = (d_ > 0 ? v : -v);
  d_ = (d_ > 0 ? d_ : -d_);

  mse_ = es.eigenvalues()[0] / nr_pts_;
  score_ = es.eigenvalues()[1] / es.eigenvalues()[0];
}
}  // namespace deplex