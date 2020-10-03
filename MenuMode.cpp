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
}

void MenuMode::draw(glm::uvec2 const& drawable_size) {
	if (background) {
		std::shared_ptr< Mode > hold_me = shared_from_this();
		background->draw(drawable_size);
		//it is an error to remove the last reference to this object in background->draw():
		assert(hold_me.use_count() > 1);
	} else {
		//glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}


	//use alpha blending:
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);


	
	{ //draw the menu
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
	






	//This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
	//You probably shouldn't be looking here to learn to use either library.
	// Source - @xiaoqiao in #game4 in 15-466 Discord
	// Source's source - https://harfbuzz.github.io/ch03s03.html
	{
		FT_Library library;
		FT_Face face;
		FT_Error ft_error;

		ft_error = FT_Init_FreeType(&library);
		if (ft_error) {
			// "A list of all FreeType error codes can be found in file fterrdef.h."
			//    - https://www.freetype.org/freetype2/docs/tutorial/step1.html
			throw std::runtime_error("Got an FT_Error while initializing the library  :(");
		}

		// Create a buffer and put your text in it
		hb_buffer_t* buf = hb_buffer_create();
		hb_buffer_add_utf8(buf, "Apple", -1, 0, -1);
		// Set the script, language, and direction of the buffer
		hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
		hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
		hb_buffer_set_language(buf, hb_language_from_string("en", -1));

		// Create a face and a font using Freetype
		ft_error = FT_New_Face(library, "E:/Coding/CMU/15466/game4/dist/Fonts/OpenSans-Regular.ttf", 0, &face);
		if (ft_error == FT_Err_Unknown_File_Format) {
			throw std::runtime_error("The font file could be opened and read, but it appears that its font format is unsupported");
		}
		else if (ft_error) {
			throw std::runtime_error("The font file could not be opened or read, or it is broken");
		}

		ft_error = FT_Set_Char_Size(face, 0, 160 * 64, 96, 96); // TODO - update the horizontal & vertical dpi here... somehow
		if (ft_error) throw std::runtime_error("Error on FT_Set_Char_Size()");

		hb_font_t* font = hb_ft_font_create(face, NULL);
		// Shape!
		hb_shape(font, buf, NULL, 0);
		// Get the glyph and position info
		unsigned int glyph_count;
		hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
		hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
		// Iterate over each glyph
		double cursor_x = 0.0, cursor_y = 0.0;
		for (unsigned int i = 0; i < glyph_count; ++i) {
			auto glyphid = glyph_info[i].codepoint;
			auto x_offset = glyph_pos[i].x_offset / 64.0;
			auto y_offset = glyph_pos[i].y_offset / 64.0;
			auto x_advance = glyph_pos[i].x_advance / 64.0;
			auto y_advance = glyph_pos[i].y_advance / 64.0;
			auto x = cursor_x + x_offset;
			auto y = cursor_y + y_offset;
			// testing what draw_glyph() should do
			ft_error = FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
			if (ft_error) throw std::runtime_error("Error on FT_Load_Glyph()");
			ft_error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			if (ft_error) throw std::runtime_error("Error on FT_Render_Glyph()");
			// ft_bitmap - https://www.freetype.org/freetype2/docs/reference/ft2-basic_types.html#ft_bitmap
			GLuint tex_gluint = texture_loading(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows);


			draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
			cursor_x += x_advance;
			cursor_y += y_advance;
		}
		// Tidy up
		hb_buffer_destroy(buf);
		hb_font_destroy(font);
	}





	GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
}


// ----- Glyph & texture drawing util -----

// Draws a glyph
void MenuMode::draw_glyph(hb_codepoint_t glyph, double x, double y) {
	printf("%d %f %f\n", glyph, x, y);
	// TODO
}


// Loads a texture to OpenGL(?)
// Source - @wdlzz926 in #game4 in 15-466 Discord
GLuint MenuMode::texture_loading(const void *tex_data, int width, int height) {
//GLuint MenuMode::texture_loading(std::vector<glm::u8vec4> tex_data, int width, int height) {
	std::cout << "check0" << std::endl;
	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	std::cout << "check1" << std::endl;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	std::cout << "check2" << std::endl;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	std::cout << "check3" << std::endl;

	return tex;
}

