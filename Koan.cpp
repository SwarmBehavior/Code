
#include "Boid.h"

#define EDIT 1
#define edit_myopia 11
#define edit_comfort 12
#define edit_danger 13
#define VIEW 2

int main(){
	sf::RenderWindow window(sf::VideoMode(width, height), "Flocking");
	std::vector<Boid> boids;
	std::vector<std::vector<Point> > map (width, std::vector<Point>(height, Point(EMPTY, 0)));
	std::vector<sf::CircleShape> foods;
	std::vector<Boid> enemies;
	int mode = VIEW;
	int myopia = MYOPIA;
	int comfort_dis = COMFORT_DIS;
	int danger_dis = DANGER_DIS;

	while (window.isOpen()){
		Mat img_map(height, width, CV_8UC3, Scalar(0,0,0));
		sf::Event event;
		while (window.pollEvent(event)){
			switch (event.type){
			// window closed
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				cout << "Mouse!" << endl;
				break;
				// key pressed
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::P){
					cin.get();
				}
				if (event.key.code == sf::Keyboard::Up){
					Boid boid;
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
					Boid predator;
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
				}
				else if (event.key.code == sf::Keyboard::Z) {
					if (mode == edit_myopia) myopia --;
					else if (mode == edit_danger) danger_dis --;
					else if (mode == edit_comfort) comfort_dis --;
				}
				else if (event.key.code == sf::Keyboard::R) {
					mode = VIEW;
					myopia = MYOPIA;
					comfort_dis = COMFORT_DIS;
					danger_dis = DANGER_DIS;
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
				nVelocity = /*boids[i].getVelocity()*/ av_vel + v1 + v2 + v3 + v4 + v5;
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
			map[boids[i].getPosition().x][boids[i].getPosition().y] = Point(FRIEND, 0);
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
		font.loadFromFile("veramono-webfont.ttf");
		text.setFont(font); // font is a sf::Font
		string mode_str = "";
		if (mode == 1) mode_str = "EDIT";
		else if (mode == 11) mode_str = "EDIT MYOPIA";
		else if (mode == 12) mode_str = "EDIT COMFORT DISTANCE";
		else if (mode == 13) mode_str = "EDIT DANGER DISTANCE";
		else if (mode == 2) mode_str = "VIEW";
		string in_screen = "Number of boids: " + to_string((int)boids.size()) +"\nMyopia: " + to_string(myopia)
																		+ "\nConfort distance: " + to_string(comfort_dis) + "\nDanger distance: " + to_string(danger_dis)
																		+ "\nMode: " + mode_str;
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


