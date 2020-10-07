/* demo_menu.cpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#include "story_menu.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include "PlayMode.hpp"
#include <iostream>


std::shared_ptr< MenuMode > story_menu;

Load< void > load_story_menu(LoadTagDefault, reset_story);


void reset_story() {
	std::cout << "reset_story() called" << std::endl;
	// ----- Predefine all story nodes -----
	MenuMode::SNode* TitleScreen = new MenuMode::SNode();

	MenuMode::SNode* start = new MenuMode::SNode();

	MenuMode::SNode* peak0 = new MenuMode::SNode();
	MenuMode::SNode* peak1 = new MenuMode::SNode();
	MenuMode::SNode* peakChoice = new MenuMode::SNode();
	MenuMode::SNode* peak3 = new MenuMode::SNode();

	MenuMode::SNode* trunk0 = new MenuMode::SNode();
	MenuMode::SNode* trunk1 = new MenuMode::SNode();
	MenuMode::SNode* trunk2 = new MenuMode::SNode();
	MenuMode::SNode* trunk3 = new MenuMode::SNode();
	MenuMode::SNode* trunk4 = new MenuMode::SNode();
	MenuMode::SNode* trunk5 = new MenuMode::SNode();

	MenuMode::SNode* sand0 = new MenuMode::SNode();
	MenuMode::SNode* sand1 = new MenuMode::SNode();
	MenuMode::SNode* sand2 = new MenuMode::SNode();
	MenuMode::SNode* sand3 = new MenuMode::SNode();
	MenuMode::SNode* sand4 = new MenuMode::SNode();


	// ----- Initialize their text and on_select values -----

	// --- Title Screen ---
	TitleScreen->items.emplace_back("PRIMORDIALS", MenuMode::ITEM_TYPE::TITLE);
	TitleScreen->items.emplace_back("          Created by Dominic Calkosz", MenuMode::ITEM_TYPE::STORY);
	TitleScreen->items.emplace_back(" ", MenuMode::ITEM_TYPE::STORY);
	TitleScreen->items.emplace_back("Play");
	TitleScreen->items.back().on_select = [start](MenuMode::Item const&) {
		story_menu->initStoryState();
		//Mode::set_current(std::make_shared< PlayMode >());
		story_menu->setSNode(start);
	};
	TitleScreen->items.emplace_back("Quit");
	TitleScreen->items.back().on_select = [](MenuMode::Item const&) {
		story_menu->quit = true;
	};


	// --- The Peak ---
	start->items.emplace_back("You grab the last handhold and pull yourself up onto the ledge. This is it... You've finally reached the peak.", MenuMode::ITEM_TYPE::STORY);
	start->items.emplace_back("Continue");
	start->items.back().on_select = [peak0](MenuMode::Item const&) {
		story_menu->setSNode(peak0);
	};

	peak0->items.emplace_back("At the other end of the summit looms a single evergreen. On the lowest branch, a tall and brawny person yawns and stretches both arms, then jumps down. "
		"They stand in front of the tree, arms folded behind their back. \"I'm sure you'd like a break at this point, but unfortunately there's no time for hesitation now.\"", MenuMode::ITEM_TYPE::STORY);
	peak0->items.emplace_back("Continue");
	peak0->items.back().on_select = [peak1](MenuMode::Item const&) {
		story_menu->setSNode(peak1);
	};

	peak1->items.emplace_back("\"In the presence of Primordials, your mental and physical being will deteriorate. You may be experiencing it already. "
		"But that's all I can explain for now.\"", MenuMode::ITEM_TYPE::STORY);
	peak1->items.emplace_back("Continue");
	peak1->items.back().on_select = [peakChoice](MenuMode::Item const&) {
		story_menu->setSNode(peakChoice);
	};

	peakChoice->items.emplace_back("\"Before you can continue, you'll need one of these.\" In front of you, two objects emerge from the ground: A small ballpoint pen, and a steel broadsword.", MenuMode::ITEM_TYPE::STORY);
	peakChoice->items.emplace_back("Pick up the pen");
	peakChoice->items.back().on_select = [peak3](MenuMode::Item const&) {
		story_menu->pen = true;
		peak3->items.at(0).name = "\"The pen is mightier than the sword, after all. " + peak3->items.at(0).name;
		story_menu->setSNode(peak3);
	};
	peakChoice->items.emplace_back("Pick up the sword");
	peakChoice->items.back().on_select = [peak3](MenuMode::Item const&) {
		story_menu->sword = true;
		peak3->items.at(0).name = "\"A double-edged blade, but a sturdy tool nonetheless. " + peak3->items.at(0).name;
		story_menu->setSNode(peak3);
	};

	peak3->items.emplace_back("Onwards, then.\" They leap back up onto their branch as an opening within the trunk swings open.", MenuMode::ITEM_TYPE::STORY);
	peak3->items.emplace_back("Step into the evergreen");
	peak3->items.back().on_select = [trunk0](MenuMode::Item const&) {
		story_menu->setSNode(trunk0);
	};


	// --- The Trunk ---
	trunk0->items.emplace_back("You step through the opening and discover a space behind which is much larger than the tree itself. "
	                           "The walls and floor are a pristine gold color. Large violet pillars hold up a high ceiling with a mosaic of several humanoids, each one smaller than the next.", MenuMode::ITEM_TYPE::STORY);
	trunk0->items.emplace_back("Continue");
	trunk0->items.back().on_select = [trunk1](MenuMode::Item const&) {
		story_menu->setSNode(trunk1);
	};

	trunk1->items.emplace_back("At the center is a person five times your height, floating cross-legged just above the ground. "
		"Exquisite silk garbs sway and twirl slowly around them as they gaze into your eyes, into your mind.", MenuMode::ITEM_TYPE::STORY);
	trunk1->items.emplace_back("Continue");
	trunk1->items.back().on_select = [trunk2](MenuMode::Item const&) {
		story_menu->setSNode(trunk2);
	};

	trunk2->items.emplace_back("\"Feel the time flow around you... Feel my energy flow through you...\"", MenuMode::ITEM_TYPE::STORY);
	trunk2->items.emplace_back("Continue");
	trunk2->items.back().on_select = [trunk3](MenuMode::Item const&) {
		story_menu->setSNode(trunk3);
	};

	trunk3->items.emplace_back("You notice an open archway on each side of the far wall. \"My presence is all you can desire. Embrace it.\"", MenuMode::ITEM_TYPE::STORY);
	trunk3->items.emplace_back("Stay");
	trunk3->items.back().on_select = [trunk4](MenuMode::Item const&) {
		story_menu->embraced = true;
		story_menu->setSNode(trunk4);
	};
	trunk3->items.emplace_back("Go through the left archway");
	trunk3->items.back().on_select = [sand0](MenuMode::Item const&) {
		story_menu->setSNode(sand0);
	};
	trunk3->items.emplace_back("Go through the right archway");
	trunk3->items.back().on_select = [sand0](MenuMode::Item const&) {
		story_menu->setSNode(sand0);
	};

	trunk4->items.emplace_back("You sit down cross-legged. Their gaze is unwavering.", MenuMode::ITEM_TYPE::STORY);
	trunk4->items.emplace_back("Stay");
	trunk4->items.back().on_select = [trunk5](MenuMode::Item const&) {
		story_menu->setSNode(trunk5);
	};
	trunk4->items.emplace_back("Go through the left archway");
	trunk4->items.back().on_select = [sand0](MenuMode::Item const&) {
		story_menu->setSNode(sand0);
	};
	trunk4->items.emplace_back("Go through the right archway");
	trunk4->items.back().on_select = [sand0](MenuMode::Item const&) {
		story_menu->setSNode(sand0);
	};

	trunk5->items.emplace_back("You unfold your legs. Their gaze is unwavering.", MenuMode::ITEM_TYPE::STORY);
	trunk5->items.emplace_back("Stay");
	trunk5->items.back().on_select = [trunk4](MenuMode::Item const&) {
		story_menu->setSNode(trunk4);
	};
	trunk5->items.emplace_back("Go through the left archway");
	trunk5->items.back().on_select = [sand0](MenuMode::Item const&) {
		story_menu->setSNode(sand0);
	};
	trunk5->items.emplace_back("Go through the right archway");
	trunk5->items.back().on_select = [sand0](MenuMode::Item const&) {
		story_menu->setSNode(sand0);
	};


	// --- The Sand ---
	sand0->items.emplace_back("You step through the archway and discover another large indoor space. It seems to be constructed mainly from bamboo and straw. "
	                          "The floor is an endless grid of square boxes. Leaning over the edge of the nearest one, you see that it is filled with sand, smoothed across the surface.", MenuMode::ITEM_TYPE::STORY);
	sand0->items.emplace_back("Continue");
	sand0->items.back().on_select = [sand1](MenuMode::Item const&) {
		story_menu->setSNode(sand1);
	};

	sand1->items.emplace_back("Your turn to the left and find yourself next to another massive humanoid, this one dwarfing the last, at least 50 meters in height. "
		"They are kneeling over one of the boxes, using a long stick of bamboo to carefully create strange patterns in the sand.", MenuMode::ITEM_TYPE::STORY);
	sand1->items.emplace_back("Continue");
	sand1->items.back().on_select = [sand2](MenuMode::Item const&) {
		story_menu->setSNode(sand2);
	};

	sand2->items.emplace_back("With their other hand, they reach into a huge basin of watermelons, then pop one into their mouth like a grape. "
		                        "They reach for another one, this time knocking an extra melon out of the basin. It rolls to a stop near your feet.", MenuMode::ITEM_TYPE::STORY);
	sand2->items.emplace_back("Draw/cut option 1");
	sand2->items.back().on_select = [sand3](MenuMode::Item const&) {
		story_menu->setSNode(sand3);
	};
	sand2->items.emplace_back("Draw/cut option 2");
	sand2->items.back().on_select = [sand3](MenuMode::Item const&) {
		story_menu->setSNode(sand3);
	};


	// ----- Initialize the menu's values -----
	if (!story_menu) {
		story_menu = std::make_shared< MenuMode >(TitleScreen);
	}

	story_menu->view_min = glm::vec2(0.0f, 0.0f);
	story_menu->view_max = glm::vec2(320.0f, 200.0f);

	story_menu->setSNode(TitleScreen);
	story_menu->TitleScreen = TitleScreen;
	story_menu->initStoryState();
}
