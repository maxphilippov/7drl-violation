//
//  log_message.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef log_message_h
#define log_message_h

#include <string>
#include <sstream>

struct LogMessage
{
    std::string message;
    int turn_timer;
};

std::string save_log_message(LogMessage const& m) {
    std::stringstream ss;
}

#endif /* log_message_h */
