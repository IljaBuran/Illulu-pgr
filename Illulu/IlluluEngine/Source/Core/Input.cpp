//-----------------------------------------------------------------------------------------
/**
 * \file       Input.cpp
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Implements keyboard and mouse input state tracking.
 */
 //-----------------------------------------------------------------------------------------

#include "Input.h"
#include "pch.h"

void Input::begin_frame()
{
	for (i32 i = 0; i < KEY_COUNT; i++)
	{
		keysPressed[i] = false;
		keysReleased[i] = false;
	}

	mouseDeltaX = 0.0f;
	mouseDeltaY = 0.0f;
}

void Input::set_key(i32 key, bool isDown)
{
	if (key < 0 || key >= KEY_COUNT)
		return;

	if (isDown)
	{
		if (!keysDown[key])
			keysPressed[key] = true;
		keysDown[key] = true;
	}
	else
	{
		if (keysDown[key])
			keysReleased[key] = true;
		keysDown[key] = false;
	}
}

void Input::set_mouse_position(f32 x, f32 y)
{
	if (mouseBegin)
	{
		mouseX = x;
		mouseY = y;
		mouseBegin = false;
		return;
	}

	mouseDeltaX += x - mouseX;
	mouseDeltaY += mouseY - y;

	mouseX = x;
	mouseY = y;
}

bool Input::is_key_down(i32 key) const
{
	return keysDown[key];
}

bool Input::is_key_pressed(i32 key) const
{
	return keysPressed[key];
}

bool Input::is_key_released(i32 key) const
{
	return keysReleased[key];
}