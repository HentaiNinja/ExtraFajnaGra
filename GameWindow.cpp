#include "GameWindow.h"
#ifdef _WIN32
//#include <vld.h>
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <stdexcept>
#include <memory>
#include <string>
#include <set>
#include <tuple>
#include <iostream>
#include <cstdint>
#include <vector>
#include <list>
#include <random>

GameWindow::GameWindow()
{
	init();
}


GameWindow::~GameWindow()
{
	delete playable_rocket;
	delete animation_rocket;
}

void GameWindow::init()
{
	std::string path = "resources/";
	window = init_window(screen_width, screen_height);
	renderer = init_renderer(window);

	background_texture = create_texture(path + "background.bmp");
	rocket_texture = create_texture(path + "rocket.bmp", true);

	for (int i = 0; i <= 2; i++) {
		comet_textures.push_back(create_texture(path + "comet0" + std::to_string(i) + ".bmp", true));
	}

	start_game_button_texture = create_texture(path + "start.bmp", true);
	start_game_button_background_texture = create_texture(path + "start_bg.bmp", true);

	bool hacks = false;
	if (!hacks)
	{
		comets.push_back(*create_object(comet_size, comet_size, Physics({ (double)-comet_size * 2, (double)40 }), NULL).get());
		comets.push_back(*create_object(comet_size, comet_size, Physics({ (double)-comet_size * 2, (double)40 }), NULL).get());
		comets.push_back(*create_object(comet_size, comet_size, Physics({ (double)-comet_size * 2, (double)40 }), NULL).get());
		manage_objects(&comets, &comet_textures, true, false);
	}

	animation_comets.push_back(*create_object(comet_size, comet_size, Physics({ (double) -comet_size * 2, (double) 40 }), NULL).get());
	animation_comets.push_back(*create_object(comet_size, comet_size, Physics({ (double) -comet_size * 2, (double) 40 }), NULL).get());
	animation_comets.push_back(*create_object(comet_size, comet_size, Physics({ (double) -comet_size * 2, (double) 40 }), NULL).get());
	animation_comets.push_back(*create_object(comet_size, comet_size, Physics({ (double) -comet_size * 2, (double) 40 }), NULL).get());
	manage_objects(&animation_comets, &comet_textures, true, false);

	std::list<std::shared_ptr<SDL_Texture>>::iterator planet_texture;
	planet_textures.push_back(create_texture(path + "planet00.bmp", true, 0, 255, 0));
	planet_textures.push_back(create_texture(path + "planet01.bmp", true, 0, 255, 0));
	planet_textures.push_back(create_texture(path + "planet02.bmp", true, 0, 255, 0));
	planet_texture = planet_textures.begin();
	planets.push_back(*create_object(60, 60, Physics({ 0.0, (double) screen_height / 4 }, { 3.0, 0.0 }), planet_texture->get()).get());
	planet_texture++;
	planets.push_back(*create_object(100, 100, Physics({ (double) screen_width / 2, (double) screen_height / 2 }, { 2.0, 0.0 }), planet_texture->get()).get());
	planets.push_back(*create_object(100, 100, Physics({ (double) screen_width / 2, (double) screen_height / 2 }, { 2.0, 0.0 }), planet_texture->get()).get());
	planet_texture++;
	planets.push_back(*create_object(112, 112, Physics({ (double) screen_width - 112, (double) 3 * screen_height / 4 }, { 1.0, 0.0 }), planet_texture->get()).get());
	manage_objects(&planets, &planet_textures, true, false, true);

	playable_rocket = new GameObject(rocket_size, rocket_size, Physics({ (double) (screen_width - rocket_size) / 2, (double) screen_height }), rocket_texture.get());
	animation_rocket = new GameObject(rocket_size, rocket_size, Physics({ (double) (screen_width - rocket_size) / 2, 0.0 }, { -1.0, -1.0 }), rocket_texture.get());

	pause_texture = create_texture(path + "pause.bmp", true, 0, 255, 0);

	game_state = GameState::Closed;

	particle_texture = create_texture(path + "particle.bmp", true);

	path = "text/";

	points_string_texture = create_texture(path + "text_score.bmp", true, 0, 255, 0);
	points_string_game_obj = create_object(72, 20, Physics(), points_string_texture.get());

	std::list<std::shared_ptr<SDL_Texture>>::iterator it = number_game_objs_test.begin();
	for (int i = 0; i <= 9; i++) {
		number_game_objs_test.push_back(create_texture(path + std::to_string(i) + ".bmp", true, 0, 255, 0));
		it = number_game_objs_test.begin();
		std::advance(it, i);
		number_game_objs.push_back(create_object(10, 20, Physics(), it->get() ));
	}

	restart_game();

}

