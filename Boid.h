/*
 * Boid.h
 *
 *  Created on: 11 feb. 2017
 *      Author: irenerrrd
 */

#ifndef BOID_H_
#define BOID_H_

#include <SFML/Graphics.hpp>
//#include <SDKDDKVer.h>
//#include <tchar.h>
//#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>				//basic building blocks
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "Friend.cpp"

using namespace cv;
using namespace std;

#define width 1200
#define height 700
#define degree2radian 0.01745329252
#define radian2degree 57.2958279
#define PI 3.14159

#define MYOPIA 60
#define COMFORT_DIS 40
#define DANGER_DIS 80
#define mod_vel 12
#define vel_max 25

#define EMPTY 0
#define FRIEND 1
#define FOOD 2
#define ENEMY 3

/*
 * Boid.cpp
 *
 *  Created on: 17 dic. 2016
 *      Author: irenerrrd
 */

class Boid: public sf::Sprite{
public:

	Boid(sf::Texture & texture){
		// Create the  shape
//		this->setPointCount(3); //3 to be a triangle
//		this->setPoint(0, sf::Vector2f(10.0f, 0.0f)); //This numbers allows the center of the triangle be the same as the sprite center
//		this->setPoint(1, sf::Vector2f(-10.0f, 7.5f));
//		this->setPoint(2, sf::Vector2f(-10.0f, -7.5f));

		// Give a random color
//		int randomR = rand() % 255;
//		int randomG = rand() % 255;
//		int randomB = rand() % 255;
//		sf::Color random_color (randomR, randomG, randomB, 255);

		// this->setFillColor(random_color);
		// this->setOutlineColor(sf::Color (255, 255, 255, 255));
		// this->setOutlineThickness(1);

		this->setTexture(texture);
		// Give a random position
		float randomx = (float)(rand() % width);
		float randomy = (float)(rand() % height);
		this->setPosition(cvRound(randomx), cvRound(randomy));
		this->setOrigin(35.0,15.0);
		this->setRotation((float)(rand() % 360));

		float rotation = this->getRotation() * degree2radian;  //We take into account the rotation to move correctly
		Point2f velocity(cosf(rotation) * mod_vel, sinf(rotation) * mod_vel);
		this->velocity_x = velocity.x;
		this->velocity_y = velocity.y;
	}

	float getVelocityX() const {
		return velocity_x;
	}
	float getVelocityY() const {
		return velocity_y;
	}
	cv::Point2f getVelocity() const {
		return cv::Point2f(velocity_x, velocity_y);
	}

	void setVelocity(float vX, float vY);
	bool look_surroundings(std::vector<std::vector<Point> > & map, vector<Boid> & boids, int myopia);
	void loopedWorld(void);
	Point keepInside(int ax, int ay);
	Point2f lookForFriends();
	Point2f lookForFood();
	Point2f avoidEnemies(int danger_dis);
	Point2f giveMeSpace(int comfort_dis);
	Point2f uniformVel();
	Point2f moveRandomly();

private:
	float velocity_x;
	float velocity_y;
	std::vector<Friend> friends;
	std::vector<cv::Point> enemies;
	cv::Point food;
};

void createFood(sf::CircleShape & food);
void createEnemy(Boid & enemy);
void limitVelocity(Point2f & vel);

#endif /* BOID_H_ */
