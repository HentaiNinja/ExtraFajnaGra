#pragma once
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <memory>
#include <string>
#include <list>
#include "GameObject.h"
#include "Particle.h"
#include "KeyboardHandler.h"

enum GameState {
	Paused,
	Ongoing,
	Closed,
};

class GameWindow
{
private:
	KeyboardHandler kb_han;
	const int screen_width = 1280, screen_height = 720;
	int last_tick = 0;
	int current_tick = 0;
	int gravity = 100;
	int max_comets = 3;
	int comet_size = 70;
	int rocket_size = 80;
	int start_button_width = 200;
	int start_button_height = 40;
	int points = 0;

	int min_comet_speed = 1000;
	int max_comet_speed = 1800;
	bool allow_comets_on_same_height = false;

	GameObject *playable_rocket;
	GameObject *animation_rocket;
	GameState game_state;

	Physics bg_physics;

	std::shared_ptr<SDL_Window> window;
	std::shared_ptr<SDL_Renderer> renderer;
	std::shared_ptr<SDL_Texture> background_texture;
	std::shared_ptr<SDL_Texture> rocket_texture;
	std::shared_ptr<SDL_Texture> pause_texture;
	std::list<GameObject> comets;
	std::list<GameObject> animation_comets;
	std::list<GameObject> planets;
	std::list<Particle> partciles;

	std::shared_ptr<SDL_Texture> particle_texture;
	std::shared_ptr<SDL_Texture> start_game_button_texture;
	std::shared_ptr<SDL_Texture> start_game_button_background_texture;

	std::shared_ptr<SDL_Texture> points_string_texture;
	std::shared_ptr<GameObject> points_string_game_obj;
	std::list<std::shared_ptr<SDL_Texture>> comet_textures;
	std::list<std::shared_ptr<SDL_Texture>> planet_textures;
	std::list<std::shared_ptr<GameObject>> number_game_objs;
	std::list<std::shared_ptr<SDL_Texture>> number_game_objs_test;

	void errthrow(const std::string &e);
	void init();
	std::shared_ptr<SDL_Window> init_window(int width, int height);
	std::shared_ptr<SDL_Renderer> init_renderer(std::shared_ptr<SDL_Window> window);
	std::shared_ptr<SDL_Surface> create_surface(std::string file_name);
	std::shared_ptr<SDL_Texture> create_texture_from_surface(SDL_Renderer* render, SDL_Surface* surface);
	std::shared_ptr<SDL_Texture> create_texture(std::string resource_path, bool apply_alhpa = false, int r = 0, int g = 255, int b = 0);
	std::shared_ptr<GameObject> create_object(int width, int height, Physics physics, SDL_Texture* texture);

	void manage_objects(std::list<GameObject>* objects, std::list<std::shared_ptr<SDL_Texture>>* textures, bool should_force, bool apply_points, bool planet = false);
	void draw_objects(std::list<GameObject>* objects);
	void apply_speed_to_objects(std::list<GameObject>* objects, int delta_ticks);
	void apply_speed_to_object(GameObject* object, int delta_ticks);
	void randomize_object(GameObject * object, std::list<GameObject>* objects, std::list<std::shared_ptr<SDL_Texture>>* textures, bool planet = false);
	bool there_is_a_object_on_this_height(int height, int object_height, std::list<GameObject>* objects);
	bool object_is_out_of_screen(GameObject* object);
	void changeRocketPosition(int delta_ticks);
	void begin();
	void restart_game();
	void animate_background(bool anim_rocket);
	int random(int min, int max);
	void resize_planets();
	bool rocket_hit_comet();
	void display_score(bool display_points = true);
	bool objects_collide(GameObject* obj1, GameObject* obj2);
	void add_particles(GameObject* rocket);
	void draw_particles();
	void draw_particle(Particle *part);
	void manage_particles();
	Particle create_particle(GameObject* rocket);
public:
	GameWindow();
	~GameWindow();
	void show_start_game_window();
};