void GameWindow::restart_game()
{
	partciles.clear();
	bg_physics.velocity[1] = 0.24555;
	playable_rocket->physics.position = { (double) (screen_width - rocket_size) / 2, (double) screen_height };
	playable_rocket->physics.velocity = { 0, 0 };
	playable_rocket->physics.acceleration = { 0,0 };
	points = 0;
	manage_objects(&comets, &comet_textures, true, false);
	kb_han.clear_events();
}

void GameWindow::changeRocketPosition(int delta_ticks) {
	playable_rocket->physics.move(delta_ticks, true);
}

void GameWindow::animate_background(bool anim_rocket) {
	bg_physics.move(current_tick - last_tick);
	if ((int)bg_physics.position[1] >= screen_height)
		bg_physics.position[1] = 0.0;
	SDL_Rect dst_bg = { 0, (int)bg_physics.position[1] - screen_height, screen_width, screen_height };
	SDL_RenderCopy(renderer.get(), background_texture.get(), NULL, &dst_bg);
	SDL_Rect dst_bg2 = { 0, (int)bg_physics.position[1], screen_width, screen_height };
	SDL_RenderCopy(renderer.get(), background_texture.get(), NULL, &dst_bg2);
	if (game_state != GameState::Paused)
	{
		apply_speed_to_objects(&planets, current_tick - last_tick);
		manage_objects(&planets, &planet_textures, false, false, true);
	}
	draw_objects(&planets);
	draw_particles();
	manage_particles();
	if (anim_rocket) {
		apply_speed_to_objects(&animation_comets, current_tick - last_tick);
		manage_objects(&animation_comets, &comet_textures, false, false);
		draw_objects(&animation_comets);
		
		animation_rocket->physics.acceleration[0] += (random(0, 100) - 50) / 992345.333;
		animation_rocket->physics.acceleration[1] += (random(0, 100) - 50) / 992345.333;

		SDL_Rect rocket_position = { (int) animation_rocket->i(), (int) animation_rocket->j(), rocket_size, rocket_size };
		SDL_RenderCopy(renderer.get(), rocket_texture.get(), NULL, &rocket_position);
		animation_rocket->physics.move(current_tick - last_tick);
		if (animation_rocket->i() >= screen_width - rocket_size) {
			animation_rocket->i(screen_width - rocket_size);
			animation_rocket->physics.velocity[0] = -0.1;
			animation_rocket->physics.acceleration[0] = -abs(animation_rocket->physics.acceleration[0]) / 2.2;
		}
		if (animation_rocket->i() <= 0.0) {
			animation_rocket->i(0.0);
			animation_rocket->physics.velocity[0] = 0.1;
			animation_rocket->physics.acceleration[0] = abs(animation_rocket->physics.acceleration[0]) / 2.2;
		}
		if (animation_rocket->j() >= screen_height - rocket_size) {
			animation_rocket->j(screen_height - rocket_size);
			animation_rocket->physics.velocity[1] = -0.1;
			animation_rocket->physics.acceleration[1] = -abs(animation_rocket->physics.acceleration[1]) / 2.2;
		}
		if (animation_rocket->j() <= 0) {
			animation_rocket->j(0.0);
			animation_rocket->physics.velocity[1] = 0.1;
			animation_rocket->physics.acceleration[1] = abs(animation_rocket->physics.acceleration[1]) / 2.2;
		}
		add_particles(animation_rocket);
	}
}

