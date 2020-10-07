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
#include <iostream>

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
	struct SNode;
	MenuMode(SNode *sNode);
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

	const FT_F26Dot6 TITLE_FONT_SIZE = 60;
	const FT_F26Dot6 STORY_FONT_SIZE = 32;
	const FT_F26Dot6 OPTION_FONT_SIZE = 36;

	const float PEAK_CORRUPTION_TIME = 120.0f;
	const float TEXT_CORRUPT_DELAY = 0.9f; // Interval between text corruptions at peak corruption

	glm::vec4 BASE_COLOR = glm::vec4(0.1f, 0.25f, 0.7f, 0.0f);
	glm::vec4 CORRUPT_COLOR = glm::vec4(143.0f / 255.0f, 0.1f, 0.0f, 0.0f);


	//----- menu state -----

	// Types of items
	enum class ITEM_TYPE {
		TITLE, STORY, OPTION
	};

	//Each menu item is an "Item":
	struct Item {
		Item(
			std::string const& name_,
			ITEM_TYPE type_ = ITEM_TYPE::OPTION,
			std::function< void(Item const&) > const& on_select_ = nullptr
		) : name(name_), type(type_), on_select(on_select_) {
		}
		std::string name;
		ITEM_TYPE type;
		std::function< void(Item const&) > on_select; //if set, item is selectable
	};


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


	//----- story state -----
	float story_time = 0.0f; // Time that has passed since you've picked up an item
	float corrupt_factor = 0.0f; // [0, 1] where 0 is not corrupt, 1 is corrupt

	float text_corrupt_countdown = TEXT_CORRUPT_DELAY;

	bool pen = false; // If you're holding the pen
	bool sword = false; // If you're holding the sword
	bool embraced = false; // If you embraced the presence of the second Primordial

	void setStoryTime(float newStoryTime) {
		story_time = newStoryTime;
		corrupt_factor = story_time / PEAK_CORRUPTION_TIME;
		if (corrupt_factor > 1) corrupt_factor = 1.0f;
		if (corrupt_factor < 0) corrupt_factor = 0.0f;
	}

	void initStoryState() {
		setStoryTime(0.0f);
		pen = false;
		sword = false;
		embraced = false;
	}

	// Sometimes (based on corruption) returns a random float to be used for text offset
	float randCorruptedOffset(FT_F26Dot6 font_size) {
		if (story_time < PEAK_CORRUPTION_TIME * 0.4f) return 0.0f;
		int randomInt = rand();
		if (randomInt % (8000 * (int)PEAK_CORRUPTION_TIME) < (int)story_time) {
			return (randomInt % 49 - 24) * font_size / 64.0f;
		}
		return 0.0f;
	}


	// ----- Story graph -----
	//story node
	struct SNode {
		std::vector<Item> items = std::vector<Item>();
	};
	SNode *sNode;
	SNode *TitleScreen;

	void setSNode(SNode *new_SNode) {
		sNode = new_SNode;
		//select first item which can be selected:
		for (uint32_t i = 0; i < sNode->items.size(); ++i) {
			if (sNode->items[i].on_select) {
				selected = i;
				break;
			}
		}
	}


	// ----- Glyph & texture drawing util -----
	FT_Library library;
	FT_Face face;
	FT_Error ft_error;
	hb_font_t* font;

	virtual GLuint texture_loading(const void *tex_data, int width, int height);
	void RenderCharTex(GLuint tex_gluint, glm::vec2 pos, glm::vec2 size, glm::vec2 bearing, float scale, glm::vec4 color, const glm::vec2 drawableSize);
};