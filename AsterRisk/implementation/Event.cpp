#include "../headers/Event.h"

Event::Event(std::string subject_name, EventType type, int x, int y)
{
    this->subject_name = subject_name;
    this->type = type;
    this->x = x;
    this->y = y;
}

Event::Event(std::string subject_name, EventType type)
{
    this->subject_name = subject_name;
    this->type = type;
    this->x = -1;
    this->y = -1;
}

Event::Event(EventType type)
{
    this->type = type;
    this->subject_name = "";
    this->x = -1;
    this->y = -1;
}

std::string Event::get_source_name()
{
    return subject_name;
}

EventType Event::get_event_type()
{
    return type;
}

int Event::get_x()
{
    return x;
}

int Event::get_y()
{
    return y;
}