void GameWindow::show_start_game_window()
{
	SDL_Event event;
	bool button_clicked = false;
	for (bool game_active = true; game_active; ) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) game_active = false;
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_SPACE:
					restart_game();
					begin();
					break;
				case SDLK_ESCAPE:
					game_active = false;
					break;
				}
			}
		}
		SDL_RenderClear(renderer.get());
		current_tick = SDL_GetTicks();
		animate_background(true);
		SDL_Rect size = { 0, 3 * (screen_height - start_button_height) / 4, screen_width, start_button_height };
		SDL_RenderCopy(renderer.get(), start_game_button_background_texture.get(), NULL, &size);

		size = { (screen_width - start_button_width) / 2, 3 * (screen_height - start_button_height) / 4, start_button_width, start_button_height };
		SDL_RenderCopy(renderer.get(), start_game_button_texture.get(), NULL, &size);
		last_tick = current_tick;
		SDL_RenderPresent(renderer.get());
		SDL_Delay(1);
	}
}

void GameWindow::begin() {
	SDL_Event event;
	current_tick = SDL_GetTicks();
	last_tick = SDL_GetTicks();
	game_state = GameState::Ongoing;
	for (; game_state != GameState::Closed; ) {
		while (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT) game_state = GameState::Closed;
			if (event.type == SDL_KEYDOWN) 
			{
				switch (event.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_DOWN:
				case SDLK_RIGHT:
				case SDLK_LEFT:
					kb_han.handle(event);
					break;
				case SDLK_SPACE:
					if (game_state == GameState::Paused)
						game_state = GameState::Ongoing;
					else if (game_state == GameState::Ongoing)
						game_state = GameState::Paused;
					break;
				case SDLK_ESCAPE:
					game_state = GameState::Closed;
					break;
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_DOWN:
				case SDLK_RIGHT:
				case SDLK_LEFT:
					kb_han.handle(event);
					break;
				}
			}
		}
		if (kb_han.is_pressed(SDLK_UP))
			playable_rocket->physics.acceleration[1] -= 0.00009 * 4.0;
		if (kb_han.is_pressed(SDLK_DOWN))
			playable_rocket->physics.acceleration[1] += 0.00009 * 4.0;
		if (kb_han.is_pressed(SDLK_RIGHT))
			playable_rocket->physics.acceleration[0] += 0.00009 * 4.0;
		if (kb_han.is_pressed(SDLK_LEFT))
			playable_rocket->physics.acceleration[0] -= 0.00009 * 4.0;
		double limit = 0.133;
		if(abs(playable_rocket->physics.velocity[0]) > limit || abs(playable_rocket->physics.velocity[1]) > limit)
			add_particles(playable_rocket);

		SDL_RenderClear(renderer.get());
		current_tick = SDL_GetTicks();

		animate_background(false);

		SDL_Rect rocket_position = { (int) playable_rocket->i(), (int) playable_rocket->j(), rocket_size, rocket_size };
		SDL_RenderCopy(renderer.get(), rocket_texture.get(), NULL, &rocket_position);


		if (game_state != GameState::Paused)
			changeRocketPosition(current_tick - last_tick);
		if (game_state != GameState::Paused)
			apply_speed_to_objects(&comets, current_tick - last_tick);

		draw_objects(&comets);

		if (playable_rocket->i() >= screen_width - rocket_size) {
			playable_rocket->i(screen_width - rocket_size);
		}
		if (playable_rocket->i() <= 0) {
			playable_rocket->i(0);
		}
		if (playable_rocket->j() >= screen_height - rocket_size) {
			playable_rocket->j(screen_height - rocket_size);
		}
		if (playable_rocket->j() <= 0) {
			playable_rocket->j(0);
		}

		if (rocket_hit_comet())
		{
			std::cout << "You have crashed!" << std::endl;
			std::cout << "You earned " << points << " points." << std::endl;
			display_score(false);
			points = 0;
			game_state = GameState::Closed;
			restart_game();
		}
		manage_objects(&comets, &comet_textures, false, true);
		if (game_state == GameState::Paused)
		{
			SDL_Rect blur;
			int blur_times = 4;
			for (int i = 0; i < blur_times; i++) {
				for (int j = 0; j < blur_times; j++) {
					blur = { i * screen_width / blur_times, j * screen_height / blur_times, screen_width / blur_times, screen_height / blur_times };
					SDL_RenderCopy(renderer.get(), start_game_button_background_texture.get(), NULL, &blur);
					//SDL_Delay(500);
					//SDL_RenderPresent(renderer.get());
				}
			}
			

			blur = { (screen_width - start_button_width) / 2, 3 * (screen_height - start_button_height) / 4, start_button_width, start_button_height };
			SDL_RenderCopy(renderer.get(), pause_texture.get(), NULL, &blur);
		}
		display_score();
		last_tick = current_tick;
		SDL_RenderPresent(renderer.get());
		SDL_Delay(1);
	}
}

