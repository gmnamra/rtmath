#include <memory>
#include <mutex>
#include <string>
#include <iostream>
#include <Ryan_Debug/splitSet.h>
#include "../scatdb_ryan/scatdb_ryan.hpp"

namespace scatdb_ryan {
	class filterImpl {
	public:
		~filterImpl() {}
	private:
		friend class filter;
		filterImpl() {
			floatFilters.resize(db::data_entries::NUM_DATA_ENTRIES_FLOATS);
			intFilters.resize(db::data_entries::NUM_DATA_ENTRIES_INTS);
		}
		std::vector<Ryan_Debug::splitSet::intervals<float>  > floatFilters;
		std::vector<Ryan_Debug::splitSet::intervals<int> > intFilters;
		enum class SortDataType { FLOATS, INTS };
		struct sortType {
			SortDataType sortDataType;
			filter::sortDir dir;
			int varnum;
		};
		std::vector<sortType> sorts;
	};

	void filter::addSortFloat(db::data_entries::data_entries_floats param, filter::sortDir fsd) {
		filterImpl::sortType nst;
		nst.dir = fsd;
		nst.varnum = (int) param;
		nst.sortDataType = filterImpl::SortDataType::FLOATS;
		p->sorts.push_back(std::move(nst));
	}
	void filter::addSortInt(db::data_entries::data_entries_ints param, filter::sortDir fsd) {
		filterImpl::sortType nst;
		nst.dir = fsd;
		nst.varnum = (int) param;
		nst.sortDataType = filterImpl::SortDataType::INTS;
		p->sorts.push_back(std::move(nst));
	}

	filter::filter() { p = std::shared_ptr<filterImpl>(new filterImpl); }
	filter::~filter() {}
	std::shared_ptr<filter> filter::generate() {
		return std::shared_ptr<filter>(new filter);
	}

	void filter::addFilterFloat(db::data_entries::data_entries_floats param, float minval, float maxval) {
		p->floatFilters[param].ranges.push_back(std::pair<float, float>(minval, maxval));
	}

	void filter::addFilterInt(db::data_entries::data_entries_ints param, int minval, int maxval) {
		p->intFilters[param].ranges.push_back(std::pair<int,int>(minval,maxval));
	}

	template<>
	void filter::addFilter<float>(int param, float minval, float maxval) {
		addFilterFloat((db::data_entries::data_entries_floats) param, minval, maxval);
	}

	template<>
	void filter::addFilter<int>(int param, int minval, int maxval) {
		addFilterInt((db::data_entries::data_entries_ints) param, minval, maxval);
	}

	void filter::addFilterFloat(db::data_entries::data_entries_floats param, const std::string &rng) {
		p->floatFilters[param].append(rng);
	}

	void filter::addFilterInt(db::data_entries::data_entries_ints param, const std::string &rng) {
		p->intFilters[param].append(rng);
	}

	template<>
	void filter::addFilter<float>(int param, const std::string &rng) {
		addFilterFloat((db::data_entries::data_entries_floats) param, rng);
	}

	template<>
	void filter::addFilter<int>(int param, const std::string &rng) {
		addFilterInt((db::data_entries::data_entries_ints) param, rng);
	}

	std::shared_ptr<const db> filter::apply(std::shared_ptr<const db> src) const {
		std::shared_ptr<db> res(new db), presort(new db);
		presort->floatMat.resize(src->floatMat.rows(), src->floatMat.cols());
		presort->intMat.resize(src->intMat.rows(), src->intMat.cols());

		res->floatMat.resize(src->floatMat.rows(), src->floatMat.cols());
		res->intMat.resize(src->intMat.rows(), src->intMat.cols());
		int numLines = res->floatMat.rows();
		int totLines = 0;

		// Apply filter operation to each row
		for(size_t i=0; i<numLines; ++i) {
			auto floatLine = src->floatMat.block<1,db::data_entries::NUM_DATA_ENTRIES_FLOATS>(i,0);
			auto intLine = src->intMat.block<1,db::data_entries::NUM_DATA_ENTRIES_INTS>(i,0);

			bool good = true;
			for (int j = 0; j < db::data_entries::NUM_DATA_ENTRIES_FLOATS; ++j) {
				if (!p->floatFilters[j].ranges.size()) continue;
				if (!p->floatFilters[j].inRange(floatLine(j))) good = false;
				if (!good) break;
			}
			if (!good) continue;
			for (int j = 0; j < db::data_entries::NUM_DATA_ENTRIES_INTS; ++j) {
				if (!p->intFilters[j].ranges.size()) continue;
				if (!p->intFilters[j].inRange(intLine(j))) good = false;
				if (!good) break;
			}
			if (!good) continue;

			res->floatMat.block<1,db::data_entries::NUM_DATA_ENTRIES_FLOATS>(totLines,0) = floatLine;
			res->intMat.block<1,db::data_entries::NUM_DATA_ENTRIES_INTS>(totLines,0) = intLine;
			totLines++;
		}
		res->floatMat.conservativeResize(totLines, src->floatMat.cols());
		res->intMat.conservativeResize(totLines, src->intMat.cols());

		return res;
	}
}

