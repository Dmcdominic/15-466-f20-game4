/* demo_menu.cpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#include "story_menu.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include "PlayMode.hpp"
#include <iostream>


std::shared_ptr< MenuMode > story_menu;

Load< void > load_story_menu(LoadTagDefault, []() {
	// ----- Predefine all story nodes -----
	MenuMode::SNode *TitleScreen = new MenuMode::SNode();
	MenuMode::SNode *start = new MenuMode::SNode();


	// ----- Initialize their text and on_select values -----

	// Title Screen
	TitleScreen->items.emplace_back("PRIMORDIALS", MenuMode::ITEM_TYPE::TITLE);
	TitleScreen->items.emplace_back("          By Dominic Calkosz", MenuMode::ITEM_TYPE::STORY);
	TitleScreen->items.emplace_back(" ", MenuMode::ITEM_TYPE::STORY);
	TitleScreen->items.emplace_back("Play");
	TitleScreen->items.back().on_select = [start](MenuMode::Item const&) {
		//Mode::set_current(std::make_shared< PlayMode >());
		story_menu->setSNode(start);
	};
	TitleScreen->items.emplace_back("Quit");
	TitleScreen->items.back().on_select = [](MenuMode::Item const&) {
		story_menu->quit = true;
	};

	// The first story node
	start->items.emplace_back("You wake up in a world...", MenuMode::ITEM_TYPE::STORY);
	start->items.emplace_back("Go back to sleep");
	start->items.back().on_select = [TitleScreen](MenuMode::Item const&) {
		story_menu->setSNode(TitleScreen);
	};


	// ----- Initialize the menu's values -----
	story_menu = std::make_shared< MenuMode >(TitleScreen);
	story_menu->view_min = glm::vec2(0.0f, 0.0f);
	story_menu->view_max = glm::vec2(320.0f, 200.0f);
});