void GameWindow::manage_objects(std::list<GameObject>* objects, std::list<std::shared_ptr<SDL_Texture>>* textures, bool should_force, bool apply_points, bool planet /*false*/) {
	std::list<GameObject>::iterator it = objects->begin();
	for (; it != objects->end(); it++) {
		if (object_is_out_of_screen(&(*it)) || should_force)
		{
			if (!should_force && apply_points) {
				points += (int) abs(it->physics.velocity[0] * 1000.0); 
			}
			randomize_object(&(*it), objects, textures, planet);
		}
	}
}

void GameWindow::randomize_object(GameObject * object, std::list<GameObject>* objects, std::list<std::shared_ptr<SDL_Texture>>* textures, bool planet /*false*/)
{
	double direction = random(0, 1) == 0 ? 1.0 : -1.0;
	double velocity = (double)random(min_comet_speed, max_comet_speed) / 2845.0;
	if (planet)
		velocity = velocity / ( (double) ( (random(200,350) / 100.0) ) );
	velocity = velocity * direction;
	if (!planet)
		object->physics.velocity = { velocity, 0 };
	else
		object->physics.velocity = { 0, abs(velocity) / 2845.0 };
	if (planet)
	{
		object->width = random(60, 112);
		object->height = object->width;
		object->physics.velocity = { 0, abs(velocity) * ((double) object->width / 150.0) };
	}
	if (!planet)
	{
		int new_height = random(0, screen_height - object->height);
		if (velocity > 0)
			object->physics.position = { (double)-object->width, (double)new_height };
		else
			object->physics.position = { (double)screen_width + object->width, (double)new_height };
	}
	else
	{
		int new_width = random(0, screen_width - object->width);
		//if (velocity > 0)
			object->physics.position = { (double)new_width, (double)-object->height };
		//else
		//	object->physics.position = { (double)new_width, (double)object->height + screen_height };
	}
	std::list<std::shared_ptr<SDL_Texture>>::iterator texture;
	texture = textures->begin();
	std::advance(texture, random(0, textures->size() - 1));
	object->texture = texture->get();
}

int GameWindow::random(int min, int max)
{
	static std::random_device rd; // obtain a random number from hardware
	static std::mt19937 eng(rd()); // seed the generator
	if (max < min) {
		int pom = max;
		max = min;
		min = pom;
	}
	std::uniform_int_distribution<> dist(min, max); // define the range
	return dist(eng);
}

