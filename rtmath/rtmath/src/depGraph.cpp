#include "Stdafx-core.h"

#include <set>
#include <memory>
#ifdef __GNUC__
#include <initializer_list>
#endif

#include "../rtmath/depGraph.h"
#include <Ryan_Debug/error.h>

namespace rtmath
{
	namespace graphs
	{
#ifdef __GNUC__
		vertex* vertex::connect(
			vertex* target, 
			std::initializer_list<vertex* > depends)
		{
			// Create connector
			vertex* connector;
			connector = new vertex(false);
			target->addSlot(connector);
			for (vertex* it : depends)
				connector->addSlot(it);

			return connector;
		}
#endif

		vertex* vertex::connect(
			vertex* target,
			const std::set<vertex* > &depends)
		{
			vertex* connector = new vertex(false);
			target->addSlot(connector);
			for (auto it = depends.begin(); it != depends.end(); it++)
			//for (std::shared_ptr<vertex> it : depends)
				connector->addSlot(*it);

			return connector;
		}

		vertex::vertex(bool OR) : _slotOR(OR), _target(nullptr)
		{
			//_signals.reserve(12);
			//_slots.reserve(12);
			std::fill(_slots.begin(), _slots.end(), nullptr);
			std::fill(_signals.begin(), _signals.end(), nullptr);
		}

		vertex::~vertex() {}

		bool vertex::addSlot(vertex* slot)
		{
			if (this == slot)
			{
				//std::cerr << "Ignoring vertex as a slot for itself " << this << ".\n";
				//RDthrow debug::xArrayOutOfBounds();
				return true;
			}
			// Find first unused _slots
			auto it = std::find_if(_slots.begin(), _slots.end(), 
				[](const vertex *v)
			{
				if (v) return false;
				return true;
			});
			//auto it = std::find(_slots.begin(), _slots.end(), nullptr);
			if (it == _slots.end())
			{
				//std::cerr << "Cannot find empty vertex for addSlot " << this << " + " << slot << ".\n";
				//RDthrow debug::xArrayOutOfBounds();
				return false;
			}
			*it = slot;
			//_slots.insert(slot);
			// Check if the parent is OR. If so, set the parent's signal
			if (!slot->_slotOR)
				return slot->_addSignal( this );
			return true;
		}

		bool vertex::_addSignal(vertex* signal)
		{
			if (this == signal)
			{
				//std::cerr << "Ignoring vertex as a signal for itself " << this << ".\n";
				//RDthrow debug::xArrayOutOfBounds();
				return true;
			}
			//_signals.insert(signal);

			auto it = std::find_if(_signals.begin(), _signals.end(), 
				[](const vertex *v)
			{
				if (v) return false;
				return true;
			});
			//auto it = std::find(_signals.begin(), _signals.end(), nullptr);
			if (it == _signals.end()) 
			{
				//std::cerr << "Cannot find empty vertex for _addSignal " << this << " + " << signal << ".\n";
				//RDthrow debug::xArrayOutOfBounds();
				return false;
			}
			*it = signal;
			return true;
		}

		void vertex::run(vertexRunnable *target, const std::string &id) const
		{
			if (target)
				target->run(id);
		}

		void vertex::run(const std::string &id) const
		{
			if (_target)
				_target->run(id);
		}

		void vertex::setVertexRunnableCode(vertexRunnable* target)
		{
			_target = target;
		}


	}
}

