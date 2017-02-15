#include "Boid.h"

float norm_angle(float angle){
	float norm;
	if (angle<PI){
		norm=angle;
		if (angle<-PI){
			norm=2*PI+angle;
		}
	}
	else{
		norm=-(2*PI-angle);
	}
	return norm;
}

float distanceP2P(Point a, Point b){
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

void createFood(sf::CircleShape & food) {
	// Create the  shape
	food.setRadius(5);
	food.setFillColor(sf::Color::Blue);
	food.setOutlineColor(sf::Color::White);
	food.setOutlineThickness(1);

	// Give a random position
	int randomx = rand() % width;
	int randomy = rand() % height;
	food.setPosition(randomx, randomy);
}

void createEnemy(Boid & enemy){
	// Create the  shape
	enemy.setPointCount(3); //3 to be a triangle
	enemy.setPoint(0, sf::Vector2f(12.0f, 0.0f)); //This numbers allows the center of the triangle be the same as the sprite center
	enemy.setPoint(1, sf::Vector2f(-12.0f, 9.0f));
	enemy.setPoint(2, sf::Vector2f(-12.0f, -9.0f));
	enemy.setFillColor(sf::Color::Red);
	enemy.setOutlineColor(sf::Color(255,0,0,50));
	enemy.setOutlineThickness(2);

	// Give a random position
	float randomx = (float)(rand() % width);
	float randomy = (float)(rand() % height);
	enemy.setPosition(cvRound(randomx), cvRound(randomy));
	enemy.setRotation((float)(rand() % 360));

	float rotation = enemy.getRotation() * degree2radian;  //We take into account the rotation to move correctly
	Point2f velocity(cosf(rotation) * mod_vel, sinf(rotation) * mod_vel);
	enemy.setVelocity(velocity.x, velocity.y);
}

void limitVelocity(Point2f & vel){

	float mod = sqrt(pow(vel.x,2) + pow(vel.y,2));
	if (mod > vel_max){
		float angle = atan2(vel.y, vel.x);
		vel = Point2f(vel_max * cos(angle), vel_max*sin(angle));
	}
}

void Boid::setVelocity(float vX, float vY) {
	velocity_x = vX;
	velocity_y = vY;
}

bool Boid::look_surroundings(std::vector<std::vector<Point> > & map, vector<Boid> & boids, int myopia) {
	int x = this->getPosition().x;
	int y = this->getPosition().y;
	this->friends.clear();
	this->enemies.clear();
	this->food = Point(0,0);
	bool found = false;

	for (int i = - 3*myopia; i < 3*myopia; i++){
		for (int j = - 3*myopia; j < 3*myopia; j++){
			if (j != 0 || i != 0){
				cv::Point pos = keepInside(x+i, y+j);
				if (map[pos.x][pos.y].x != EMPTY){
					float dis = sqrt((pow(i,2) + pow(j,2)));
					if (dis <= 3*myopia){
						float direction = norm_angle(atan2(y, x));
						if (fabs(norm_angle(direction - norm_angle(this->getRotation() * degree2radian))) < 120){

							if (map[pos.x][pos.y].x == FOOD){
								found = true;
								this->food = Point(x+i, y+j);
							}

							if (dis <= 3*myopia){
								if (map[pos.x][pos.y].x == ENEMY){
									this->enemies.push_back(Point(x+i, y+j));
									found = true;
								}

								if (dis <= myopia){
									if (map[pos.x][pos.y].x == FRIEND){
										Boid b = boids[map[pos.x][pos.y].y];
										Friend fr (Point(x+i,y+j), b.getVelocityX(), b.getVelocityY());
										this->friends.push_back(fr);
										found = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return found;
}

void Boid::loopedWorld(void){
	// Keep the boid in a correct horizontal position
	if (this->getPosition().x >= width) this->setPosition(this->getPosition().x - width, this->getPosition().y);
	if (this->getPosition().x < 0) this->setPosition(this->getPosition().x + width, this->getPosition().y);

	// Keep the boid in a correct vertical position
	if (this->getPosition().y >= height) this->setPosition(this->getPosition().x, this->getPosition().y - height);
	if (this->getPosition().y < 0) this->setPosition(this->getPosition().x, this->getPosition().y + height);
}

Point Boid::keepInside(int ax, int ay){
	Point b(0,0);

	if (ax >= width) b.x = ax - width;
	else if (ax < 0) b.x = ax + width;
	else b.x = ax;

	if (ay >= height) b.y = ay - height;
	else if (ay < 0) b.y = ay + height;
	else b.y = ay;

	return b;
}

Point2f Boid::lookForFriends(){
	int x = this->getPosition().x;
	int y = this->getPosition().y;
	Point2f mass_center (0, 0);
	int found_friends = 0;

	for (int i = 0; i < (int)this->friends.size(); i++){
		mass_center.x += this->friends[i].getPosition().x;
		mass_center.y += this->friends[i].getPosition().y;
		found_friends ++;
	}

	if (found_friends != 0){
		mass_center.x /= found_friends;
		mass_center.y /= found_friends;

		Point2f vel(mass_center.x - x, mass_center.y - y) ;
		return vel / 10.0;
	} else {
		return Point2f(0,0);
	}
}

Point2f Boid::lookForFood() {
	int x = this->getPosition().x;
	int y = this->getPosition().y;

	if (this->food.x != 0 && this->food.y != 0) {
		Point2f vel(this->food.x - x, this->food.y - y);
		return vel / 10.0;
	} else {
		return Point2f(0, 0);
	}
}

Point2f Boid::avoidEnemies(int danger_dis){
	int x = this->getPosition().x;
	int y = this->getPosition().y;
	Point2f center(0, 0);
	bool found_enemy = false;

	for (int i = 0; i < (int)this->enemies.size(); i++){
		found_enemy = true;
		float dis = distanceP2P(Point(x,y), enemies[i]);
		if (dis < danger_dis){
			float repulsion = danger_dis - dis;
			float angle = norm_angle(atan2(enemies[i].y - y, enemies[i].x - x));
			center.x -= repulsion * cos(angle);
			center.y -= repulsion * sin(angle);
		}
	}

	if (found_enemy) {
		return center;
	} else {
		return Point2f(0, 0);
	}
}

Point2f Boid::giveMeSpace(int comfort_dis){
	int x = this->getPosition().x;
	int y = this->getPosition().y;
	Point2f center (0, 0);
	bool encontrado = false;

	for (int i = 0; i < (int)this->friends.size(); i++){
		encontrado = true;
		float dis = distanceP2P(Point(x,y), this->friends[i].getPosition());
		if (dis < comfort_dis){
			float repulsion = comfort_dis - dis;
			float angle = norm_angle(atan2(this->friends[i].getPosition().y - y, this->friends[i].getPosition().x - x));
			center.x -= repulsion * cos(angle);
			center.y -= repulsion * sin(angle);
		}
	}
	for (int i = 0; i < (int)this->enemies.size(); i++){
		encontrado = true;
		float dis = distanceP2P(Point(x,y), this->enemies[i]);
		if (dis < comfort_dis){
			float repulsion = comfort_dis - dis;
			float angle = norm_angle(atan2(this->enemies[i].y - y,this->enemies[i].x - x));
			center.x -= repulsion * cos(angle);
			center.y -= repulsion * sin(angle);
		}
	}

	if (encontrado)
		return Point2f(center.x / 2, center.y / 2);
	else
		return center;
}

Point2f Boid::uniformVel(){
	int found_friends = 0;
	Point2f uni_vel(0, 0);

	for(int i = 0; i < (int)this->friends.size(); i++){
		uni_vel.x += friends[i].getVelocityX();
		uni_vel.y += friends[i].getVelocityY();
		found_friends ++;
	}

	if (found_friends != 0){
		uni_vel.x /= found_friends;
		uni_vel.y /= found_friends;

		Point2f dif(uni_vel.x - this->velocity_x, uni_vel.y - this->velocity_y);
		return dif/2;
	} else {
		return Point2f(0,0);
	}
}

/*
 * Given the boid velocity, there is a 70% probability to continue with
 * the same direction there is a 15% proability to turn left and
 * a 15% probability to turn right.
 */
Point2f Boid::moveRandomly(){
	float rotation = this->getRotation();  //We take into account the rotation to move correctly
	int keep_mov = (int)(rand() % 100);
	if (keep_mov > 80){
		rotation = rotation + 1.0;
	} else if (keep_mov > 90){
		rotation = rotation - 1.0;
	}
	rotation = rotation * degree2radian;
	sf::Vector2f velocity(cosf(rotation) * mod_vel, sinf(rotation) * mod_vel);

	return Point2f(velocity.x, velocity.y);
}