bool GameWindow::there_is_a_object_on_this_height(int height, int object_height, std::list<GameObject>* objects)
{
	bool is_there_a_object_on_height = false;
	std::list<GameObject>::iterator it = objects->begin();
	for (; it != objects->end(); it++) {
		if (!object_is_out_of_screen(&(*it)))
		{
			if (it->j() <= height + object_height && it->j() + it->height >= height)
			{
				is_there_a_object_on_height = true;
			}
			else if (it->j() <= height + object_height && it->j() + it->height >= height)
			{
				is_there_a_object_on_height = true;
			}
		}
	}
	return is_there_a_object_on_height;
}

bool GameWindow::object_is_out_of_screen(GameObject* object)
{
	int pom = 10;
	bool out_of_screen = false;
	if (object->i() > screen_width + object->width + pom)
		out_of_screen = true;
	if (object->i() < -object->width - pom)
		out_of_screen = true;
	if (object->j() < -object->height - pom)
		out_of_screen = true;
	if (object->j() > screen_height + object->height + pom)
		out_of_screen = true;
	return out_of_screen;
}

void GameWindow::draw_objects(std::list<GameObject>* objects) {
	std::list<GameObject>::iterator it = objects->begin();
	for (; it != objects->end(); it++) {
		SDL_Rect object_position = { (int) it->i(), (int) it->j(), it->width, it->height};
		SDL_RenderCopy(renderer.get(), it->texture, NULL, &object_position);
	}
}

//lista
void GameWindow::apply_speed_to_objects(std::list<GameObject>* comets, int delta_ticks) {
	std::list<GameObject>::iterator comet = comets->begin();
	for (; comet != comets->end(); comet++) {
		apply_speed_to_object(&(*comet), delta_ticks);
	}
}

//pojedynczy obiekt
void GameWindow::apply_speed_to_object(GameObject* object, int delta_ticks) {
	object->physics.move(delta_ticks);
}

void GameWindow::resize_planets()
{
	int sizes[3][2] = { {40, 40}, {70, 70}, {100, 100} };
	std::list<GameObject>::iterator planet = planets.begin();
	int rand;
	for (; planet != planets.end(); planet++) {
		rand = random(0, 2);
		planet->width = sizes[rand][0];
		planet->height = sizes[rand][1];
	}
}

bool GameWindow::rocket_hit_comet() {
	std::list<GameObject>::iterator comet = comets.begin();
	bool colision = false;
	for (; comet != comets.end(); comet++) {
		if (objects_collide(playable_rocket, &(*comet)))
			colision = true;
	}
	return colision;
}

bool GameWindow::objects_collide(GameObject* obj1, GameObject* obj2)
{
	bool colision = false;
	if (obj1->j() <= obj2->j() + obj2->height && obj1->j() + obj1->height >= obj2->j())
		if (obj1->i() <= obj2->i() + obj2->width && obj1->i() + obj1->height >= obj2->i())
			colision = true;
	return colision;
}

void GameWindow::display_score(bool display_points /*true*/) {
	SDL_Rect size;

	int horizontal_offset = 0;
	int vertical_offset = 0;
	if (!display_points)
	{
		horizontal_offset = screen_width / 2 - points_string_game_obj->width;
		vertical_offset = screen_height / 2;
	}
	size = { horizontal_offset + (int)points_string_game_obj->i(), vertical_offset + (int)points_string_game_obj->j(), points_string_game_obj->width, points_string_game_obj->height };
	SDL_RenderCopy(renderer.get(), points_string_game_obj->texture, NULL, &size);

	int copy = points;
	std::list<int> pointss;
	do {
		pointss.push_front(copy % 10);
		copy /= 10;
	} while (copy > 0);
	std::list<int>::iterator pointsss = pointss.begin();
	std::list<std::shared_ptr<GameObject>>::iterator number = number_game_objs.begin();
	for (int i = 0; pointsss != pointss.end(); pointsss++, i++) {
		number = number_game_objs.begin();
		std::advance(number, *pointsss);
		int pos_i = (int)(horizontal_offset + i * number->get()->width + points_string_game_obj->width + 4 * (i + 1));
		int pos_j = (int)(vertical_offset + number->get()->j());
		size = { pos_i, pos_j, number->get()->width, number->get()->height };
		SDL_RenderCopy(renderer.get(), number->get()->texture , NULL, &size);
	}

	if (!display_points)
	{
		SDL_RenderPresent(renderer.get());
		SDL_Delay(2000);
	}
}

