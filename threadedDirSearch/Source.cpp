#include "example/example.h"

int main() {
	const unsigned int repeats = 10;
	const bool muted = true;
	const auto now = std::chrono::high_resolution_clock::now();
	auto complete_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - now);
	
	for (int i = 0; i < repeats + 1; i++) {

		const auto start = std::chrono::high_resolution_clock::now();
		std::string dir = "C:\\Projects\\";

		if (muted == false) {
			std::cout << "[+] Welcome to rapid windows file search (c) Constantin Fuerst" << std::endl;
		}

		example_use file_querry(search::singletowide(dir));
		file_querry.search(muted);

		const auto stop = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		complete_time += duration;
		
		if (muted == false) {
			std::cout << "[+] quit by pressing any key\n";
			std::cout << "[+] Search took" << duration.count() << std::endl;
		}
	}

	const auto average = complete_time.count() / repeats;
	std::cout << "[+] Program took an average of " << average << " milliseconds per run" << std::endl;
	
	std::cin.get();
	return 1;
}