/* MenuMode.hpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#pragma once

/*
 * MenuMode is a game mode that implements a multiple-choice system.
 *
 */

#include "Mode.hpp"

#include <vector>
#include <functional>
#include <string>

// for GLuint
#include "GL.hpp"

// Text shaping and stuff
#include <ft2build.h>
//#include <freetype.h>
#include FT_FREETYPE_H
#include <hb.h>
#include <hb-ft.h>




struct MenuMode : Mode {
	struct Item;
	MenuMode(std::vector< Item > const& items);
	virtual ~MenuMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const&, glm::uvec2 const& window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const& drawable_size) override;

	//----- SETTINGS -----
	const double INITIAL_HEIGHT_FACTOR = 1.1; // This is multiplied by the font_size
	const double NEWLINE_HEIGHT_FACTOR = 1.4; // This is multiplied by the font_size
	const double NEWITEM_HEIGHT_FACTOR = 1.7; // This is multiplied by the font_size
	const double LEFT_MARGIN = 20.0;
	const double RIGHT_MARGIN = 20.0;
	const double TOP_MARGIN = 20.0;

	const FT_F26Dot6 TITLE_FONT_SIZE = 48;
	const FT_F26Dot6 STORY_FONT_SIZE = 32;
	const FT_F26Dot6 OPTION_FONT_SIZE = 36;


	//----- menu state -----

	//Each menu item is an "Item":
	struct Item {
		Item(
			std::string const& name_,
			float scale_ = 1.0f,
			std::function< void(Item const&) > const& on_select_ = nullptr,
			glm::vec2 const& at_ = glm::vec2(0.0f)
		) : name(name_), scale(scale_), on_select(on_select_), at(at_) {
		}
		std::string name;
		float scale; //scale for sprite
		std::function< void(Item const&) > on_select; //if set, item is selectable
		glm::vec2 at; //location to draw item
	};
	std::vector< Item > items;


	//currently selected item:
	uint32_t selected = 0;

	//area to display; by default, menu lays items out in the [-1,1]^2 box:
	glm::uvec2 view_min = glm::vec2(-1.0f, -1.0f);
	glm::uvec2 view_max = glm::vec2(1.0f, 1.0f);

	//if not nullptr, background's functions are called as follows:
	// background->handle_event() is called at the end of handle_event() [if this doesn't handle the event]
	// background->update() is called at the end of update()
	// background->draw() is called at the start of draw()
	//IMPORTANT NOTE: this means that if background->draw() ends up deleting this (e.g., by removing
	//  the last shared_ptr that references it), then it will crash. Don't do that!
	std::shared_ptr< Mode > background;

	// ----- Glyph & texture drawing util -----
	FT_Library library;
	FT_Face face;
	FT_Error ft_error;
	hb_font_t* font;

	virtual GLuint texture_loading(const void *tex_data, int width, int height);
	void RenderCharTex(GLuint tex_gluint, glm::vec2 pos, glm::vec2 size, glm::vec2 bearing, float scale, glm::vec4 color, const glm::vec2 drawableSize);
};