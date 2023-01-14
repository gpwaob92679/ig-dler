#include <direct.h>

#include <ctime>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

std::string GetISOCurrentTime() {
	std::time_t now = std::time(nullptr);
	std::tm now_local_time;
	localtime_s(&now_local_time, &now);

	char buffer[50];
	std::strftime(buffer, sizeof(buffer), "%FT%H_%M_%S%z", &now_local_time);
	return std::string(buffer);
}

void Init() {
	int ret;

	ret = _chdir("../ig_dl");
	if (ret != 0) {
		exit(ret);
	}

	std::string now = GetISOCurrentTime();
	std::cerr << "Current time: " << now << std::endl;
	ret = _mkdir(now.c_str());
	if (ret != 0) {
		exit(ret);
	}

	ret = _chdir(now.c_str());
	if (ret != 0) {
		exit(ret);
	}
}

int main() {
	Init();

	std::fstream all, list, script;
	all.open("all.txt", std::ios::out);
	list.open("list.txt", std::ios::out);
	// Open file in binary mode to avoid endl conversions from '\n' to '\r\n'
	script.open("download.sh", std::ios::out | std::ios::binary);
	if (!all.is_open() || !list.is_open() || !script.is_open()) {
		std::cerr << "Unable to open output file." << std::endl;
		return 1;
	}

	std::ifstream input;
	input.open("../input.txt");
	if (input.is_open()) {
		std::cerr << "Input file found. Reading from it..." << std::endl;
		all << input.rdbuf();
		input.close();
	} else {
		std::cerr << "Input file not found. Reading from STDIN..." << std::endl;
		std::string line;
		while (std::getline(std::cin, line)) {
			all << line;
		}
	}
	all.close();

	int file_count = 0;
	std::string filename = "";

	all.open("all.txt", std::ios::in);
	if (all.is_open()) {
		std::string line;
		while (std::getline(all, line)) {
			if (line.substr(0, 4) == "curl") {
				std::regex pattern("[A-Z0-9]*_video_dashinit\\.mp4*");
				std::smatch match;

				bool found = regex_search(line, match, pattern);
				if (found) {
					filename = std::to_string(file_count) + '_' + match.str();
					++file_count;
				} else {
					filename = "";
				}
			}
			if (!filename.empty()) {
				if (line.substr(0, 14) == "  --compressed") {
					script << "  --compressed -o " << filename << " ;" << '\n';
					list << filename << std::endl;
				} else {
					script << line << '\n';
				}
			}
		}
		all.close();
		list.close();
		script.close();
	}

	return 0;
}
