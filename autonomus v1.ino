#include "Ultrasonic.h"
#include <AFMotor.h>

Ultrasonic sensorBack(A0, A1);
Ultrasonic sensorRight(A0, A2);
Ultrasonic sensorLeft(A0, A3);
Ultrasonic sensorFront(A0, A4);

AF_DCMotor motorFront(1, MOTOR12_64KHZ);
AF_DCMotor motorBack(2, MOTOR12_64KHZ);
AF_DCMotor wheel(3, MOTOR12_64KHZ);

const int NOT_ESCAPING = 0;
const int ESCAPING_LEFT = 1;
const int ESCAPING_RIGHT = 2;
const int ESCAPING_FRONT = 3;
const int ESCAPING_BACK = 4;
const int ESCAPING_REVERSE_LEFT = 5;
const int ESCAPING_REVERSE_RIGHT = 6;

int escaping = NOT_ESCAPING;

const int FAR_AWAY_THRESHOLD = 50;
const int FAR_THRESHOLD = 20;
const bool FAKE_MOTOR = false;
const int MAX_SPEED = 200;

int delaySensor = 0;
int distanceFront = 0, distanceBack = 0, distanceRight = 0, distanceLeft = 0;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	delay(50);
	probeDistances();

	//⇦⇧⇨⇩
	//↰↱
	//⇅
	
	steerStraight();
	stop();

	if(escaping == NOT_ESCAPING)
	{
		if(isFarAway(distanceFront))
		{
			Serial.println("⇧");
			goFront();
			
			if(!isFar(distanceLeft) && isFar(distanceRight)) {
				Serial.println("↱");
				steerRight();
			}
			if(isFar(distanceLeft) && !isFar(distanceRight)) {
				Serial.println("↰");
				steerLeft();
			}
			
		}
		else if(isFarAway(distanceLeft) || isFarAway(distanceRight))
		{
			if(compareDistance(distanceLeft, distanceRight) > 0)
			{
				escaping = ESCAPING_LEFT;
			}
			else
			{
				escaping = ESCAPING_RIGHT;
			}
		}
		else if(isFarAway(distanceBack))
		{
			escaping = ESCAPING_BACK;
		}
		else
		{
			Serial.println("please don't move");
			stop();
		}
	}

	if(escaping != NOT_ESCAPING)
	{
		Serial.print("escape to : ");
		if(escaping == ESCAPING_LEFT) Serial.println("⇦");
		if(escaping == ESCAPING_RIGHT) Serial.println("⇨");
		if(escaping == ESCAPING_REVERSE_LEFT) Serial.println("⇅⇦");
		if(escaping == ESCAPING_REVERSE_RIGHT) Serial.println("⇅⇨");
		if(escaping == ESCAPING_FRONT) Serial.println("⇧");
		if(escaping == ESCAPING_BACK) Serial.println("⇩");
	}

	if(escaping == ESCAPING_LEFT)
	{
		if(isFarAway(distanceFront))
		{
			Serial.println("stop escaping to left");
			escaping = NOT_ESCAPING;
		}
		else if(isFar(distanceFront))
		{
			Serial.println("⇦");
			goFront();
			steerLeft();
		}
		else
		{
			//can't go any further, reverse escape
			escaping = ESCAPING_REVERSE_RIGHT;
		}
	}
	else if(escaping == ESCAPING_RIGHT)
	{
		if(isFarAway(distanceFront))
		{
			Serial.println("stop escaping to right");
			escaping = NOT_ESCAPING;
		}
		else if(isFar(distanceFront))
		{
			Serial.println("⇨");
			goFront();
			steerRight();
		}
		else
		{
			//can't go any further, reverse escape
			escaping = ESCAPING_REVERSE_LEFT;
		}
	}
	else if(escaping == ESCAPING_BACK)
	{
		if(isFarAway(distanceLeft) || isFarAway(distanceRight))
		{
			Serial.println("stop escaping to back");
			escaping = NOT_ESCAPING;
		}
		else if(isFar(distanceBack))
		{
			Serial.println("⇩");
			goBack();
		}
		else
		{
			//can't go any further, not escaping anymore
			escaping = NOT_ESCAPING;
		}
	}
	else if(escaping == ESCAPING_REVERSE_LEFT)
	{
		if(isFar(distanceFront) || !isFar(distanceBack))
		{
			Serial.println("stop reverse escaping to left");
			escaping = ESCAPING_RIGHT;
		}
		else
		{
			Serial.println("⇩⇦");
			goBack();
			steerLeft();
		}
	}
	else if(escaping == ESCAPING_REVERSE_RIGHT)
	{
		if(isFar(distanceFront) || !isFar(distanceBack))
		{
			Serial.println("stop reverse escaping to right");
			escaping = ESCAPING_LEFT;
		}
		else
		{
			Serial.println("⇩⇨");
			goBack();
			steerRight();
		}
	}
}

void probeDistances()
{
	distanceFront = sensorFront.Ranging(CM);
	delay(delaySensor);
	distanceRight = sensorRight.Ranging(CM);
	delay(delaySensor);
	distanceLeft = sensorLeft.Ranging(CM);
	delay(delaySensor);
	distanceBack = sensorBack.Ranging(CM);
	delay(delaySensor);
	/*
	Serial.print("front : ");
	Serial.print(distanceFront);
	Serial.print(", left : ");
	Serial.print(distanceLeft);
	Serial.print(", right : ");
	Serial.print(distanceRight);
	Serial.print(", back : ");
	Serial.println(distanceBack);
	*/
}

void steerLeft()
{
	steer(BACKWARD);
}

void steerRight()
{
	steer(FORWARD);
}

void steerStraight()
{
	steer(RELEASE);
}

void steer(int direction)
{
	if(FAKE_MOTOR) return;
	wheel.run(direction); //backward == left
	wheel.setSpeed(255);
}

void goFront()
{
	go(FORWARD);
}

void goBack()
{
	go(BACKWARD);
}

void stop()
{
	go(RELEASE);
}

void go(int direction)
{
	if(FAKE_MOTOR) return;
	motorFront.run(direction);
	motorBack.run(direction);
	motorFront.setSpeed(MAX_SPEED);
	motorBack.setSpeed(MAX_SPEED);
}

bool isFarAway(int distance)
{
	return distance > FAR_AWAY_THRESHOLD || distance == 0;
}

bool isFar(int distance)
{
	return distance > FAR_THRESHOLD || distance == 0;
}

int compareDistance(int d1, int d2)
{
	return d2 - d1;
}

