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
**    Harry Storbacka
*/

#pragma once

#include "../../Core/Signals/signal.h"
#include "../../Core/Math/point.h"
#include <memory>

namespace clan
{
	/// \addtogroup clanDisplay_Input clanDisplay Input
	/// \{

	class InputEvent;
	class InputDeviceProvider;
	class InputDevice_Impl;

	/// \brief InputDevice
	class InputDevice
	{
	public:
		/// \brief Input device types.
		enum Type
		{
			keyboard,
			pointer,
			joystick,
			unknown
		};

		/// \brief Constructs a null instance.
		InputDevice();

		/// \brief Constructs a InputDevice
		///
		/// \param provider = Input Device Provider
		InputDevice(InputDeviceProvider *provider);

		/// \brief Constructs a InputDevice
		///
		/// \param impl = Weak Ptr
		InputDevice(std::weak_ptr<InputDevice_Impl> impl);

		~InputDevice();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the provider for this input device
		InputDeviceProvider *get_provider() const;

		/// \brief Returns the human readable name of the device (i.e. 'Microsoft Sidewinder 3D').
		std::string get_name() const;

		/// \brief Return the hardware id/device for this device (i.e. /dev/input/js0)
		std::string get_device_name() const;

		/// \brief Returns the input device type.
		Type get_type() const;

		/// \brief Retrieves the localized friendly key name for specified identifier (i.e. A, B, Leertaste, Backspace, Mouse Left, ...).
		/// \note The returned name for the key may be localized by the system; it
		//        should only be used to display the name of the key to the user
		//        and not as a key identifier inside key binding configuration
		//        files and such.
		std::string get_key_name(int id) const;

		/// \brief Returns a generic string name for the specified key code.
		std::string keyid_to_string(int keycode) const;

		/// \brief Returns the key code for the specified generic string key name.
		int string_to_keyid(const std::string &str) const;

		/// \brief Returns true if the passed key code is down for this device.
		/// See `keys.h` for list of key codes.
		bool get_keycode(int keycode) const;

		/// \brief Returns the current device-independent x and y position (DIP) of the device. (Pointing devices only)
		/// The returned positional value is scaled to the pixel ratio of the display.
		Pointf get_position() const;

		/// \brief Returns the current devicesice-supplied x and y position of the device. (Pointing devices only)
		/// The returned positional value is in the scale of the physical pixel on the screen.
		Point get_device_position() const;

		/// \brief Returns the the current position of a joystick axis. (Joysticks only)
		float get_axis(int axisid) const;

		/// \brief Returns the number of axes available on this device. (Joysticks only)
		std::vector<int> get_axis_ids() const;

		/// \brief Returns the current position of a joystick hat. (Joysticks only)
		/// \return Hat direction in degrees (0-360), or -1 if the hat is centered.
		int get_hat(int index) const;

		/// \brief Returns the number of buttons available on this device.
		/// \warn If used on a keyboard or mouse, this function returns -1.
		int get_button_count() const;

		InputDevice &operator =(const InputDevice &copy);

		/// \brief Sets the display-independent position of the device. (Pointing devices only)
		void set_position(float x, float y);

		/// \brief Sets the actual position of the device. (Pointing devices only)
		void set_device_position(int x, int y);

		/// \brief Signal emitted when key is pressed.
		Signal<void(const InputEvent &)> &sig_key_down();

		/// \brief Signal emitted when key is released.
		Signal<void(const InputEvent &)> &sig_key_up();

		/// \brief Signal emitted when pointer is moved (absolute movement).
		Signal<void(const InputEvent &)> &sig_pointer_move();

		/// \brief Signal emitted when axis is moved.
		Signal<void(const InputEvent &)> &sig_axis_move();

		/// \brief Signal emitted when the mouse is double-clicked.
		Signal<void(const InputEvent &)> &sig_key_dblclk();

		/// \brief Signal emitted when proximity is entered or exited
		Signal<void(const InputEvent &)> &sig_proximity_change();

	private:
		std::shared_ptr<InputDevice_Impl> impl;
	};

	/// \}
}
