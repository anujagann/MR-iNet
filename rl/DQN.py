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
** Author(s):Collin Farquhar, Swatantra Kafle
** Email:   cfarquhar@androcs.com, skafle@androcs.com
** File: rl/DQN.py

'''


import numpy as np
from collections import namedtuple
import torch.nn.functional as F
import random
import torch
import torch.optim as optim

from tensorflow import keras
from tensorflow.keras import Sequential
from collections import deque
from tensorflow.keras.layers import Dense
import matplotlib.pyplot as plt
from tensorflow.keras.activations import relu, linear

from ReplayMemory import Memory
from AgentBase import Agent

class KerasDQN(Agent):

    def __init__(self, state_space : int, action_space : int,
                model_type="FNN",
                epsilon=1,
                epsilon_min=0.01,
                epsilon_decay=0.996,
                gamma=0.99,
                batch_size=64,
                lr=1e-4,
                memory=Memory(1e6), 
                hidden_layer_one_dims=288,
                hidden_layer_two_dims=352,
                model=None
                ):
        self.action_space = action_space
        self.state_space = state_space
        self.epsilon = epsilon # exploration parameter
        self.epsilon_min = epsilon_min
        self.epsilon_decay = epsilon_decay
        self.gamma = gamma
        self.batch_size = batch_size
        self.lr = lr
        self.memory = memory
        self.hidden_layer_one_dims = hidden_layer_one_dims
        self.hidden_layer_two_dims = hidden_layer_two_dims
        
        if model is not None:
            self.model = model
        else:
            if model_type == "FNN":
                self.model = self.build_FNN_model()
            elif model_type == "RNN":
                self.model = self.build_RNN_model()
            else:
                raise ValueError("model_type is \"" + model_type + "\", which is not valid.")

    def build_FNN_model(self):
        model = Sequential()
        model.add(keras.Input(shape=(1, self.state_space)))
 
        model.add(Dense(self.hidden_layer_one_dims, activation=relu))
        model.add(Dense(self.hidden_layer_two_dims, activation=relu))
        model.add(Dense(self.action_space, activation=linear))
        model.compile(loss='mse', optimizer=keras.optimizers.Adam(lr=self.lr))
        return model
    
    def build_RNN_model(self):
        raise NotImplementedError

    def remember(self, state, action, reward, next_state, done):
        self.memory.remember(state, action, reward, next_state, done)

    def choose_action(self, state):
        if np.random.rand() <= self.epsilon:
            action = random.randrange(self.action_space)
        else:
            act_values = self.model(state) # Calling with this syntax is faster for small batch sizes
            action = np.argmax(act_values[0])

        return action

    def learn(self):
        if len(self.memory) < self.batch_size:
            return

        minibatch = self.memory.sample(self.batch_size)
        states = np.array([i[0] for i in minibatch])
        actions = np.array([i[1] for i in minibatch])
        rewards = np.array([i[2] for i in minibatch])
        next_states = np.array([i[3] for i in minibatch])
        dones = np.array([i[4] for i in minibatch])

        states = np.squeeze(states)
        next_states = np.squeeze(next_states)

        targets = rewards + self.gamma*(np.amax(self.model.predict_on_batch(next_states), axis=1))*(1-dones)
        targets_full = self.model.predict_on_batch(states)
        ind = np.array([i for i in range(self.batch_size)])
        targets_full[[ind], [actions]] = targets

        self.model.fit(states, targets_full, epochs=1, verbose=0)
        if self.epsilon > self.epsilon_min:
            self.epsilon *= self.epsilon_decay

