from SamplingTree import SamplingTree

import random
import numpy as np
from tqdm import tqdm

def test_add():
    capacity = 20000
    tree = SamplingTree(capacity)

    for _ in range(2 * capacity):
        tree.add(random.random() * 100)

        if not tree.check_tree(0.1):
            print(f'max err: {tree.max_err()}')
            assert(not tree.check_tree(0.1))

def get_filltree(capacity: int=20000, min: float=0.0, max: float=100.0) -> SamplingTree:
    tree = SamplingTree(capacity)

    for _ in range(capacity):
        tree.add(random.random() * (max - min) + min)
    
    return tree


def test_get_sample():
    capacity = 10
    tree = get_filltree(capacity=capacity)

    values = np.zeros(capacity)
    count = np.zeros(capacity)

    iters = 1000000
    batch_size = 3
    for _ in tqdm(range(iters)):
        # サンプリング
        samples, indics = tree.get_smaples(batch_size)
        print(indics)

        # 集計
        for sample, idx in zip(samples, indics):
            values[idx] = sample
            count[idx] += 1
    
    # 集計
    max_err = 0.0
    for value, cnt in zip(values, count):
        pred = cnt / (iters * batch_size)
        prob = value / tree.total()

        max_err = max(max_err, abs(pred - prob))
    assert(max_err < 0.001)

def test_get_sample_sameValues():
    capacity = 20000
    tree = SamplingTree(capacity)
    for _ in range(capacity):
        tree.add(1.0)
    
    count = np.zeros(capacity)
    
    iters = 1000000
    batch_size = 128
    for _ in tqdm(range(iters)):
        # サンプリング
        samples, indics = tree.get_smaples(batch_size)

        # 集計
        for sample, idx in zip(samples, indics):
            count[idx] += 1
    
    # 集計
    max_count = 0
    min_count = iters * batch_size
    for cnt in count:
        max_count = max(max_count, cnt)
        min_count = min(min_count, cnt)
    assert(1000 > (max_count - min_count))

def test_update():
    capacity = 20000
    tree = get_filltree(capacity=capacity)

    iters = 1000000
    batch_size = 128
    for _ in tqdm(range(iters)):
        samples, indics = tree.get_smaples(batch_size)

        new_values = np.random.random(len(indics)) * 100.0

        tree.update(new_values.tolist(), indics)

        if not tree.check_tree(0.1):
            print(f'max err: {tree.max_err()}')
            assert(not tree.check_tree(0.1))

def test_max_leaf():
    capacity = 20000
    tree = get_filltree(capacity=capacity)
    tree.add(200.0)

    iters = 1000000
    batch_size = 128
    for _ in tqdm(range(iters)):
        samples, indics = tree.get_smaples(batch_size)

        new_values = np.random.random(len(indics)) * 100.0

        tree.update(new_values.tolist(), indics)

        assert(tree.check_max_leaf())

test_get_sample()