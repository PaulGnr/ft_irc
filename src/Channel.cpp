#include "Channel.hpp"

std::string	Channel::getName(void) {return (_name);}

Channel::Channel(const std::string &name) : _name(name)
{}

Channel::~Channel(void)
{}
