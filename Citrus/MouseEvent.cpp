#include "MouseEvent.h"

MouseEvent::MouseEvent()
	:
	type(EventType::Invalid),
	x(0),
	y(0)
{}

MouseEvent::MouseEvent(EventType type, int x, int y)
	:
	type(type),
	x(x),
	y(y)
{}

bool MouseEvent::IsValid() const noexcept
{
	return this->type != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::GetType() const noexcept
{
	return this->type;
}

MousePoint MouseEvent::GetPos() const noexcept
{
	return{ this->x,this->y };
}

int MouseEvent::GetPosX() const noexcept
{
	return this->x;
}

int MouseEvent::GetPosY() const noexcept
{
	return this->y;
}