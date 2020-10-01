/* MenuMode.cpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#include "MenuMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for easy sprite drawing:
//#include "DrawSprites.hpp"

//for playing movement sounds:
#include "Sound.hpp"

//for loading:
#include "Load.hpp"

// TEMPORARY DrawLines for temp text rendering
#include "DrawLines.hpp"

// Real text rendering libraries
//#include <ft2build.h>
//#include FT_FREETYPE_H
//#include <hb.h>
//#include <hb-ft.h>
//#include <freetype.h>

// Extraneous
#include <random>

Load< Sound::Sample > sound_click(LoadTagDefault, []() -> Sound::Sample* {
	std::vector< float > data(size_t(48000 * 0.2f), 0.0f);
	for (uint32_t i = 0; i < data.size(); ++i) {
		float t = i / float(48000);
		//phase-modulated sine wave (creates some metal-like sound):
		data[i] = std::sin(3.1415926f * 2.0f * 440.0f * t + std::sin(3.1415926f * 2.0f * 450.0f * t));
		//quadratic falloff:
		data[i] *= 0.3f * std::pow(std::max(0.0f, (1.0f - t / 0.2f)), 2.0f);
	}
	return new Sound::Sample(data);
	});

Load< Sound::Sample > sound_clonk(LoadTagDefault, []() -> Sound::Sample* {
	std::vector< float > data(size_t(48000 * 0.2f), 0.0f);
	for (uint32_t i = 0; i < data.size(); ++i) {
		float t = i / float(48000);
		//phase-modulated sine wave (creates some metal-like sound):
		data[i] = std::sin(3.1415926f * 2.0f * 220.0f * t + std::sin(3.1415926f * 2.0f * 200.0f * t));
		//quadratic falloff:
		data[i] *= 0.3f * std::pow(std::max(0.0f, (1.0f - t / 0.2f)), 2.0f);
	}
	return new Sound::Sample(data);
	});


MenuMode::MenuMode(std::vector< Item > const& items_) : items(items_) {

	//select first item which can be selected:
	for (uint32_t i = 0; i < items.size(); ++i) {
		if (items[i].on_select) {
			selected = i;
			break;
		}
	}
}

MenuMode::~MenuMode() {
}

bool MenuMode::handle_event(SDL_Event const& evt, glm::uvec2 const& window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_UP ||  // UP
			  evt.key.keysym.sym == SDLK_w) {
			//skip non-selectable items:
			for (uint32_t i = selected - 1; i < items.size(); --i) {
				if (items[i].on_select) {
					selected = i;
					Sound::play(*sound_click);
					break;
				}
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN ||  // DOWN
			         evt.key.keysym.sym == SDLK_s) {
			//note: skips non-selectable items:
			for (uint32_t i = selected + 1; i < items.size(); ++i) {
				if (items[i].on_select) {
					selected = i;
					Sound::play(*sound_click);
					break;
				}
			}
			return true;
		} else if (evt.key.keysym.sym == SDLK_RETURN ||  // SELECT
			         evt.key.keysym.sym == SDLK_SPACE) {
			if (selected < items.size() && items[selected].on_select) {
				Sound::play(*sound_clonk);
				items[selected].on_select(items[selected]);
				return true;
			}
		} else if (evt.key.keysym.sym == SDLK_q) {  // QUIT
			quit = true;
			return true;
		}
	}
	if (background) {
		return background->handle_event(evt, window_size);
	}
	else {
		return false;
	}
}

void MenuMode::update(float elapsed) {
	select_bounce_acc = select_bounce_acc + elapsed / 0.7f;
	select_bounce_acc -= std::floor(select_bounce_acc);

	if (background) {
		background->update(elapsed);
	}

	//This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
	//You probably shouldn't be looking here to learn to use either library.
	// Source - @xiaoqiao in #game4 in 15-466 Discord
	{
		FT_Library library;
		FT_Face face;
		FT_Init_FreeType(&library);

		hb_buffer_t* buf = hb_buffer_create();
		hb_buffer_add_utf8(buf, "Apple", -1, 0, -1);
		hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
		hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
		hb_buffer_set_language(buf, hb_language_from_string("en", -1));
		FT_New_Face(library, "E:/Coding/CMU/15466/game4/dist/Fonts/OpenSans-Regular.ttf", 0, &face);
		FT_Set_Char_Size(face, 0, 1000, 0, 0);
		hb_font_t* font = hb_ft_font_create(face, NULL);
		hb_shape(font, buf, NULL, 0);
		unsigned int glyph_count;
		hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
		hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
		double cursor_x = 0.0, cursor_y = 0.0;
		for (unsigned int i = 0; i < glyph_count; ++i) {
			auto glyphid = glyph_info[i].codepoint;
			auto x_offset = glyph_pos[i].x_offset / 64.0;
			auto y_offset = glyph_pos[i].y_offset / 64.0;
			auto x_advance = glyph_pos[i].x_advance / 64.0;
			auto y_advance = glyph_pos[i].y_advance / 64.0;
			draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
			printf("%d %f %f\n", glyphid, cursor_x + x_offset, cursor_y + y_offset);
			cursor_x += x_advance;
			cursor_y += y_advance;
		}
		hb_buffer_destroy(buf);
		// TODO destroy other stuff.
	}
}

void MenuMode::draw(glm::uvec2 const& drawable_size) {
	if (background) {
		std::shared_ptr< Mode > hold_me = shared_from_this();
		background->draw(drawable_size);
		//it is an error to remove the last reference to this object in background->draw():
		assert(hold_me.use_count() > 1);
	}
	else {
		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//use alpha blending:
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	//float bounce = (0.25f - (select_bounce_acc - 0.5f) * (select_bounce_acc - 0.5f)) / 0.25f * select_bounce_amount;

	{ //draw the menu using DrawSprites:
		/*assert(atlas && "it is an error to try to draw a menu without an atlas");
		DrawSprites draw_sprites(*atlas, view_min, view_max, drawable_size, DrawSprites::AlignPixelPerfect);*/

		float y_offset = 0.0f;
		for (auto const& item : items) {
			bool is_selected = (&item == &items[0] + selected);
			// TEMP text rendering. TODO - Replace with better text stuff
			float aspect = float(drawable_size.x) / float(drawable_size.y);
			DrawLines lines(glm::mat4(
				1.0f / aspect, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			));

			// Level/freeplay text
			//std::cout << "item.name: " << item.name << std::endl;
			constexpr float H = 0.2f;
			glm::u8vec4 color = (is_selected ? glm::u8vec4(0xff, 0xff, 0xff, 0x00) : glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			lines.draw_text(item.name,
				glm::vec3(-aspect + 0.1f * H, 1.0f - 1.1f * H + y_offset, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				color
			);

			y_offset -= 0.5f;
		}
	} //<-- gets drawn here!


	GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
}


