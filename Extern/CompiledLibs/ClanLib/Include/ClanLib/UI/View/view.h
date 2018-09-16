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
**    Artem Khomenko (Direct redraw changed state of View, without redraw the entire window).
*/

#pragma once

#include "../../Core/Math/mat4.h"
#include "../../Core/Math/rect.h"
#include "../../Core/Math/easing.h"
#include "../../Core/Signals/signal.h"
#include "../../UI/Events/event.h"
#include "../View/view_geometry.h"
#include "../Style/style.h"
#include "../Style/style_cascade.h"
#include "../Style/style_get_value.h"
#include "focus_policy.h"
#include <vector>
#include <memory>
#include <functional>

namespace clan
{
	class Style;
	class StyleCascade;
	class Canvas;
	class ActivationChangeEvent;
	class CloseEvent;
	class FocusChangeEvent;
	class PointerEvent;
	class ResizeEvent;
	class KeyEvent;
	class ViewImpl;
	class CursorDescription;
	enum class StandardCursor;
	class DisplayWindow;
	class ViewTree;
	class ViewAction;

	/// View for an area of the user interface
	class View : public std::enable_shared_from_this<View>
	{
	public:
		View();
		virtual ~View();
		
		/// Style cascade currently active for this view
		const StyleCascade &style_cascade() const;

		/// Style properties for the specified state
		const std::shared_ptr<Style> &style(const std::string &state = std::string()) const;
		
		/// Test if a style state is currently set
		bool state(const std::string &name) const;

		/// Set or clear style state
		void set_state(const std::string &name, bool value);

		/// Sets the state for this view and all children recursively, until a manually set state of the same name is found
		void set_state_cascade(const std::string &name, bool value);

		/// Slot container helping with automatic disconnection of connected slots when the view is destroyed
		SlotContainer slots;

		/// Parent view node or nullptr if the view is the current root node
		View *parent() const;

		/// List of all immediate child views
		const std::vector<std::shared_ptr<View>> &children() const;

		/// Add a child view
		void add_child(const std::shared_ptr<View> &view);

		template<typename T, typename... Types>
		std::shared_ptr<T> add_child(Types &&... args)
		{
			auto child = std::make_shared<T>(std::forward<Types>(args)...);
			add_child(child);
			return child;
		}

		std::shared_ptr<View> add_child()
		{
			return add_child<View>();
		}

		/// Remove view from parent
		void remove_from_parent();

		/// Add an action recognizer
		void add_action(const std::shared_ptr<ViewAction> &action);

		template<typename T, typename... Types>
		std::shared_ptr<T> add_action(Types &&... args)
		{
			auto action = std::make_shared<T>(std::forward<Types>(args)...);
			add_action(action);
			return action;
		}

		/// List of all action recognizers
		const std::vector<std::shared_ptr<ViewAction>> &actions() const;

		/// Test if view is set to hidden
		bool hidden() const;

		/// Hides a view from layout and rendering
		void set_hidden(bool value = true);

		/// Test if view should participate in static layout calculations (layout_children)
		bool is_static_position_and_visible() const;

		/// Test if view geometry needs to be recalculated
		bool needs_layout() const;

		/// Forces recalculation of view geometry before next rendering
		void set_needs_layout();

		/// Actual view position and size after layout
		const ViewGeometry &geometry() const;

		/// Sets the view position and size
		///
		/// This function should only be called by layout_children.
		void set_geometry(const ViewGeometry &geometry);

		/// Gets the current canvas used to render this view
		///
		/// This function may return a null canvas if the view does not have a canvas attached to it yet.
		Canvas canvas() const;

		/// Signals this view needs to be rendered again
		void set_needs_render();

		/// Test if this view generated an exception during rendering
		bool render_exception_encountered() const;

		/// Clears exception encountered flag
		///
		/// If a view generates an exception during rendering the view's render_content function will not be called again until this function is called.
		void clear_exception_encountered();

		/// Current view transform
		const Mat4f &view_transform() const;

		/// Specifies the view transform to be applied before its contents and children are rendered
		void set_view_transform(const Mat4f &transform);

		/// Content clipping flag
		bool content_clipped() const;

		/// Specifies if content should be clipped during rendering
		void set_content_clipped(bool clipped);

		/// Calculates the preferred width of this view
		float preferred_width(Canvas &canvas);

		/// Calculates the preferred height of this view
		float preferred_height(Canvas &canvas, float width);

		/// The content width used for percentages or other definite calculations
		float definite_width();

		/// The content height used for percentages or other definite calculations
		float definite_height();

		/// Test if the view has a definite width
		bool is_width_definite();

		/// Test if the view has a definite height
		bool is_height_definite();

		/// Calculates the offset to the first baseline
		float first_baseline_offset(Canvas &canvas, float width);

		/// Calculates the offset to the last baseline
		float last_baseline_offset(Canvas &canvas, float width);

		/// Sets the view geometry for all children of this view
		virtual void layout_children(Canvas &canvas);

		/// Tree in view hierachy
		const ViewTree *view_tree() const;
		ViewTree *view_tree();

		/// The view receiving keyboard events or nullptr if no view has the focus
		View *focus_view() const;