void GameWindow::add_particles(GameObject* rocket)
{
	int rand = random(15, 20);
	for (int i = 0; i <= rand; i++) {
		partciles.push_back(create_particle(rocket));
	}
}

void GameWindow::draw_particles()
{
	std::list<Particle>::iterator part = partciles.begin();
	for (; part != partciles.end(); part++)
	{
		part->physics.move(current_tick - last_tick);
		draw_particle(&(*part));
	}
}

void GameWindow::draw_particle(Particle *part)
{
	SDL_SetRenderDrawColor(renderer.get(), part->r, part->g, part->b, part->a);
	int i = (int)part->i();
	int j = (int)part->j();
	for (int w = i + 1; w <= i + 3; w++) {
		for (int h = j + 1; h <= j + 3; h++) {
			SDL_RenderDrawPoint(renderer.get(), w, h);
		}
	}
	SDL_RenderDrawPoint(renderer.get(), i + 2, j);
	SDL_RenderDrawPoint(renderer.get(), i, j + 2);
	SDL_RenderDrawPoint(renderer.get(), i + 2, j + 4);
	SDL_RenderDrawPoint(renderer.get(), i + 4, j + 2);
}

void GameWindow::manage_particles()
{
	std::list<Particle>::iterator part = partciles.begin();
	int now = SDL_GetTicks();
	for (; part != partciles.end(); )
	{
		if (part->deathtime < now)
		{
			part = partciles.erase(part);
		}
		else
		{
			apply_speed_to_object( &((GameObject)*part), current_tick - last_tick);
			part++;
		}
	}
}

Particle GameWindow::create_particle(GameObject* rocket)
{
	int rand = random(0, 1);
	int rocket_engine_width = 11;
	int engine_height = (int)rocket->j() + rocket->height + rocket_engine_width / 2 - 14;
	int j = engine_height;
	int left_engine = (int)rocket->i() + rocket_engine_width / 2;
	int right_engine = (int)rocket->i() + rocket->width - rocket_engine_width + 1;
	int i = rand == 0 ? left_engine : right_engine ;



	double acc_scale = 5100.91;
	int offset_i = random(-rocket_engine_width/2, rocket_engine_width/2);
	int offset_j = random(-rocket_engine_width/2, rocket_engine_width/2);
	i += offset_i;
	j += offset_j;

	double splash = 2.87;
	double hor_speed = -rocket->physics.acceleration[0];
	double hor_speed_offset = random((int)-(rocket->physics.acceleration[1] * 2345.0), (int)(rocket->physics.acceleration[1] * 2345.0)) / (2345.0*splash);
	hor_speed += hor_speed_offset;
	double vert_speed = -rocket->physics.acceleration[1];
	double vert_speed_offset = random((int)-(rocket->physics.acceleration[0] * 2345.0), (int)(rocket->physics.acceleration[0] * 2345.0)) / (2345.0*splash);
	vert_speed += vert_speed_offset;

	double violet_limit_i_upper = -rocket->physics.acceleration[0] - (-rocket->physics.acceleration[0] / (2 * splash));
	double violet_limit_j_upper = -rocket->physics.acceleration[1] - (-rocket->physics.acceleration[1] / (2 * splash));

	double violet_limit_i_lower = -rocket->physics.acceleration[0] + (-rocket->physics.acceleration[0] / (2 * splash));
	double violet_limit_j_lower = -rocket->physics.acceleration[1] + (-rocket->physics.acceleration[1] / (2 * splash));

	bool violet = violet_limit_i_upper <= hor_speed && violet_limit_i_lower >= hor_speed ? true : false;
	violet = violet_limit_j_upper <= vert_speed && violet_limit_j_lower >= vert_speed ? violet : false;
	int r = violet ? 229 : 249;
	int g = violet ? 160 : 222;
	int b = violet ? 64 : 64;

	hor_speed = hor_speed * 13;
	vert_speed = vert_speed * 13;
	int deathtime = random(800, 1300) + SDL_GetTicks();
	double acc_i = -hor_speed / 1700.0;
	double acc_j = -vert_speed / 1700.0;
	Particle p = Particle(5, 5, Physics({ (double)i, (double)j }, { hor_speed,  vert_speed }, { acc_i , acc_j}), NULL, r, g, b, 50, deathtime);
	return p;
}