// ----- Glyph & texture drawing util -----

// Draws a glyph
void MenuMode::draw_glyph(hb_codepoint_t glyph, double x, double y) {
	// TODO
}


// Loads a texture to OpenGL(?)
// Source - @wdlzz926 in #game4 in 15-466 Discord
GLuint texture_loading(std::vector<glm::u8vec4> tex_data, int width, int height) {
//GLuint MenuMode::texture_loading(std::vector<glm::u8vec4> tex_data, int width, int height) {
	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}



// DEPRECATED
//void MenuMode::layout_items(float gap) {
//	//DrawSprites temp(*atlas, view_min, view_max, view_max - view_min, DrawSprites::AlignPixelPerfect); //<-- doesn't actually draw
//	float y = (float)view_max.y;
//	for (auto& item : items) {
//		glm::vec2 min(0.0f), max(0.0f);
//		/*if (item.sprite) {
//			min = item.scale * (item.sprite->min_px - item.sprite->anchor_px);
//			max = item.scale * (item.sprite->max_px - item.sprite->anchor_px);
//		} else {
//			temp.get_text_extents(item.name, glm::vec2(0.0f), item.scale, &min, &max);
//		}*/
//		item.at.y = y - max.y;
//		item.at.x = 0.5f * (view_max.x + view_min.x) - 0.5f * (max.x + min.x);
//		y = y - (max.y - min.y) - gap;
//	}
//	float ofs = -0.5f * y;
//	for (auto& item : items) {
//		item.at.y += ofs;
//	}
//}