''' ------------------------------------------------------------------------------
* Copyright 2022 ANDRO Computational Solutions, LLC.  All Rights Reserved
 ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
This file is part of the MR-iNET software codebase released for public use under 
the [Creative Commons Attribution - NonCommercial - ShareAlike 4.0 (CC BY-NC-SA 4.0) License]
(https://creativecommons.org/licenses/by-nc-sa/4.0/).

This material is based upon work supported by the US Army Contract No. W15P7T-20-C-0006.
Any opinions, findings, and conclusions or recommendation expressed in this material 
are those of the author(s) and do not necessarily reflect the views of the US Army.
* ------------------------------------------------------------------------------

* I-ROAM
** Author(s):Collin Farquhar
** Email:   cfarquhar@androcs.com
** File: rl/ReplayMemory.py

'''




from collections import deque
from random import sample as random_sample
from random import shuffle

class Memory:

    def __init__(self, max_capacity=1e5, sample_strategy="random"):
        """
        Wraps collectons.deque to serve a "memory" or database to store experiences. 

        Attributes:
            - self.max_capacity=1e5 (int): Maximum number of memories that can
            be stored
            - self.memory (collections.deque): Stores the memories
        """
        self.max_capacity = int(max_capacity)
        self.memory = deque(maxlen=self.max_capacity)
        self.sample_strategy = sample_strategy


    def __len__(self):
        """ Returns number of transitions stored in memory """
        return len(self.memory)

    def __getitem__(self, key):
        """ Direct indexing of deque memory object """
        return self.memory[key]

    def remember(self, *args):
        """ Store transition """
        # The appened function of a deque object will overwrite the oldest entries
        # after the deque object's maxlen is reached.
        self.memory.append(args)

    def sample(self, batch_size):
        """ Sample minibatch """
        if self.sample_strategy == "random":
            batch = random_sample(self.memory, batch_size)
        elif self.sample_strategy == "half":
            half_batch_size = int(batch_size / 2)
            recent_batch = [self.memory[-1 * (i + 1)] for i in range(half_batch_size)] # most recent entries
            random_batch = random_sample(self.memory, half_batch_size)
            batch = recent_batch + random_batch
            shuffle(batch) # randomly shuffles the list; acts in-place
        else:
            raise ValueError("self.sample_strategy value is not recognized.")

        return batch
    
    def clear(self):
        """ Clear all elements from the memory """ 
        self.memory.clear()
