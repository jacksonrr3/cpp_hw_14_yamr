#include <iostream>
#include "map_reduce.h"

std::function<vec_str(const std::string)> map_func = [](std::string s) {
	return vec_str{s};
};

std::function<vec_str(set_str&)> reduce_func = [](set_str& set) {
	int pref = 1;
	if (!set.empty()) {
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
	return vec_str{std::to_string(pref)};
};

int main(int args, char * argv[]) {
	
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


