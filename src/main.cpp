#include <iostream>
#include "ROM.h"

int main() {
	std::cout << "fmemu v." << VERSION << " rev." << COMMIT << "";
	ROM file = ROM::FromFile("test.gb");
}