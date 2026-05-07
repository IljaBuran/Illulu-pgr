#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       Input.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines keyboard and mouse input state tracking.
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"

struct Input
{
	static constexpr i32 KEY_COUNT = 512;

	bool keysDown[KEY_COUNT]     = {};
	bool keysPressed[KEY_COUNT]  = {};
	bool keysReleased[KEY_COUNT] = {};

	f32 mouseX = 0.0f;
	f32 mouseY = 0.0f;
	f32 mouseDeltaX = 0.0f;
	f32 mouseDeltaY = 0.0f;

	bool mouseBegin = true;

	void begin_frame();
	void set_key(i32 key, bool isDown);
	void set_mouse_position(f32 x, f32 y);

	bool is_key_down(i32 key) const;
	bool is_key_pressed(i32 key) const;
	bool is_key_released(i32 key) const;
};

