#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <utility>
#include <tuple>

#define TYPE double


class SamplingTree {
private:
	std::vector<TYPE> _tree;			// 木構造本体

	// add 関連
	int _write_idx;						// 次に追加する位置(leaf idx)

	// get_sample関連
	std::random_device _rd;
	std::mt19937 _sampling_randEngine;

	size_t _capacity;
	size_t _real_size;

	// max leaf
	int max_leaf_idx;

public:
	SamplingTree(const size_t capacity) 
		: _write_idx(0), _capacity(capacity), _real_size(0)
	{

		this->_tree.resize(2 * this->_capacity - 1);

		this->_sampling_randEngine = std::mt19937(this->_rd());
	}

	/*
	値を追加する
	Args:
		value: 追加する値
	Ret:
		追加した位置のidx
	*/
	int add(const TYPE value) {
		// 書き込み位置をtree_idxに変換
		auto write_tree_idx = this->conv_leaf_idx2tree_idx(this->_write_idx);

		// 更新差の記録
		TYPE diff = value - this->_tree[write_tree_idx];

		// 値を追加
		this->_tree[write_tree_idx] = value;

		// 木を修正
		this->propagate_update(write_tree_idx, diff);

		// write_idxとreal_sizeを更新
		auto old_write_idx = this->_write_idx;
		this->step_write_idx();
		this->step_real_size();

		return old_write_idx;
	}

	/*
	複数の値を追加する
	実装的には複数回のaddと同じ

	Args:
		values: 追加する値の配列
	Ret:
		追加した値の位置のindics
	*/
	std::vector<int> add_multiple(const std::vector<TYPE> values) {
		std::vector<int> indics(values.size());

		for (auto i = 0; i < values.size(); ++i) {
			indics[i] = this->add(values[i]);
		}

		return indics;
	}

	/*
	サンプリング
	Args:
		batch_size: サンプリングサイズ
	Ret:
		抽出したサンプルと抽出したサンプルのindex
	*/
	std::vector<TYPE> get_smaple(const size_t batch_size) {
		//
	}

	/*
	値の更新
	Args: 
		values: 更新後の値
		indics: 更新位置(leaf idx)
	*/
	void update(const std::vector<TYPE> values, const std::vector<int> indics) {

	}

private:
	// ------------------------------ add 関連 ------------------------------

	/*
	idxの位置から根までの値の更新を行う
	*/
	void propagate_update(const int idx, const TYPE diff) {
		int seek = idx;

		while (seek != 0) {
			// seek 更新
			seek = this->parent_idx(seek);

			// 値更新
			this->_tree[seek] += diff;
		}
	}

	// ------------------------------ state step ------------------------------

	void step_write_idx() {
		this->_write_idx = (this->_write_idx + 1) % this->_capacity;
	}

	void step_real_size() {
		this->_real_size = std::min(this->_real_size + 1, this->_capacity);
	}

	// ------------------------------ max leaf 管理 ------------------------------



private:
	// ------------------------------ 親子 変換処理 ------------------------------
	inline int parent_idx(const int child_idx) const { return (child_idx - 1) / 2; }
	inline int leftchild_idx(const int parent_idx) const { return parent_idx * 2 + 1; }
	inline int rightchild_idx(const int parent_idx) const { return parent_idx * 2 + 2; }
	
	inline TYPE parent(const int child_idx) const { return this->_tree[this->parent_idx(child_idx)]; }
	inline TYPE leftchild(const int parent_idx) const { return this->_tree[this->leftchild_idx(parent_idx)]; }
	inline TYPE rightchild(const int parent_idx) const { return this->_tree[this->rightchild_idx(parent_idx)]; }

	// ------------------------------ leaf idx <-> tree idx ------------------------------
	int conv_tree_idx2leaf_idx(const int treeidx) const { return treeidx - this->_capacity + 1; }
	int conv_leaf_idx2tree_idx(const int leafidx) const { return this->_capacity - 1 + leafidx; }
	bool is_leaf(const int tree_idx) { return (0 <= this->conv_tree_idx2leaf_idx(tree_idx) && this->conv_leaf_idx2tree_idx(tree_idx) < this->_capacity); }

	// ------------------------------ getter ------------------------------
	inline TYPE total() const { return this->_tree[0]; }
	inline TYPE mal_leaf() const {}

public:
	/*
	値の確認．親と子の和の差がerr_threshold以上であるものがあれば，false
	*/
	bool check_tree(const TYPE err_threshold) const {
		for (int i = 0; i < this->_capacity - 1; ++i) {
			auto abs_err = abs(this->_tree[i] - (this->leftchild(i) + this->rightchild(i)));
			if (abs_err - err_threshold > 0) return false;
		}
		return true;
	}

	TYPE max_err() {
		TYPE max_err = 0.0;
		for (int i = 0; i < this->_capacity - 1; ++i) {
			auto abs_err = abs(this->_tree[i] - (this->leftchild(i) + this->rightchild(i)));
			max_err = std::max(abs_err, max_err);
		}
		return max_err;
	}

	void show() const {
		for (auto val : this->_tree) {
			std::cout << val << ",";
		}
		std::cout << std::endl;
	}
};