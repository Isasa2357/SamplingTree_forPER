
#include <iostream>
#include <random>

#include "SamplingTree.h"

using std::vector;

SamplingTree get_fillTree(const int capacity, const double min, const double max) {
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_real_distribution<> val_dist(min, max);
	
	SamplingTree tree(capacity);

	for (int i = 0; i < capacity; ++i) {
		tree.add(val_dist(engine));
	}

	return tree;
}

void test_add(const size_t capacity) {
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_real_distribution<> val_dist(0.0, 100.0);

	SamplingTree tree(capacity);


	for (int i = 0; i < 5 * capacity; ++i) {
		tree.add(val_dist(engine));
		if (!tree.check_tree(0.1)) {
			std::cout << "誤差が大きい\n";
			std::cout << "max err: " << tree.max_err() << "\n";
		}
	}
	// tree.show();
	std::cout << "max err: " << tree.max_err() << "\n";
}

void test_get_sample() {
	auto tree = get_fillTree(2000, 0.0, 100.0);

	vector<int> count(2000);
	vector<double> values(2000);

	const int iter = 10000000;
	const int batch_size = 128;
	for (int i = 0; i < iter; ++i) {
		auto ret = tree.get_smaples(batch_size);
		auto samples = ret.first;
		auto indics = ret.second;

		for (auto i = 0; i < samples.size(); ++i) {
			count[indics[i]] += 1;
			values[indics[i]] = samples[i];
		}
	}

	// 集計
	double max_err = 0;
	for (auto i = 0; i < count.size(); ++i) {
		double pred = count[i] / (iter * batch_size);
		double prob = values[i] / tree.total();
		max_err = (abs(pred - prob));
	}
	std::cout << "max err : " << max_err << "\n";
}

void test_get_sample_sameValues() {
	int capacity = 2000;
	SamplingTree tree(capacity);

	for (int i = 0; i < capacity; ++i) {
		tree.add(1.0);
	}

	vector<int> count(capacity);
	vector<double> values(capacity);

	const int iter = 10000000;
	const int batch_size = 128;
	for (int i = 0; i < iter; ++i) {
		auto ret = tree.get_smaples(batch_size);
		auto samples = ret.first;
		auto indics = ret.second;

		for (auto i = 0; i < samples.size(); ++i) {
			count[indics[i]] += 1;
			values[indics[i]] = samples[i];
		}
	}

	// 集計
	int max_count = 0.0, min_count = iter * batch_size;
	for (auto i = 0; i < count.size(); ++i) {
		max_count = std::max(max_count, count[i]);
		min_count = std::min(min_count, count[i]);
	}
	std::cout << "max err : " << (max_count - min_count) << "\n";
	std::cout << "max: " << max_count << "\n";
	std::cout << "min: " << min_count << "\n";
}

void test_update() {
	const int capacity = 10;
	auto tree = get_fillTree(capacity, 0.0, 100.0);
	std::cout << "inital show\n";
	tree.show();

	const int iter = 1000000;
	for (int i = 0; i < iter; ++i) {
		// サンプル抽出
		auto ret = tree.get_smaples(1);
		auto samples = ret.first;
		auto indics = ret.second;

		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_real_distribution<> val_dist(0.0, 100.0);

		for (auto& sample : samples) {
			sample = val_dist(engine);
		}

		// update
		tree.update(samples, indics);

		if (!tree.check_tree(0.01)) {
			std::cout << "誤差が大きい max err = " << tree.max_err() << "\n";
			std::cout << i << "\n";
			std::cout << indics[0] << "\n";
			tree.show();
			return;
		}

	}
}

void test_max_leaf() {
	const int capacity = 20000;
	auto tree = get_fillTree(capacity, 0.0, 100.0);

	const int iter = 1000000;
	const int batch_size = 128;
	for (int i = 0; i < iter; ++i) {
		// サンプル抽出
		auto ret = tree.get_smaples(batch_size);
		auto samples = ret.first;
		auto indics = ret.second;

		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_real_distribution<> val_dist(0.0, 100.0);

		for (auto& sample : samples) {
			sample = val_dist(engine);
		}

		// update
		tree.update(samples, indics);


		if (!tree.check_max_leaf()) {
			std::cout << i << "\n";
			std::cout << tree.max_leaf() << ", " << tree.tree()[tree.max_leafIdx()] << "\n\n";
		}

	}
}


int main(void)
{
	// test_add(20000);
	// test_get_sample();
	test_get_sample_sameValues();
	// test_update();

	// test_max_leaf();
}