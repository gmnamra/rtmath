/// \brief Provides silo file IO
#define _SCL_SECURE_NO_WARNINGS

#include <array>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <tuple>

#define DB_USE_MODERN_DTPTR
#include <silo.h>

#include "../../rtmath/rtmath/defs.h"
#include "../../rtmath/rtmath/ddscat/shapefile.h"
#include "../../rtmath/rtmath/ddscat/shapestats.h"
#include "../../rtmath/rtmath/plugin.h"
#include "../../rtmath/rtmath/error/debug.h"
#include "../../rtmath/rtmath/error/error.h"

#include "plugin-silo.h"
#include "WritePoints.h"


namespace rtmath {
	namespace registry {
		using std::shared_ptr;
		using rtmath::ddscat::ddOutput;
		using namespace rtmath::plugins::silo;


		shared_ptr<IOhandler> 
			write_file_type_multi
			(shared_ptr<IOhandler> sh, const char* filename, 
			const rtmath::ddscat::shapefile::shapefile *s, 
			const char* key, IOhandler::IOtype iotype)
		{
			using std::shared_ptr;
			std::shared_ptr<silo_handle> h;
			if (!sh)
			{
				// Access the hdf5 file
				h = std::shared_ptr<silo_handle>(new silo_handle(filename, iotype));
			} else {
				if (sh->getId() != PLUGINID) RTthrow debug::xDuplicateHook("Bad passed plugin");
				h = std::dynamic_pointer_cast<silo_handle>(sh);
			}

			/// \todo Modify to also support external symlinks
			//shared_ptr<Group> newstatsbase = write_hdf5_statsrawdata(grpHash, s);
			//shared_ptr<Group> newshapebase = write_hdf5_shaperawdata(grpHash, s->_shp.get());

			std::string meshname("Points_");
			//if (key)
			//	meshname.append(std::string(key));
			//else meshname.append(s->filename);

			Eigen::MatrixXf lPts(s->latticePts.rows(), s->latticePts.cols());
			lPts = s->latticePts;
			const char* axislabels[] = { "x", "y", "z" };
			const char* axisunits[] = { "dipoles", "dipoles", "dipoles" };

			std::string dielsName = meshname;
			dielsName.append("Dielectrics");
			std::string indexName = meshname;
			indexName.append("Point_IDs");
			auto pm = h->file->createPointMesh<float>(meshname.c_str(), lPts, axislabels, axisunits);

			Eigen::MatrixXi lRi = s->latticePtsRi.col(0).cast<int>();
			pm->writeData<int>(dielsName.c_str(), lRi.data(), "Dimensionless");
			Eigen::MatrixXi lIndices = s->latticeIndex.cast<int>();
			pm->writeData<int>(indexName.c_str(), lIndices.data(), "Dimensionless");

			for (const auto &extras : s->latticeExtras)
			{
				std::string varname = meshname;
				varname.append(extras.first);
				if (extras.second->cols() < 4)
					pm->writeData<float>(varname.c_str(), extras.second->data(), "Unknown");
				else
				{
					// Make a new mesh from the first three columns of the data
					std::string meshname2(varname);
					meshname2.append("_mesh");
					Eigen::MatrixXf pts = extras.second->block(0, 0, extras.second->rows(), 3);
					// Extract the values
					size_t ndims = extras.second->cols() - 3;
					Eigen::MatrixXf vals = extras.second->block(0, 3, extras.second->rows(), ndims);
					auto npm = h->file->createPointMesh<float>(meshname2.c_str(), pts, axislabels, axisunits);
					
					npm->writeData<float>(varname.c_str(), vals, "Unknown");
					//npm->writeData<float>(varname.c_str(), vals.data(), "Unknown");
				}
			}

			return h; // Pass back the handle
		}


		shared_ptr<IOhandler> 
			write_file_type_multi
			(shared_ptr<IOhandler> sh, const char* filename, 
			const rtmath::ddscat::stats::shapeFileStats *s, 
			const char* key, IOhandler::IOtype iotype)
		{
			using std::shared_ptr;
			std::shared_ptr<silo_handle> h;
			if (!sh)
			{
				// Access the hdf5 file
				h = std::shared_ptr<silo_handle>(new silo_handle(filename, iotype));
			} else {
				if (sh->getId() != PLUGINID) RTthrow debug::xDuplicateHook("Bad passed plugin");
				h = std::dynamic_pointer_cast<silo_handle>(sh);
			}

			write_file_type_multi(h, filename, s->_shp.get(), key, iotype);


			return h; // Pass back the handle
		}


	}
}
