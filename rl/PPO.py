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
** File: rl/PPO.py

'''






# Code referenece: https://github.com/higgsfield/RL-Adventure-2/blob/master/3.ppo.ipynb




import math
import random

import gym
import numpy as np

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.distributions import Normal
from torch.autograd import Variable
from torch.distributions import Categorical

from matplotlib import pyplot as plt

from AgentBase import Agent

use_cuda = torch.cuda.is_available()
device   = torch.device("cuda" if use_cuda else "cpu")

# From ActorCritic.py
class ActorCriticNetworks(nn.Module):
    def __init__(self, num_inputs, num_actions, hidden_size):
        super(ActorCriticNetworks, self).__init__()

        self.num_actions = num_actions
        self.critic_linear1 = nn.Linear(num_inputs, hidden_size)
        self.critic_linear2 = nn.Linear(hidden_size, 1)

        self.actor_linear1 = nn.Linear(num_inputs, hidden_size)
        self.actor_linear2 = nn.Linear(hidden_size, num_actions)
    
    def forward(self, state):
        #state = Variable(torch.from_numpy(state).float().unsqueeze(0))
        value = F.relu(self.critic_linear1(state))
        value = self.critic_linear2(value)
        
        state1 = state.clone()
        policy_dist = F.relu(self.actor_linear1(state1), inplace=False)
        #policy_dist = F.softmax(self.actor_linear2(policy_dist), dim=1)
        policy_dist = F.softmax(self.actor_linear2(policy_dist))

        return value, policy_dist

class PPO(Agent):
    def __init__(self, state_space, action_space,
                model=None,
                hidden_size=256,
                learning_rate=3e-4,
                optimizer=optim.Adam,
                training_schedule=10,
                ppo_epochs=4,
                mini_batch_size=5
                ):
        """
        Inputs:
            - optimizer [torch optimizer]
            - training_schedule [string, int]: If training is episodic, then use the string "episodic". Otherwise, use 
            an integer x, and the agent will train every x steps.
        """
        self.state_space = state_space
        self.action_space = action_space
        self.hidden_size = hidden_size
        self.learning_rate = learning_rate
        self.training_schedule = training_schedule
        self.ppo_epochs = ppo_epochs
        self.mini_batch_size = mini_batch_size
        
        self.step = 0

        if model == None:
            self.model = ActorCriticNetworks(state_space, action_space, hidden_size)
        else:
            self.model = model

        self.optimizer = optimizer(self.model.parameters(), lr=self.learning_rate)

        self.log_probs = []
        self.values    = []
        self.states    = []
        self.actions   = []
        self.rewards   = []
        self.done      = False 
        self.masks     = []
        self.next_state = None

    def choose_action(self, state):
        state = torch.FloatTensor(state).to(device)
        value, policy_dist = self.model(state)

        dist = policy_dist.detach().numpy() 
        action = np.random.choice(self.action_space, p=np.squeeze(dist))

        log_prob = torch.log(policy_dist.squeeze(0)[action])

        self.values.append(value)
        self.log_probs.append(log_prob)

        return action

    def remember(self, state, action, reward, next_state, done):
        state = torch.FloatTensor(state).to(device)
        self.states.append(state)
        self.actions.append(action)
        self.rewards.append(reward)
        self.next_state = next_state
        self.done = done
        self.masks.append(1 - done)

    def learn(self):
        """ Train the agent """
        if self.training_schedule == "episodic":
            if self.done == True:
                self.train()
        elif (self.step % self.training_schedule == 0):
            self.train()
        
    def train(self):
        # Get the value estimate for the next state
        next_state = torch.FloatTensor(self.next_state).to(device)
        next_value, _ = self.model(next_state)

        # Compute returns using Generalized Advantage Estimates
        returns = self.compute_gae(next_value, self.rewards, self.masks, self.values)
        returns = torch.cat(returns).detach()

        # Convert to tensors
        log_probs = torch.Tensor(self.log_probs).detach()
        values = torch.cat(self.values).detach()
        states = torch.stack(self.states)
        actions = torch.Tensor(self.actions)

        advantage = returns - values

        # Train neural networks with the PPO update
        self.ppo_update(self.ppo_epochs, self.mini_batch_size, states, actions, log_probs, returns, advantage)     

        # Clear past trajectories
        self.log_probs = []
        self.values    = []
        self.states    = []
        self.actions   = []
        self.rewards   = []
        self.done      = False 
        self.masks     = []
        self.next_state = None
        

    def compute_gae(self, next_value, rewards, masks, values, gamma=0.99, tau=0.95):
        """ Generalized Advantage Estimation """
        values = values + [next_value]
        gae = 0
        returns = []
        for step in reversed(range(len(rewards))):
            delta = rewards[step] + gamma * values[step + 1] * masks[step] - values[step]
            gae = delta + gamma * tau * masks[step] * gae
            returns.insert(0, gae + values[step])
        return returns
    
    def ppo_iter(self, mini_batch_size, states, actions, log_probs, returns, advantage):
        """ Yields a minibatach """
        #batch_size = states.size(0) # original 
        batch_size = len(states)
        
        for _ in range(batch_size // mini_batch_size):
            rand_ids = np.random.randint(0, batch_size, mini_batch_size)
            yield states[rand_ids], actions[rand_ids], log_probs[rand_ids], returns[rand_ids], advantage[rand_ids]

    def ppo_update(self, ppo_epochs, mini_batch_size, states, actions, log_probs, returns, advantages, clip_param=0.5):
        """ Performs PPO update to neural networks """
        for _ in range(ppo_epochs):
            for state, action, old_log_probs, return_, advantage in self.ppo_iter(mini_batch_size, states, actions, log_probs, returns, advantages):
                value, dist = self.model(state)

                dist = Categorical(dist) # Convert to torch distribution for the following methods
                entropy = 10*dist.entropy().mean()
                new_log_probs = dist.log_prob(action)

                ratio = (new_log_probs - old_log_probs).exp()
                surr1 = ratio * advantage
                surr2 = torch.clamp(ratio, 1.0 - clip_param, 1.0 + clip_param) * advantage

                actor_loss  = - torch.min(surr1, surr2).mean()
                critic_loss = (return_ - value).pow(2).mean()

                loss = 0.5 * critic_loss + actor_loss - 0.001 * entropy

                self.optimizer.zero_grad()
                loss.backward()
                self.optimizer.step()

def main():
  ## Config ##
  ENV = "CartPole-v1"
  RANDOM_SEED = 1
  N_EPISODES = 200
  render_n = 1 

  # random seed (reproduciblity)
  np.random.seed(RANDOM_SEED)

  # set the env
  env = gym.make(ENV) # env to import
  env.seed(RANDOM_SEED)
  env.reset() # reset to en

  agent = PPO(env.observation_space.shape[0], env.action_space.n, training_schedule="episodic")                  

  total_rewards = np.zeros(N_EPISODES)

  # Loop over episodes
  for episode in range(N_EPISODES):
    state = env.reset()
    done = False          
    episode_reward = 0 
    
    # Loop over steps
    while not done:
      # play an action and record the game state & reward per episode
      action = agent.choose_action(state)
      next_state, reward, done, _ = env.step(action)
      
      agent.remember(state, action, reward, next_state, done)
      agent.learn()

      state = next_state
      episode_reward += reward

      if episode % render_n == 0: ## render env to visualize.
        env.render()

    total_rewards[episode] = episode_reward
    print("Episode", episode, "reward = ", episode_reward)
    
  env.close()
  plt.plot(total_rewards)
  plt.title("Episode reward")
  plt.show()
  plt.show()

if __name__ == "__main__":
  main()