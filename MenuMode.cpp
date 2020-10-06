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

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

//color texture program from base code
#include "FontTextureProgram.hpp"

// TEMPORARY DrawLines for temp text rendering
#include "DrawLines.hpp"

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

  // Initialize font and shaping stuff

  //This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
  //You probably shouldn't be looking here to learn to use either library.
  // Source - @xiaoqiao in #game4 in 15-466 Discord
  // Source's source - https://harfbuzz.github.io/ch03s03.html
  {

    ft_error = FT_Init_FreeType(&library);
    if (ft_error) {
      // "A list of all FreeType error codes can be found in file fterrdef.h."
      //    - https://www.freetype.org/freetype2/docs/tutorial/step1.html
      throw std::runtime_error("Got an FT_Error while initializing the library  :(");
    }

    // Create a face and a font using Freetype
    ft_error = FT_New_Face(library, "E:/Coding/CMU/15466/game4/dist/Fonts/Oswald-Regular.ttf", 0, &face);
    if (ft_error == FT_Err_Unknown_File_Format) {
      throw std::runtime_error("The font file could be opened and read, but it appears that its font format is unsupported");
    } else if (ft_error) {
      throw std::runtime_error("The font file could not be opened or read, or it is broken");
    }

    font = hb_ft_font_create(face, NULL);

    // Actual text drawing used to go here
  }
}

MenuMode::~MenuMode() {
  hb_font_destroy(font);
  FT_Done_Face(face);
  FT_Done_FreeType(library);
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
  } else {
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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //don't use the depth test:
  glDisable(GL_DEPTH_TEST);


  // Render the menu text!

  FT_F26Dot6 font_size = 36;
  ft_error = FT_Set_Char_Size(face, 0, font_size * 64, (FT_UInt)DPI.x, (FT_UInt)DPI.y);
  if (ft_error) throw std::runtime_error("Error on FT_Set_Char_Size()");

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

  // Create a buffer and put your text in it
  hb_buffer_t* buf = hb_buffer_create();
  hb_buffer_add_utf8(buf, "Apple", -1, 0, -1);
  // Set the script, language, and direction of the buffer
  hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
  hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
  hb_buffer_set_language(buf, hb_language_from_string("en", -1));

  // Shape!
  hb_shape(font, buf, NULL, 0);
  // Get the glyph and position info
  unsigned int glyph_count;
  hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
  hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

  // Iterate over each glyph
  double cursor_x = 0.0, cursor_y = 64.0;
  for (unsigned int i = 0; i < glyph_count; ++i) {
    auto glyphid = glyph_info[i].codepoint;
    auto x_offset = glyph_pos[i].x_offset / 64.0;
    auto y_offset = glyph_pos[i].y_offset / 64.0;
    auto x_advance = glyph_pos[i].x_advance / 64.0;
    auto y_advance = glyph_pos[i].y_advance / 64.0;
    auto x = cursor_x + x_offset;
    auto y = cursor_y + y_offset;

    // Draw the glyph!
    ft_error = FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
    if (ft_error) throw std::runtime_error("Error on FT_Load_Glyph()");
    ft_error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if (ft_error) throw std::runtime_error("Error on FT_Render_Glyph()");

    // ft_bitmap - https://www.freetype.org/freetype2/docs/reference/ft2-basic_types.html#ft_bitmap
    GLuint tex_gluint = texture_loading(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows);

    glm::vec2 pos(x, y);
    glm::vec2 size(face->glyph->bitmap.width, face->glyph->bitmap.rows);
    glm::vec2 bearing(face->glyph->bitmap_left, face->glyph->bitmap_top);
    RenderCharTex(tex_gluint, pos, size, bearing, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), drawable_size);

    cursor_x += x_advance;
    cursor_y += y_advance;
  }
  // Tidy up
  hb_buffer_destroy(buf);




  // Text drawing test
  //RenderText("test", 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));


  GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
}


// ----- Glyph & texture drawing util -----

// Loads a texture to OpenGL(?)
// Source - @wdlzz926 in #game4 in 15-466 Discord
GLuint MenuMode::texture_loading(const void* tex_data, int width, int height) {
  //GLuint MenuMode::texture_loading(std::vector<glm::u8vec4> tex_data, int width, int height) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLuint tex;
  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, tex_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // Swizzling - thx to Kyle Jannak-Huang in #game4
  /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);*/
  glBindTexture(GL_TEXTURE_2D, 0);

  return tex;
}


// Source - https://learnopengl.com/In-Practice/Text-Rendering
void MenuMode::RenderCharTex(GLuint tex_gluint, glm::vec2 pos, glm::vec2 size, glm::vec2 bearing, float scale, glm::vec4 color, const glm::vec2 drawableSize) {
  // activate corresponding render state	
  glUseProgram(font_texture_program->program);
  
  glm::mat4 projection = glm::ortho(0.0f, drawableSize.x, 0.0f, drawableSize.y);
  glUniformMatrix4fv(font_texture_program->projection, 1, GL_FALSE, glm::value_ptr(projection));
  glUniform4f(font_texture_program->Color_vec4, color.r, color.g, color.b, color.a);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(font_texture_program->VAO);


  // draw the character

  float xpos = pos.x + bearing.x * scale;
  float ypos = pos.y - (size.y - bearing.y) * scale;

  float w = size.x * scale;
  float h = size.y * scale;
  // update VBO for each character
  float vertices[6][4] = {
      { xpos,     ypos + h,   0.0f, 0.0f },
      { xpos,     ypos,       0.0f, 1.0f },
      { xpos + w, ypos,       1.0f, 1.0f },

      { xpos,     ypos + h,   0.0f, 0.0f },
      { xpos + w, ypos,       1.0f, 1.0f },
      { xpos + w, ypos + h,   1.0f, 0.0f }
  };
  // render glyph texture over quad
  glBindTexture(GL_TEXTURE_2D, tex_gluint);
  // update content of VBO memory
  glBindBuffer(GL_ARRAY_BUFFER, font_texture_program->VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // render quad
  glDrawArrays(GL_TRIANGLES, 0, 6);
  // now advance cursors for next glyph (note that advance is number of 1/64 pixels)


  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}