#include <iostream>
#include "map_reduce.h"

std::function<vec_str(const std::string&)> map_func = [](std::string s) {

	vec_str res;
	
	for (std::size_t i = 1; i <= s.size(); i++) {
		res.push_back(s.substr(0, i));
	}
	
	//res.push_back(s);
	return res;
};


//пока оставил, как временную заглушку. редусер определен в методе reduce
std::function<vec_str(const std::string&)> reduce_func = [&](const std::string& str) {
	int pref = 1;
	//std::set<std::string> set;
	//std::stringstream ss(str);
	//std::string temp_str;
	//while (std::getline(ss, temp_str)) {
	//	set.emplace(temp_str);
	//}
	/*
	if (!str.empty()) {
		auto first = set.begin();
		auto second = set.begin();
		++second;
		while (second != set.end()) {
			int diff = std::distance(first->begin(),
				std::mismatch(first->begin(), first->end(), second->begin(), second->end()).first) + 1;
			pref = std::max(pref, diff);
			++first;
			++second;
		}
	}
	*/
	//return std::to_string(pref);
	return vec_str{ std::to_string(pref) };
};

int main(int args, char* argv[]) {

	if (args != 4) {
		std::cerr << "Порядок запуска - # yamr <src> <mnum> <rnum>\n" << std::endl;
		return EXIT_FAILURE;
	}
	std::string src = argv[1];
	int mnum = std::atoi(argv[2]);
	int rnum = std::atoi(argv[3]);

	std::ifstream test(src);
	if (!mnum || !rnum || test.fail()) {
		std::cerr << "Data err!" << std::endl;
		return EXIT_FAILURE;
	}

	MapReduce m(mnum, rnum, src);
	m.run(map_func, reduce_func);

	return 0;
}




