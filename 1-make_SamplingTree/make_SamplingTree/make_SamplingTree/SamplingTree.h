#pragma once

#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <random>
#include <utility>
#include <tuple>
#include <format>

#define TYPE double


class SamplingTree {
private:
	std::vector<TYPE> _tree;			// 木構造本体

	// add 関連
	int _write_idx;						// 次に追加する位置(leaf idx)

	// get_sample関連
	std::random_device _rd;
	std::mt19937 _randEngine;

	size_t _capacity;
	size_t _real_size;

	// max leaf
	int _max_leafIdx;

public:
	SamplingTree(const size_t capacity) 
		: _write_idx(0), _capacity(capacity), _real_size(0), _max_leafIdx(capacity)
	{
		this->_tree.resize(2 * this->_capacity - 1);

		this->_randEngine = std::mt19937(this->_rd());
	}

	SamplingTree(const SamplingTree& tree) {
		this->_tree = tree._tree;
		this->_write_idx = tree._write_idx;
		this->_capacity = tree._capacity;
		this->_real_size = tree._real_size;
		this->_max_leafIdx = tree._max_leafIdx;
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
		auto write_treeIdx = this->conv_leafIdx2treeIdx(this->_write_idx);

		// 更新差の記録
		TYPE diff = value - this->_tree[write_treeIdx];

		// 値を追加
		this->_tree[write_treeIdx] = value;

		// max_leafIdxの管理
		if (value > this->_tree[this->_max_leafIdx]) this->_max_leafIdx = this->conv_treeIdx2leafIdx(write_treeIdx);
		else if (write_treeIdx == this->_max_leafIdx) this->find_max_leaf();

		// 木を修正
		this->propagate_update(write_treeIdx, diff);

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
	std::pair<std::vector<TYPE>, std::vector<int>> get_smaples(const size_t sample_size) {
		// keyの作成
		std::vector<TYPE> keys(sample_size);
		std::uniform_real_distribution<> key_dist(0.0, this->total());

		for (auto& key : keys) key = key_dist(this->_randEngine);

		// samplesの抽出
		std::vector<TYPE> samples(sample_size);
		std::vector<int> indics(sample_size);

		for (int i = 0; i < keys.size(); ++i) {
			auto ret = this->get_sample(keys[i]);
			TYPE sample = ret.first;
			int idx = ret.second;

			samples[i] = sample;
			indics[i] = idx;
		}

		return std::make_pair(samples, indics);
	}

	/*
	値の更新
	Args: 
		values: 更新後の値
		indics: 更新位置(leaf idx)
	*/
	void update(const std::vector<TYPE> values, const std::vector<int> indics) {
		// 例外処理
		if (values.size() != indics.size()) {
			std::ostringstream except_oss;
			except_oss << "SamplingTree::updat, values.size(" << values.size() << ") != indics.size(" << indics.size() << ")\n";
			throw std::runtime_error(except_oss.str());
		}

		// 更新
		for (auto i = 0; i < values.size(); ++i) {
			TYPE diff = values[i] - this->_tree[indics[i]];
			this->_tree[indics[i]] = values[i];
			this->propagate_update(indics[i], diff);
		}
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

	// ------------------------------ get_samples 関連 ------------------------------

	/*
	keyから値を取得する
	*/
	std::pair<TYPE, int> get_sample(double key) {
		int retry_limit = 100;
		for (int i = 0; i < retry_limit; ++i) {
			int seek_idx = 0;
			while (seek_idx < this->_capacity - 1) {
				double lc = this->leftchild(seek_idx);
				if (key <= lc) {
					seek_idx = this->leftchild_idx(seek_idx);
				} else {
					key -= lc;
					seek_idx = this->rightchild_idx(seek_idx);
				}
			}

			if (this->conv_treeIdx2leafIdx(seek_idx) < this->_real_size)
				return std::make_pair(this->_tree[seek_idx], this->conv_treeIdx2leafIdx(seek_idx));

			std::cout << "miss\n";
			key = std::uniform_real_distribution<double>(0, total())(this->_randEngine);
		}
		throw std::runtime_error("Failed to sample a valid element after multiple retries.");
	}

	// ------------------------------ state step ------------------------------

	void step_write_idx() {
		this->_write_idx = (this->_write_idx + 1) % this->_capacity;
	}

	void step_real_size() {
		this->_real_size = std::min(this->_real_size + 1, this->_capacity);
	}

	// ------------------------------ max leaf 管理 ------------------------------
	
	/*
	最大の葉を見つけ，その位置をmax_leafIdxに記録する
	*/
	void find_max_leaf() {
		this->_max_leafIdx = this->get_first_leafpos_as_treeIdx();
		for (int i = this->get_first_leafpos_as_treeIdx(); i <= this->get_last_leafpos_as_treeIdx(); ++i) {
			if (this->_tree[this->_max_leafIdx] < this->_tree[i]) this->_max_leafIdx = i;
		}
	}


private:
	// ------------------------------ 親子 変換処理 ------------------------------
	inline int parent_idx(const int child_idx) const { return (child_idx - 1) / 2; }
	inline int leftchild_idx(const int parent_idx) const { return parent_idx * 2 + 1; }
	inline int rightchild_idx(const int parent_idx) const { return parent_idx * 2 + 2; }
	
	inline TYPE parent(const int child_idx) const { return this->_tree[this->parent_idx(child_idx)]; }
	inline TYPE leftchild(const int parent_idx) const { return this->_tree[this->leftchild_idx(parent_idx)]; }
	inline TYPE rightchild(const int parent_idx) const { return this->_tree[this->rightchild_idx(parent_idx)]; }

	// ------------------------------ leaf idx <-> tree idx ------------------------------
	int conv_treeIdx2leafIdx(const int treeIdx) const { return treeIdx - this->_capacity + 1; }
	int conv_leafIdx2treeIdx(const int leafIdx) const { return this->_capacity - 1 + leafIdx; }
	bool is_leaf(const int treeIdx) { return (0 <= this->conv_treeIdx2leafIdx(treeIdx) && this->conv_leafIdx2treeIdx(treeIdx) < this->_capacity); }

	// ------------------------------ 意味のある変数を取得する ------------------------------
	inline int get_first_leafpos_as_treeIdx() const { return this->_capacity; }						// 最初の葉の位置をtreeIdxで取得する
	inline int get_last_leafpos_as_treeIdx() const { return this->_tree.size() - 1; }				// 最後の葉の位置をtreeIdxで取得する


	// ------------------------------ getter ------------------------------
public:
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