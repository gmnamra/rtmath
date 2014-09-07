/// \brief Provides vtk
#define _SCL_SECURE_NO_WARNINGS

#include <string>

#include "../../rtmath/rtmath/defs.h"
#include "../../rtmath/rtmath/ddscat/shapefile.h"
#include "../../rtmath/rtmath/ddscat/shapestats.h"
#include "../../rtmath/rtmath/ddscat/hulls.h"
#include "../../rtmath/rtmath/Voronoi/Voronoi.h"
#include "../../rtmath/rtmath/plugin.h"
#include "voro-Voronoi.h"

#include "plugin-voro.h"

void dllEntry();
rtmath_plugin_init(dllEntry);

void dllEntry()
{
	using namespace rtmath::registry;
	using namespace rtmath::plugins::voro;
	static const rtmath::registry::DLLpreamble id(
		"Plugin-voro",
		"Provides voronoi diagram generation",
		PLUGINID);
	rtmath_registry_register_dll(id);

	//genAndRegisterIOregistry<::rtmath::ddscat::shapefile::shapefile, 
	//	rtmath::ddscat::shapefile::shapefile_IO_output_registry>("silo",PLUGINID);

	//genAndRegisterIOregistry<::rtmath::Voronoi::VoronoiDiagram,
	//	rtmath::Voronoi::Voronoi_IO_output_registry>("silo", PLUGINID);

	rtmath::Voronoi::Voronoi_provider reg_Voronoi;
	reg_Voronoi.name = "voro";
	reg_Voronoi.generator = ::rtmath::plugins::voro::VoroVoronoiDiagram::generateStandard;
	reg_Voronoi.voronoiBlankGenerator = ::rtmath::plugins::voro::VoroVoronoiDiagram::generateBlank;
	reg_Voronoi.voronoiUpcastGenerator = ::rtmath::plugins::voro::VoroVoronoiDiagram::generateUpcast;

	doRegisterHook<rtmath::Voronoi::VoronoiDiagram, ::rtmath::Voronoi::Voronoi_provider_registry,
		::rtmath::Voronoi::Voronoi_provider >(reg_Voronoi);
}
