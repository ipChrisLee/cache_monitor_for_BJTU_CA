#include <fstream>
#include <iostream>

int main() {
	auto ofs = std::ofstream("demo/stride132.txt");
	int stride = 132;
	for (int i = 0; i < 10000; ++i) {
		for (int j = 0; j < 256; j += stride) {
			ofs << j * 4 << "\n";
		}
	}
	return 0;
}