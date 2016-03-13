//
//  basic_types.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef basic_types_h
#define basic_types_h

typedef unsigned int district_id_type;

typedef unsigned long identity_id_type;

typedef unsigned long physical_object_id_type;

struct ActorCollisionInfo
{
    physical_object_id_type first;
    physical_object_id_type second;
};

#endif /* basic_types_h */
