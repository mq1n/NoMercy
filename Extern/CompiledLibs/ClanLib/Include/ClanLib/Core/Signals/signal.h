/*
**  ClanLib SDK
**  Copyright (c) 1997-2016 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include "bind_member.h"
#include <memory>
#include <functional>
#include <vector>

namespace clan
{
	/// \addtogroup clanCore_Signals clanCore Signals
	/// \{

	class SlotImpl;

	class Slot
	{
	public:
		Slot() { }

		operator bool() const { return static_cast<bool>(impl); }

		template<typename T>
		explicit Slot(T impl) : impl(impl) { }

	private:
		std::shared_ptr<SlotImpl> impl;
	};

	class SlotImpl
	{
	public:
		SlotImpl() { }
		SlotImpl(const SlotImpl &) = delete;
		SlotImpl &operator=(const SlotImpl &) = delete;
		virtual ~SlotImpl() { }
	};

	template<typename SlotImplType>
	class SignalImpl
	{
	public:
		std::vector<std::weak_ptr<SlotImplType>> slots;
	};

	template<typename FuncType>
	class SlotImplT : public SlotImpl
	{
	public:
		SlotImplT(const std::weak_ptr<SignalImpl<SlotImplT>> &signal, const std::function<FuncType> &callback) : signal(signal), callback(callback)
		{
		}

		~SlotImplT()
		{
			std::shared_ptr<SignalImpl<SlotImplT>> sig = signal.lock();
			if (sig)
			{
				for (auto it = sig->slots.begin(); it != sig->slots.end(); ++it)
				{
					// todo: investigate if "it->lock().get() == this" is required
					if (it->expired() || it->lock().get() == this)
					{
						it = sig->slots.erase(it);
						if (it == sig->slots.end())
							break;
					}
				}
			}
		}

		std::weak_ptr<SignalImpl<SlotImplT>> signal;
		std::function<FuncType> callback;
	};

	template<typename FuncType>
	class Signal
	{
	public:
		Signal() : impl(std::make_shared<SignalImpl<SlotImplT<FuncType>>>()) { }

		template<typename... Args>
		void operator()(Args&&... args)
		{
			std::vector<std::weak_ptr<SlotImplT<FuncType>>> slots = impl->slots;
			for (std::weak_ptr<SlotImplT<FuncType>> &weak_slot : slots)
			{
				std::shared_ptr<SlotImplT<FuncType>> slot = weak_slot.lock();
				if (slot)
				{
					slot->callback(std::forward<Args>(args)...);
				}
			}
		}

		Slot connect(const std::function<FuncType> &func)
		{
			auto slot_impl = std::make_shared<SlotImplT<FuncType>>(impl, func);
			impl->slots.push_back(slot_impl);
			return Slot(slot_impl);
		}

		template<typename InstanceType, typename MemberFuncType>
		Slot connect(InstanceType instance, MemberFuncType func)
		{
			return connect(bind_member(instance, func));
		}

	private:
		std::shared_ptr<SignalImpl<SlotImplT<FuncType>>> impl;
	};

	class SlotContainer
	{
	public:
		template<typename FuncType, typename InstanceType, typename MemberFuncType>
		void connect(Signal<FuncType> &signal, InstanceType instance, MemberFuncType func)
		{
			slots.push_back(signal.connect(instance, func));
		}

		template<typename FuncType, typename CallbackType>
		void connect(Signal<FuncType> &signal, CallbackType func)
		{
			slots.push_back(signal.connect(func));
		}

	private:
		std::vector<Slot> slots;
	};

	/// \}
}
