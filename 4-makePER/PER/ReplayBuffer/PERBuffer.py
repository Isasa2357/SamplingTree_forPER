
import numpy as np
from numpy import ndarray
from nptyping import NDArray
from typing import List, Any, Tuple

import torch

from usefulParam.Param import ScalarParam
from SamplingTree_pywapper import SamplingTree_pywapper

class ReplayBufferInterface:
    def __init__(self, capacity: int, 
                 state_size: int, action_size: int, reward_size: int=1, done_size: int=1, 
                 state_type: torch.dtype=torch.float32, action_type: torch.dtype=torch.float32, reward_type: torch.dtype=torch.float32, done_type: torch.dtype=torch.int8, 
                 device: torch.device=torch.device("cpu")):
        # インスタンス変数
        self._capacity = capacity
        self._device = device

        self._state_size = state_size
        self._action_size = action_size
        self._reward_size = reward_size
        self._done_size = done_size

        self._real_size = 0
        
        # バッファ本体
        self._status = torch.empty(capacity, self._state_size, dtype=state_type, device=self._device)
        self._actions = torch.empty(capacity, self._action_size, dtype=action_type, device=self._device)
        self._rewards = torch.empty(capacity, self._reward_size, dtype=reward_type, device=self._device)
        self._next_status = torch.empty(capacity, self._state_size, dtype=state_type, device=self._device)
        self._dones = torch.zeros(capacity, self._done_size, dtype=done_type, device=self._device)
    
    def add(self, state: NDArray, action: NDArray, reward: NDArray, next_state: NDArray, done: NDArray) -> None:
        '''
        バッファへ経験を追加する

        Args:
            [state, action, reward, next_state, done]
        '''
        raise NotImplementedError("ReplayBufferInterface.addは仮想関数です")

    
    def get_sample(self, sample_size: int) ->  Any:
        '''
        バッファからサンプリングを行う

        Args:
            size: サンプリングサイズ
        Ret:
            [status, actions, rewards, next_status, dones]
        '''
        raise NotImplementedError("ReplayBufferInterface.get_samplesは仮想関数です")
    
    @property
    def real_size(self):
        return self._real_size
    
    @property
    def capacity(self):
        return self._capacity
    
    def __len__(self):
        return self._real_size
    
    def to(self, device: torch.device):
        '''
            デバイスの変更
        '''
        self._status.to(device)
        self._actions.to(device)
        self._rewards.to(device)
        self._next_status.to(device)
        self._dones.to(device)

class PERBuffer(ReplayBufferInterface):
    def __init__(self, capacity: int, alpha: ScalarParam, beta: ScalarParam, 
                 state_size: int, action_size: int, reward_size: int=1, done_size: int=1, 
                 state_type: torch.dtype=torch.float32, action_type: torch.dtype=torch.float32, reward_type: torch.dtype=torch.float32, done_type: torch.dtype=torch.int8, 
                 device: torch.device=torch.device("cpu")):
        super().__init__(capacity, 
                         state_size, action_size, reward_size, done_size, 
                         state_type, action_type, reward_type, done_type, 
                         device)
        self._alpha = alpha
        self._beta = beta

        self._priorities = SamplingTree_pywapper(self._capacity)

        self._indics4update = None
    
    def add(self, state: NDArray, action: NDArray, reward: NDArray, next_state: NDArray, done: NDArray) -> None:
        # 優先度更新
        write_idx = 0
        if self._real_size == 0:
            write_idx = self._priorities.add(1.0)
        else:
            write_idx = self._priorities.add(self._priorities.max_leaf)
        
        # バッファ更新
        self._status[write_idx] = torch.tensor(state, dtype=self._status.dtype, device=self._device)
        self._actions [write_idx] = torch.tensor(action, dtype=self._actions.dtype, device=self._device)
        self._rewards[write_idx] = torch.tensor(reward, dtype=self._rewards.dtype, device=self._device)
        self._next_status[write_idx] = torch.tensor(next_state, dtype=self._next_status.dtype, device=self._device)
        self._dones[write_idx] = torch.tensor(done, dtype=self._dones.dtype, device=self._device)

    def get_sample(self, sample_size: int) -> Tuple[torch.Tensor, torch.Tensor, torch.Tensor, torch.Tensor, torch.Tensor, torch.Tensor]:
        priorities, indics = self._priorities.get_sample(sample_size)
        weights = self._calc_weight(priorities)

        self._indics4update = np.array(indics)

        extracted_status = self._status[indics]
        extracted_actions = self._actions[indics]
        extracted_rewards = self._rewards[indics]
        extracted_next_status = self._next_status[indics]
        extracted_dones = self._dones[indics]

        return extracted_status, extracted_actions, extracted_rewards, extracted_next_status, extracted_dones, weights
    
    def _calc_weight(self, priorities: ndarray) -> torch.Tensor:
        '''
        優先度から重みを計算する
        '''
        # 優先度全体の合計を取得する(c++へのアクセスを1度に抑えるために記録)
        priority_total = self._priorities.total

        # 経験の選択確率を計算する
        select_probs = [priority / priority_total for priority in priorities]

        # 重みの計算
        weights = [(self.real_size * select_prob)**self._beta.value for select_prob in select_probs]

        # 正規化
        weights /= max(weights)

        return torch.tensor(weights, device=self._device)

    def update(self, td_diffs: ndarray) -> None:
        '''
        優先度を更新する
        '''
        if self._indics4update == None:
            raise RuntimeError("まだ，サンプリングが行われていません")

        # 優先度の計算
        new_priorities = self._calc_priorities(td_diffs)

        # 更新
        self._priorities.update(new_priorities, self._indics4update)
        self._indics4update = None

    def _calc_priorities(self, td_diffs: ndarray) -> ndarray:
        '''
        TD誤差から優先度を計算する
        '''
        priorities = (td_diffs + 1e-6)**self._alpha.value
        return priorities