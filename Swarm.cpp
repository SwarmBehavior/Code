#include "Boid.h"

#define EDIT 1
#define edit_myopia 11
#define edit_comfort 12
#define edit_danger 13
#define edit_cohesion 21
#define edit_separation 22
#define edit_alingment 23
#define edit_hunger 24
#define edit_fear 25
#define VIEW 3

#define RATE0 20
#define RATE1 10
#define RATE2 15
#define RATE3 8
#define RATE4 8
#define RATE5 15

int main(){
	sf::RenderWindow window(sf::VideoMode(width, height), "Flocking");
	sf::Texture background;
	if (!background.loadFromFile("/home/irenerrrd/Workspace/Swarm/background.jpg"))
	    return -1;
	sf::Sprite sea(background);
	std::vector<Boid> boids;
	std::vector<std::vector<Point> > map (width, std::vector<Point>(height, Point(EMPTY, 0)));
	std::vector<sf::CircleShape> foods;
	std::vector<Boid> enemies;
	int mode = VIEW, myopia = MYOPIA, comfort_dis = COMFORT_DIS;
	int rate0 = RATE0, rate1 = RATE1, rate2 = RATE2, rate3 = RATE3, rate4 = RATE4, rate5 = RATE5;
	int danger_dis = DANGER_DIS;

	sf::Texture fish1, fish2, fish3, shark;
	fish1.loadFromFile("/home/irenerrrd/Workspace/Swarm/gulf.png");
	shark.loadFromFile("/home/irenerrrd/Workspace/Swarm/redfish.png");
	vector<sf::Texture> fishes;
	fishes.push_back(fish1);

	while (window.isOpen()){
		Mat img_map(height, width, CV_8UC3, Scalar(0,0,0));
		sf::Event event;
		while (window.pollEvent(event)){
			switch (event.type){
			// window closed
			case sf::Event::Closed:
				window.close();
				break;
				// key pressed
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::P){
					cin.get();
				}
				if (event.key.code == sf::Keyboard::Up){
					Boid boid(fishes[0]);
					boids.push_back(boid);
				} else if (event.key.code == sf::Keyboard::Down){
					if (!boids.empty()){
						Boid last_boid = boids.back();
						map[last_boid.getPosition().x][last_boid.getPosition().y].x = EMPTY;
						boids.pop_back();
					}
				}
				else if (event.key.code == sf::Keyboard::Left) {
					sf::CircleShape food;
					createFood(food);
					foods.push_back(food);
					map[food.getPosition().x][food.getPosition().y].x = FOOD;
				}
				else if (event.key.code == sf::Keyboard::Right) {
					if (!foods.empty()){
						sf::CircleShape last_food = foods.back();
						map[last_food.getPosition().x][last_food.getPosition().y].x = EMPTY;
						foods.pop_back();
					}
				}
				else if (event.key.code == sf::Keyboard::D) {
					Boid predator(shark);
					createEnemy(predator);
					enemies.push_back(predator);
				}
				else if (event.key.code == sf::Keyboard::S) {
					if (!enemies.empty()){
						Boid last_predator = enemies.back();
						map[last_predator.getPosition().x][last_predator.getPosition().y].x = EMPTY;
						enemies.pop_back();
					}
				}
				else if (event.key.code == sf::Keyboard::E) {
					if (mode != VIEW) mode = VIEW;
					else if (mode == VIEW) mode = EDIT;
				}
				else if (event.key.code == sf::Keyboard::M) {
					if (mode != VIEW){
						mode = edit_myopia;
					}
				}
				else if (event.key.code == sf::Keyboard::N) {
					if (mode != VIEW){
						mode = edit_comfort;
					}
				}
				else if (event.key.code == sf::Keyboard::B) {
					if (mode != VIEW){
						mode = edit_danger;
					}
				}
				else if (event.key.code == sf::Keyboard::X) {
					if (mode == edit_myopia) myopia ++;
					else if (mode == edit_danger) danger_dis ++;
					else if (mode == edit_comfort) comfort_dis ++;
					else if (mode == edit_cohesion) rate1 ++;
					else if (mode == edit_separation) rate2 ++;
					else if (mode == edit_alingment) rate3 ++;
					else if (mode == edit_hunger) rate4 ++;
					else if (mode == edit_fear) rate5 ++;
				}
				else if (event.key.code == sf::Keyboard::Z) {
					if (mode == edit_myopia) myopia --;
					else if (mode == edit_danger) danger_dis --;
					else if (mode == edit_comfort) comfort_dis --;
					else if (mode == edit_cohesion) rate1 --;
					else if (mode == edit_separation) rate2 --;
					else if (mode == edit_alingment) rate3 --;
					else if (mode == edit_hunger) rate4 --;
					else if (mode == edit_fear) rate5 --;
				}
				else if (event.key.code == sf::Keyboard::R) {
					mode = VIEW;
					myopia = MYOPIA;
					comfort_dis = COMFORT_DIS;
					danger_dis = DANGER_DIS;
					rate0 = RATE0;
					rate1 = RATE1;
					rate2 = RATE2;
					rate3 = RATE3;
					rate4 = RATE4;
					rate5 = RATE5;
				}
				else if (event.key.code == sf::Keyboard::T) {
					mode = edit_cohesion;
				}
				else if (event.key.code == sf::Keyboard::Y) {
					mode = edit_separation;
				}
				else if (event.key.code == sf::Keyboard::U) {
					mode = edit_alingment;
				}
				else if (event.key.code == sf::Keyboard::I) {
					mode = edit_hunger;
				}
				else if (event.key.code == sf::Keyboard::O) {
					mode = edit_fear;
				}
				break;

			default:
				break;
			}
		}

		for (int i = 0; i < (int)boids.size(); i++){
			// Keep the boid inside de window
			boids[i].loopedWorld();
			bool something_found = boids[i].look_surroundings(map, boids, myopia);
			Point2f nVelocity (0,0);
			// Make the boid move randomly
			if (something_found){
				Point2f v1 = boids[i].lookForFriends();
				Point2f v2 = boids[i].giveMeSpace(comfort_dis);
				Point2f v3 = boids[i].uniformVel();
				Point2f v4 = boids[i].lookForFood();
				Point2f v5 = boids[i].avoidEnemies(danger_dis);

				Point2f av_vel(mod_vel*cos(boids[i].getRotation() * degree2radian), mod_vel*sin(boids[i].getRotation() * degree2radian));
				nVelocity = /*boids[i].getVelocity()*/ ( rate0*av_vel + rate1*v1 + rate2*v2 + rate3*v3 + rate4*v4 + rate5*v5 )
						/ (100.0);
				limitVelocity(nVelocity);
			} else {
				nVelocity = boids[i].moveRandomly();
			}

			map[boids[i].getPosition().x][boids[i].getPosition().y] = Point(EMPTY, 0);

			boids[i].setVelocity(nVelocity.x, nVelocity.y);
			float rot = atan2(nVelocity.y, nVelocity.x) * radian2degree;
			boids[i].move(nVelocity.x, nVelocity.y);
			boids[i].setRotation(rot);

			boids[i].setPosition(cvRound(boids[i].getPosition().x), cvRound(boids[i].getPosition().y));
			boids[i].loopedWorld();
			map[boids[i].getPosition().x][boids[i].getPosition().y] = Point(FRIEND, i);
		}

		for (int i = 0; i < (int)enemies.size(); i++) {
			Point2f nVelocity = enemies[i].moveRandomly();
			map[enemies[i].getPosition().x][enemies[i].getPosition().y] = Point(EMPTY, 0);

			enemies[i].setVelocity(nVelocity.x, nVelocity.y);
			float rot = atan2(nVelocity.y, nVelocity.x) * radian2degree;
			enemies[i].move(nVelocity.x, nVelocity.y);
			enemies[i].setRotation(rot);

			enemies[i].setPosition(cvRound(enemies[i].getPosition().x), cvRound(enemies[i].getPosition().y));
			enemies[i].loopedWorld();
			map[enemies[i].getPosition().x][enemies[i].getPosition().y] = Point(ENEMY, 0);
		}

		//update the boid position
		window.clear();
		window.draw(sea);

		for (int i = 0; i < (int)boids.size(); i++){
			window.draw(boids[i]);
		}

		for (int i = 0; i < (int)enemies.size(); i++) {
			window.draw(enemies[i]);
		}

		for (int k = 0; k < (int)foods.size(); k++) {
			bool found = false;
			int x = foods[k].getPosition().x;
			int y = foods[k].getPosition().y;
			int radius = 10;

			for (int i = -radius; i < radius; i++) {
				if(!found){
					for (int j = -radius; j < radius; j++) {
						if (pow(i,2) + pow(j,2) <= radius*radius){

							Point pos = boids[0].keepInside(x+i, y+j);

							if (map[pos.x][pos.y].x == FRIEND) {
								found = true;
								break;
							}
						}
					}
				}
			}

			if (found) {
				map[x][y] = Point(EMPTY, 0);
				foods.erase(foods.begin() + k);
				found = false;
			}
		}
		for (int k = 0; k < (int)foods.size(); k++) {
			window.draw(foods[k]);
		}

		sf::Text text;
		sf::Font font;
		font.loadFromFile("/home/irenerrrd/Workspace/Swarm/Debug/veramono-webfont.ttf");
		text.setFont(font); // font is a sf::Font
		string mode_str = "";
		if (mode == 1) mode_str = "EDIT";
		else if (mode == 11) mode_str = "EDIT MYOPIA";
		else if (mode == 12) mode_str = "EDIT COMFORT DISTANCE";
		else if (mode == 13) mode_str = "EDIT DANGER DISTANCE";
		else if (mode == 21) mode_str = "EDIT COHESION";
		else if (mode == 22) mode_str = "EDIT SEPARATION";
		else if (mode == 23) mode_str = "EDIT ALINGMENT";
		else if (mode == 24) mode_str = "EDIT HUNGER";
		else if (mode == 25) mode_str = "EDIT FEAR";
		else if (mode == 3) mode_str = "VIEW";
		string in_screen = "Number of boids: " + to_string((int)boids.size()) +"\nMyopia: " + to_string(myopia)
								+ "\nConfort distance: " + to_string(comfort_dis) + "\nDanger distance: " + to_string(danger_dis)
								+ "\nMode: " + mode_str + "\nCohesion: " + to_string(rate1) +"\nSeparation: " + to_string(rate2)
								+ "\nAlignment: " + to_string(rate3) + "\nHunger: " + to_string(rate4) + "\nFear: " + to_string(rate5);
		text.setString(in_screen);
		text.setCharacterSize(14); // in pixels, not points!
		text.setColor(sf::Color::White);

		// inside the main loop, between window.clear() and window.display()
		window.draw(text);
		window.display();

	}
	map.clear();
	foods.clear();
	boids.clear();
	enemies.clear();
	return 0;
}


