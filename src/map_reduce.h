#pragma once
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <functional>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <future>
#include <functional>
#include <thread>
#include <iterator>

using vec_str = std::vector<std::string>;
using set_str = std::set<std::string>;
using vec_thr = std::vector<std::thread>;

std::mutex console_m;
std::mutex vector_m;
static std::atomic_int file_id = 11;

class MapReduce {
	std::size_t m_threads_;
	std::size_t r_threads_;
	std::string file_path_;

	std::vector<set_str> map_res_;
	std::vector<set_str> shuffle_res_;

	MapReduce() = delete;
	MapReduce(const MapReduce&) = delete;

public:
	MapReduce(std::size_t m, std::size_t r, const std::string& path) :
		m_threads_(m), r_threads_(r), file_path_(path) {}

	void run(std::function<vec_str(const std::string&)> m_f, std::function<vec_str(const std::string&)> r_f) {
		auto v = split_file();
		map(v, m_f);
		shuffle();
		reduce(r_f);
	}

private:
	std::vector<std::size_t> split_file() {
		std::ifstream in_file;
		in_file.open(file_path_, std::ios_base::ate);
		std::size_t file_size = in_file.tellg();
		std::vector<std::size_t> offsets(m_threads_ + 1, file_size + 1);
		std::size_t block_size = file_size / m_threads_;
		offsets[0] = 0;
		for (std::size_t i = 1; i <= m_threads_; i++) {
			std::size_t offset = offsets[i - 1] + block_size;
			in_file.seekg(offset, in_file.beg);
			char c = 'a';
			while (c != '\n' && !in_file.eof()) {
				++offset;
				c = in_file.get();
			}
			c = in_file.peek();
			if (in_file.eof()) {
				offsets[i] = file_size + 1;
				break;
			}
			else {
				if (c == '\n') { ++offset; }
				offsets[i] = offset;
			}
		}
		in_file.close();
		return offsets;
	}

	void map(std::vector<std::size_t> off, std::function<vec_str(const std::string&)> f) {
		vec_thr vtr;
		for (std::size_t i = 0; i < m_threads_; i++) {
			vtr.push_back(std::thread([this, f](int from, int to) {
				std::ifstream file;
				file.open(file_path_);
				file.seekg(from, file.beg);
				char c = file.peek();
				if (c == '\n') { ++from; file.seekg(from, file.beg); }
				set_str thread_res;
				while (from < to) {
					std::string str;
					std::getline(file, str);
					from += str.size() + 2;
					auto v(f(str));
					thread_res.insert(v.begin(), v.end());
				}
				file.close();
				vector_m.lock();
				map_res_.push_back(std::move(thread_res));
				vector_m.unlock();
				},
				off[i], off[i + 1] - 1
					));
		}
		for (auto& v : vtr) {
			if (v.joinable())
				v.join();
		}
	}

	void shuffle() {
		shuffle_res_.resize(r_threads_);
		vec_thr vtr;
		for (std::size_t i = 0; i < m_threads_; i++) {
			vtr.push_back(std::thread([this, i]() {
				for (auto& m : map_res_[i]) {
					std::size_t num = std::hash<std::string>{}(m) % r_threads_;
					vector_m.lock();
					shuffle_res_[num].emplace(m);
					vector_m.unlock();
				}
				})
			);
		}
		for (auto& v : vtr) {
			if (v.joinable())
				v.join();
		}
	}

	void reduce(std::function<vec_str(const std::string&)> f) {
		vec_thr vtr;
		for (std::size_t i = 0; i < r_threads_; i++) {
			vtr.push_back(std::thread([this, i, f]() {
				std::string str;
				for (auto& s : shuffle_res_[i]) {
					str += (s + "\n");
				}
				vec_str red_res{ f(str)};
				std::string file_name = "R" + std::to_string(i) + "_" + std::to_string(file_id++) + ".txt";
				std::ofstream out_file;
				out_file.open(file_name);
				for (auto& v : red_res) {
					out_file << v << std::endl;
				}
				out_file.close();
				})
			);
		}
		for (auto& v : vtr) {
			if (v.joinable())
				v.join();
		}
	}
};
