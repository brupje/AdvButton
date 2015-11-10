/*
*
* Thanks for using this library! If you like it, please drop me a comment at bart@sbo-dewindroos.nl.
*
* File     : AdvButton.cpp
* Version  : 1.0
* Released : 24/01/2010
* Author   : Bart Meijer (bart@sbo-dewindroos.nl)
*
* This is the Advanced Button library for the Arduino platform. It will enable you to easily 
* react to user input using simple press-to-make buttons. Features include:
* - Event based implementation 
* - Recording the time a button is pressed
* - Adjustable repeat delay, start delay for the keypressed event
* - requires only a single call in the main loop
*
* This file implements the AdvButton class
*
*/

#include "AdvButton.h"
#include "ButtonManager.h"

AdvButton::AdvButton(uint8_t pin,void (*OnKeyPress)(AdvButton*) , unsigned long repeat, unsigned long startDelay, buttonMode mode)
{
	setPin(pin);
	setRepeat(repeat);
	setStartDelay(startDelay);
	setOnKeyPress( (*OnKeyPress));
	setOnKeyDown( NULL);
	setOnKeyUp( NULL);
	debounceTime = 100;
	ButtonManager::instance()->addButton(this);
	if (mode == btn_Digital){
		pinMode(pin,INPUT);
		digitalWrite(pin,HIGH);
  }
	this->mode = mode;
	lastChange = millis(); 
    lastState = HIGH;
}

AdvButton::AdvButton(uint8_t pin,void (*OnKeyPress)(AdvButton*) , void (*OnKeyDown)(AdvButton*),void (*OnKeyUp)(AdvButton*), buttonMode mode)
{
	setPin(pin);
	setRepeat(300);
	setStartDelay(500);
	setOnKeyPress( (*OnKeyPress));
	setOnKeyDown( (*OnKeyDown));
	setOnKeyUp( (*OnKeyUp));

	debounceTime = 100;
	ButtonManager::instance()->addButton(this);
	if (mode == btn_Digital){
    pinMode(pin, INPUT_PULLUP);
		digitalWrite(pin,HIGH);
	}
	this->mode = mode;
  lastChange = millis();
  lastState = HIGH;
}

void AdvButton::check()
{
	// determine current pin state
	int cur;
	if (mode == btn_Digital)
		cur = digitalRead(pin);
	else if (mode == btn_Analog)
		cur = analogRead(pin) >200 ? HIGH : LOW;

	/* button state changed */
	if (lastState != cur)
	{
		

		if (millis() - lastChange < debounceTime)
		{
			return;
		}
		lastChange = millis();
		lastState = cur; 
	}

	
	
  if (cur == LOW)
	{	
		// the button is pressed, but last time we checked, was the button still up?		
		if (startPress ==0)
		{
			/* mark the start time and notify others */
			startPress = millis();
			if (func_keyDown != NULL)
				func_keyDown(this);
            if (func_keyPress != NULL)
                func_keyPress(this);
		}
		
		/* is repeating enabled? */
		if ((repeat > 0 ) && (func_keyPress != NULL))
		{
			/* is the startdelay passed? */
			if (millis() >= startPress+startDelay)
			{
				/* is it time for a keypressed call? */
				if ((millis() - prevPres) > repeat)
				{
					prevPres = millis(); 	
					func_keyPress(this);		
				}
				
			}
			else
				prevPres = millis();
		}
	}
	else
	{
		prevPres = 0;
		/* the button is released, but was it previously pressed */		
		if (startPress != 0)
		{
			/* reset start time, notify others */
			
			if (func_keyUp != NULL)
				func_keyUp(this);	
			startPress = 0;
		}
	}

}

unsigned long AdvButton::getPressTime()
{
	return millis() - startPress;
}

void AdvButton::setOnKeyPress(void (*f)(AdvButton*))
{
	func_keyPress =  (*f);
}

void AdvButton::setOnKeyDown(void (*f)(AdvButton*))
{
	func_keyDown =  (*f);
}

void AdvButton::setOnKeyUp(void (*f)(AdvButton*))
{
	func_keyUp =  (*f);
}

void AdvButton::setRepeat(unsigned long repeat)
{
	this->repeat = repeat;
}

void AdvButton::setStartDelay(unsigned long startDelay)
{
	this->startDelay = startDelay;
}

void AdvButton::setPin(uint8_t pin)
{
	this->pin = pin;
}
