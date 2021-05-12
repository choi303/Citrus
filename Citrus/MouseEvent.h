#pragma once
struct MousePoint
{
	int x;
	int y;
};

class MouseEvent
{
public:
	enum EventType
	{
		LPress,
		LRelease,
		RPress,
		RRelease,
		MPress,
		MRelease,
		WheelUp,
		WheelDown,
		Move,
		RAW_MOVE,
		Invalid
	};
private:
	EventType type;
	int x;
	int y;
public:
	MouseEvent();
	MouseEvent(const EventType type, const int x, const int y);
	bool IsValid() const noexcept;
	EventType GetType() const noexcept;
	MousePoint GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
};