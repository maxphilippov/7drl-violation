//
//  message_log.hpp
//  7drl-violation
//
//  Created by Max Philippov on 13/03/16.
//  Copyright © 2016 AGNMTLT. All rights reserved.
//

#ifndef message_log_h
#define message_log_h

#include <string>
#include <sstream>

#include "time.hpp"

auto format_message_log(std::string const& message, int turn_counter)
{
    std::ostringstream ss;

    auto dh = turns_to_days_hours(turn_counter);
    ss << "d" << dh.first << "h" << dh.second << "  ";

    ss << message;

    return ss.str();
}

#endif /* message_log_h */