std::shared_ptr<GameObject> GameWindow::create_object(int width, int height, Physics physics, SDL_Texture* texture)
{
	GameObject *com = new GameObject(width, height, physics, texture);
	std::shared_ptr<GameObject> comet(com, [](GameObject* com) {
		delete com;
	});
	return comet;
}
void GameWindow::errthrow(const std::string &e) {
	std::string errstr = e + " : " + SDL_GetError();
	SDL_Quit();
	throw std::runtime_error(errstr);
};

std::shared_ptr<SDL_Window> GameWindow::init_window(const int width = 640, const int height = 480) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) errthrow("SDL_Init");

	SDL_Window *win = SDL_CreateWindow("Discount Galaga",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, SDL_WINDOW_SHOWN);
	if (win == nullptr) errthrow("SDL_CreateWindow");
	std::shared_ptr<SDL_Window> window(win, [](SDL_Window * ptr) {
		SDL_DestroyWindow(ptr);
	});
	return window;
}

std::shared_ptr<SDL_Renderer> GameWindow::init_renderer(std::shared_ptr<SDL_Window> window) {
	SDL_Renderer *ren = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) errthrow("SDL_CreateRenderer");
	std::shared_ptr<SDL_Renderer> renderer(ren, [](SDL_Renderer * ptr) {
		SDL_DestroyRenderer(ptr);
	});
	return renderer;
}

std::shared_ptr<SDL_Surface> GameWindow::create_surface(std::string file_name) {
	SDL_Surface *surf = SDL_LoadBMP(file_name.data());
	if (surf == nullptr) errthrow("SDL_CreateSurface");
	std::shared_ptr<SDL_Surface> surface(surf, [](SDL_Surface *surf) {
		SDL_FreeSurface(surf);
	});
	return surface;
}

std::shared_ptr<SDL_Texture> GameWindow::create_texture_from_surface(SDL_Renderer* render, SDL_Surface* surface) {
	SDL_Texture *tex = SDL_CreateTextureFromSurface(render, surface);
	if (tex == nullptr) errthrow("SDL_CreateTexture");
	std::shared_ptr<SDL_Texture> texture(tex, [](SDL_Texture *tex) {
		SDL_DestroyTexture(tex);
	});
	return texture;
}

std::shared_ptr<SDL_Texture> GameWindow::create_texture(std::string resource_path, bool apply_alhpa /*false*/, int r /*0*/, int g /*255*/, int b /*0*/)
{
	std::shared_ptr<SDL_Surface> surface = create_surface(resource_path);
	if (apply_alhpa)
	{
		Uint32 pixel = SDL_MapRGB(surface->format, r, g, b);
		SDL_SetColorKey(surface.get(), SDL_TRUE, pixel);
	}	
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer.get(), surface.get());
	if (tex == nullptr) errthrow("SDL_CreateTexture");
	std::shared_ptr<SDL_Texture> texture(tex, [](SDL_Texture *tex) {
		SDL_DestroyTexture(tex);
	});
	return texture;
}
