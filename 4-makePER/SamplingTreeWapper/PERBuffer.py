
import torch

class ReplayBufferInterface:
    def __init__(self, capacity: int, 
                 state_size: int, action_size: int, reward_size: int=1, done_size: int=1, 
                 state_type: torch.dtype=torch.float32, action_type: torch.dtype=torch.float32, reward_type: torch.dtype=torch.float32, done_type: torch.dtype=torch.int8, 
                 device: torch.device=torch.device("cpu")):
        # インスタンス変数
        self._capacity = capacity
        self._device = device
        
        # バッファ本体
        self._status = torch.empty(size=self._capacity, )



class PERBuffer(BaseReplayBuffer):
    pass