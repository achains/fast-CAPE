#include <deplex/plane_extractor.h>
#include <deplex/utils/depth_image.h>
#include <deplex/utils/eigen_io.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <numeric>

using uint = unsigned int;

long long calculateVariance(const Eigen::VectorXi& data, long long mean) {
  long long sum = 0;

  for (auto x : data) {
    sum += (x - mean) * (x - mean);
  }

  sum /= data.size();
  return sum;
}

int main() {
  std::filesystem::path data_dir =
      std::filesystem::current_path().parent_path().parent_path().parent_path() / "benchmark-artifact/data";
  std::filesystem::path image_path = data_dir / "depth/000004415622.png";
  std::filesystem::path intrinsics_path = data_dir / "config/intrinsics.K";
  std::filesystem::path config_path = data_dir / "config/TUM_fr3_long_val.ini";

  auto start_time = std::chrono::high_resolution_clock::now();
  auto end_time = std::chrono::high_resolution_clock::now();
  long long time;

  int NUMBER_OF_RUNS = 20;
  Eigen::VectorXi test_duration = Eigen::VectorXi::Zero(NUMBER_OF_RUNS);

  deplex::config::Config config = deplex::config::Config(config_path.string());
  Eigen::Matrix3f intrinsics(deplex::utils::readIntrinsics(intrinsics_path.string()));
  deplex::utils::DepthImage image(image_path.string());
  Eigen::MatrixXf pcd_array = image.toPointCloud(intrinsics);

  auto algorithm = deplex::PlaneExtractor(image.getHeight(), image.getWidth(), config);

  Eigen::VectorXi labels;
  int found_planes;

  for (int i = 0; i < NUMBER_OF_RUNS; ++i) {
    std::cout << "Iteration #" << i + 1 << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    labels = algorithm.process(pcd_array);
    end_time = std::chrono::high_resolution_clock::now();

    time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    test_duration[i] = static_cast<int>(time);
  }

  found_planes = labels.maxCoeff();

  deplex::utils::savePointCloudCSV(test_duration.cast<float>().transpose(), (data_dir / "process_cloud.csv").string());

  long long elapsed_time_mean = std::accumulate(test_duration.begin(), test_duration.end(), 0) / NUMBER_OF_RUNS;

  long long dispersion = calculateVariance(test_duration, elapsed_time_mean);
  double standard_deviation = sqrt(dispersion);
  double standard_error = standard_deviation / sqrt(NUMBER_OF_RUNS);

  // 95% confidence interval
  const float t_value = 1.96;
  double lower_bound = static_cast<double>(elapsed_time_mean) - t_value * standard_error;
  double upper_bound = static_cast<double>(elapsed_time_mean) + t_value * standard_error;

  std::cout << "\nFound planes: " << found_planes << '\n';
  std::cout << "Dispersion: " << dispersion << std::endl;
  std::cout << "Standard deviation: " << standard_deviation << std::endl;
  std::cout << "Standard error: " << standard_error << std::endl;
  std::cout << "Confidence interval (95%): [" << lower_bound << "; " << upper_bound << "]\n\n";
  std::cout << "Elapsed time (ms.): " << elapsed_time_mean << '\n';
  std::cout << "FPS: " << 1e6l / elapsed_time_mean << '\n';

  return 0;
}