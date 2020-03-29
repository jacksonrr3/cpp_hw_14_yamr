#include <iostream>
#include "map_reduce.h"

class Mapper {
public:
	std::vector<std::string> operator()(const std::string& s) {
		vec_str res;
		for (std::size_t i = 1; i <= s.size(); i++) {
			res.push_back(s.substr(0, i));
		}
		return res;
	}
};

class Reducer {
	std::string pred = "";
	std::size_t count = 0;
public:
	std::vector<std::string> operator()(const std::string& s) {
		vec_str res;
		if (s == pred) {
			count = std::max(count, s.size());
			res.push_back(std::to_string(count));
		}
		else {
			pred = s;
		//	res.push_back(std::to_string(count));
		}
		return res;
	}
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

	std::function<vec_str(const std::string&)> mf = Mapper();

	MapReduce<Mapper, Reducer> m(mnum, rnum, src);
	m.run(/*mf, reduce_func*/);

	return 0;
}