		/// Find descendant view at the specified content relative position
		std::shared_ptr<View> find_view_at(const Pointf &pos) const;

		/// Focus policy active for this view
		FocusPolicy focus_policy() const;

		/// Set if this view automatically can gain focus
		void set_focus_policy(FocusPolicy policy);

		/// Tab index for keyboard focus changes
		unsigned int tab_index() const;

		/// Sets the tab index used for keyboard focus changes
		void set_tab_index(unsigned int index);

		/// Set this view as the focused view
		void set_focus();

		/// Remove focus from this view
		void remove_focus();

		/// Test if this view is receiving keyboard input
		bool has_focus() const { return focus_view() == this; }

		/// Give focus to the previous view in the keyboard tab index order
		void prev_focus();

		/// Give focus to the next view in the keyboard tab index order
		void next_focus();

		/// Continously call an animation function for the specified duration
		void animate(float from, float to, const std::function<void(float)> &setter, int duration_ms = 400, const std::function<float(float)> &easing = Easing::linear, std::function<void()> animation_end = std::function<void()>());

		/// Stop all activate animation functions
		void stop_animations();

		/// Set the cursor icon used when cursor is above this view
		void set_cursor(const CursorDescription &cursor);
		void set_cursor(StandardCursor type);

		/// Specify that the cursor icon is inherited from the parent view
		void set_inherit_cursor();

		/// Window activated event
		Signal<void(ActivationChangeEvent &)> &sig_activated(bool use_capture = false);

		/// Window deactivated event
		Signal<void(ActivationChangeEvent &)> &sig_deactivated(bool use_capture = false);

		/// Window close button clicked event
		Signal<void(CloseEvent &)> &sig_close(bool use_capture = false);

		/// Window resize event
		Signal<void(ResizeEvent &)> &sig_resize(bool use_capture = false);

		/// View gained focus event
		Signal<void(FocusChangeEvent &)> &sig_focus_gained(bool use_capture = false);

		/// View lost focus event
		Signal<void(FocusChangeEvent &)> &sig_focus_lost(bool use_capture = false);

		/// Pointer entering view geometry event
		Signal<void(PointerEvent &)> &sig_pointer_enter(bool use_capture = false);

		/// Pointer leaving view geometry event
		Signal<void(PointerEvent &)> &sig_pointer_leave(bool use_capture = false);

		/// Pointer moved above view event
		Signal<void(PointerEvent &)> &sig_pointer_move(bool use_capture = false);

		/// Pointer button pressed event
		Signal<void(PointerEvent &)> &sig_pointer_press(bool use_capture = false);

		/// Pointer button released event
		Signal<void(PointerEvent &)> &sig_pointer_release(bool use_capture = false);

		/// Pointer button double clicked event
		Signal<void(PointerEvent &)> &sig_pointer_double_click(bool use_capture = false);

		/// Pointer proximity change event
		Signal<void(PointerEvent &)> &sig_pointer_proximity_change(bool use_capture = false);

		/// Key pressed event
		Signal<void(KeyEvent &)> &sig_key_press(bool use_capture = false);

		/// Key released event
		Signal<void(KeyEvent &)> &sig_key_release(bool use_capture = false);

		/// Update window cursor to the cursor used by this view
		void update_cursor(DisplayWindow &window);

		/// Map from local content to screen coordinates
		Pointf to_screen_pos(const Pointf &pos);

		/// Map from screen to local content coordinates
		Pointf from_screen_pos(const Pointf &pos);

		/// Map from local content to root content or margin (plus content, padding, border and margin) coordinates.
		Pointf to_root_pos(const Pointf &pos, bool relative_to_margin = false);

		/// Map from root content to local content coordinates
		Pointf from_root_pos(const Pointf &pos);

		/// Dispatch event to signals listening for events
		static void dispatch_event(View *target, EventUI *e, bool no_propagation = false);

		/// Render view and its children directly, without re-layout.
		void draw_without_layout();

	protected:
		/// Renders the content of a view
		virtual void render_content(Canvas &canvas) { }

		/// Renders the border of a view
		virtual void render_border(Canvas &canvas);

		/// Renders the background of a view
		virtual void render_background(Canvas &canvas);

		/// Child view was added to this view
		virtual void child_added(const std::shared_ptr<View> &view) { }

		/// Child view was removed from this view
		virtual void child_removed(const std::shared_ptr<View> &view) { }

		/// Calculates the preferred width of this view
		virtual float calculate_preferred_width(Canvas &canvas);

		/// Calculates the preferred height of this view
		virtual float calculate_preferred_height(Canvas &canvas, float width);

		/// Calculates the offset to the first baseline
		virtual float calculate_first_baseline_offset(Canvas &canvas, float width);

		/// Calculates the offset to the last baseline
		virtual float calculate_last_baseline_offset(Canvas &canvas, float width);

		/// The content width used for percentages or other definite calculations
		virtual float calculate_definite_width(bool &out_is_definite);

		/// The content height used for percentages or other definite calculations
		virtual float calculate_definite_height(bool &out_is_definite);

	private:
		View(const View &) = delete;
		View &operator=(const View &) = delete;

		std::unique_ptr<ViewImpl> impl;

		friend class ViewTree;
		friend class ViewImpl;
		friend class ViewAction;
	};
}
