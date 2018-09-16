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

#if defined(WIN32) || defined(DOXYGEN)

#include "databuffer.h"
#include <vector>

namespace clan
{
	/// \addtogroup clanCore_System clanCore System
	/// \{

	class RegistryKey_Impl;

	/// \brief Registry key class.
	/** <p>This class is only available on Windows.<p>
		!group=Core/System! !header=core.h!*/
	class RegistryKey
	{
	public:
		enum PredefinedKey
		{
			key_classes_root,
			key_current_config,
			key_current_user,
			key_local_machine,
			key_users
		};

		enum CreateFlags
		{
			create_always = 0,
			create_new = 1,
			create_volatile = 2
		};

		RegistryKey();
		RegistryKey(PredefinedKey key, const std::string &subkey, unsigned int access_rights = KEY_ALL_ACCESS, unsigned int create_flags = create_always);
		RegistryKey(HKEY key);
		~RegistryKey();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		HKEY get_key() const;
		std::vector<std::string> get_subkey_names() const;
		std::vector<std::string> get_value_names() const;
		int get_value_int(const std::string &name, int default_value = 0) const;
		DataBuffer get_value_binary(const std::string &name, const DataBuffer &default_value = DataBuffer()) const;
		std::string get_value_string(const std::string &name, const std::string &default_value = std::string()) const;
		std::vector<std::string> get_value_multi_string(const std::string &name, const std::vector<std::string> &default_value = std::vector<std::string>()) const;

		RegistryKey open_key(const std::string &subkey, unsigned int access_rights = KEY_ALL_ACCESS);
		RegistryKey create_key(const std::string &subkey, unsigned int access_rights = KEY_ALL_ACCESS, CreateFlags create_flags = create_always);
		void delete_key(const std::string &subkey, bool recursive);
		static void delete_key(PredefinedKey key, const std::string &subkey, bool recursive);
		void set_value_int(const std::string &name, int value);
		void set_value_binary(const std::string &name, const DataBuffer &value);
		void set_value_string(const std::string &name, const std::string &value);
		//	void set_value_multi_string(const std::string &name, const std::vector<std::string> &value);
		void delete_value(const std::string &name);

	private:
		std::shared_ptr<RegistryKey_Impl> impl;
	};

	/// \}
}

#endif
