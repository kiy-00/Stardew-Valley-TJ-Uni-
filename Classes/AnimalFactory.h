#ifndef __ANIMAL_FACTORY_H__
#define __ANIMAL_FACTORY_H__

#include "Animal.h"
#include <string>


class AnimalFactory 
{
public:
	static Animal* createAnimal(const std::string& typeName);
};

#endif // __ANIMAL_FACTORY_H__

