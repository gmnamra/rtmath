#include <array>
#include <string>
#include <tuple>
#include <vector>
#include <Eigen/Dense>

struct DBfile;

namespace rtmath {
	namespace plugins {
		namespace silo {

			void WritePoints(DBfile *db, const std::array<std::string, 3> &axislabels,
				const std::array<std::string, 3> &axisunits, 
				const Eigen::Matrix<float, Eigen::Dynamic, 3> &pts,
				std::vector<std::tuple<std::string, std::string, 
				const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> > > &vals);
		}
	}
}
