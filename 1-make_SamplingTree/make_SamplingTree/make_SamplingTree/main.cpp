
#include <iostream>
#include <random>

#include "SamplingTree.h"

void test_add(const size_t capacity) {
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_real_distribution<> val_dist(0.0, 100.0);

	SamplingTree tree(capacity);


	for (int i = 0; i < 5 * capacity; ++i) {
		tree.add(val_dist(engine));
		if (!tree.check_tree(0.1)) {
			std::cout << "Œë·‚ª‘å‚«‚¢\n";
			std::cout << "max err: " << tree.max_err() << "\n";
		}
	}
	// tree.show();
	std::cout << "max err: " << tree.max_err() << "\n";
}

int main(void)
{
	test_add(20000);
}