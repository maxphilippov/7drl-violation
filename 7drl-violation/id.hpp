//
//  id.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef id_h
#define id_h

#include <string>

struct IDData
{
    typedef std::string id_type;
    
    id_type name;
    
    enum Type
    {
        human,
        android
    } type;
    
    int balance;
};

// One id is attached to you
// You can have a phone with a fake ID
//
// Every time in a public place when someone ask you for your id
// your phone can hide your original ID and show a fake one
// If you get caught and PO brings you to the station the phone won't help
// you and it's game over
//
// You use your ID to pay for stuff. Every citizen is allowed to borrow money
// (means his balance can go below zero). But your fake IDs aren't good ones
// so for you borrowing money is a law violation
// To check your balance shop will need 1-2 hours, so after that if you had no
// money they'll call police at that place


#endif /* id_h */
