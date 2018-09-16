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

#include <memory>

namespace clan
{
	/// \addtogroup clanCore_System clanCore System
	/// \{

	class UserDataBase
	{
	public:
		virtual ~UserDataBase() { }
	};

	template<typename T>
	class UserData : public UserDataBase
	{
	public:
		UserData(const std::shared_ptr<T> &data) : data(data) { }
		std::shared_ptr<T> data;
	};

	/// \brief Helper class to store any shared_ptr as user data on an object.
	class UserDataOwner
	{
	public:
		UserDataOwner()
			: user_data(nullptr)
		{
		}

		~UserDataOwner()
		{
			if (user_data)
			{
				user_data->~UserDataBase();
				user_data = nullptr;
			}
		}

		template<typename T>
		void set_data(const std::shared_ptr<T> &data)
		{
			if (user_data)
			{
				user_data->~UserDataBase();
				user_data = nullptr;
			}

			static_assert(sizeof(UserData<T>) <= 32, "userdata_storage is too small!");
			user_data = new(userdata_storage)UserData<T>(data);
		}

		template<typename T>
		std::shared_ptr<T> get_data()
		{
			UserData<T> *d = dynamic_cast<UserData<T> *>(user_data);
			if (d)
			{
				return d->data;
			}
			else
			{
				return std::shared_ptr<T>();
			}
		}

	private:
		UserDataOwner(const UserDataOwner &that); // do not implement
		UserDataOwner &operator=(const UserDataOwner &that); // do not implement

		UserDataBase *user_data;
		char userdata_storage[32];
	};

	/// \}
}
