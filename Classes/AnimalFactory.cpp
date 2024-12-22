#include "AnimalFactory.h"

Animal* AnimalFactory::createAnimal(const std::string& typeName)
{
	return Animal::createWithType(typeName);
}
