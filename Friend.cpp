#include <SFML/Graphics.hpp>
#include "opencv2/opencv.hpp"

class Friend{
public:

	Friend(cv::Point pos, float v_x, float v_y){
		this->position = pos;
		this->velocity_x = v_x;
		this->velocity_y = v_y;
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

	void setVelocity(float vX, float vY) {
		velocity_x = vX;
		velocity_y = vY;
	}

	const cv::Point& getPosition() const {
		return position;
	}

	void setPosition(const cv::Point& position) {
		this->position = position;
	}

private:
	float velocity_x;
	float velocity_y;
	cv::Point position;
};




