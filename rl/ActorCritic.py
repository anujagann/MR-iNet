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
** File: rl/ActorCritic.py

'''






# Reference: https://towardsdatascience.com/understanding-actor-critic-methods-931b97b6df3f

import sys
import torch  
import gym
import numpy as np  
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.autograd import Variable
import matplotlib.pyplot as plt
from AgentBase import Agent
import random

total_entropies = []

class ActorCriticNetworks(nn.Module):
    def __init__(self, num_inputs, num_actions, hidden_sizes):
        super(ActorCriticNetworks, self).__init__()

        self.num_actions = num_actions
        self.critic_hidden_layers = []
        self.actor_hidden_layers = []
        for i in range(len(hidden_sizes)):
            if i == 0:
                actor_layer = nn.Linear(num_inputs, hidden_sizes[0])
                critic_layer = nn.Linear(num_inputs, hidden_sizes[0])
            else:
                actor_layer = nn.Linear(hidden_sizes[i - 1], hidden_sizes[i])
                critic_layer = nn.Linear(hidden_sizes[i - 1], hidden_sizes[i])
            
            self.actor_hidden_layers.append(actor_layer)
            self.critic_hidden_layers.append(critic_layer)

        self.actor_out = nn.Linear(hidden_sizes[-1], num_actions)
        self.critic_out = nn.Linear(hidden_sizes[-1], 1)

        # original
        """
        self.critic_linear1 = nn.Linear(num_inputs, hidden_size)
        self.critic_linear2 = nn.Linear(hidden_size, 1)

        self.actor_linear1 = nn.Linear(num_inputs, hidden_size)
        self.actor_linear2 = nn.Linear(hidden_size, num_actions)
        """
    
    def forward(self, state):
        # original
        """
        state = Variable(torch.from_numpy(state).float().unsqueeze(0))
        value = F.relu(self.critic_linear1(state))
        value = self.critic_linear2(value)
        
        state1 = state.clone()
        policy_dist = F.relu(self.actor_linear1(state1), inplace=False)
        policy_dist = F.softmax(self.actor_linear2(policy_dist), dim=1)
        """
        state = Variable(torch.from_numpy(state).float().unsqueeze(0))

        critic_layer_input = state
        actor_layer_input = state.clone() # not sure if it's necessary to clone

        for i in range(len(self.critic_hidden_layers)):
            critic_layer_input = F.relu(self.critic_hidden_layers[i](critic_layer_input))
        value = self.critic_out(critic_layer_input)
        

        for i in range(len(self.actor_hidden_layers)):
            actor_layer_input = F.relu(self.actor_hidden_layers[i](actor_layer_input), inplace=False)
        policy_dist = F.softmax(self.actor_out(actor_layer_input), dim=1)

        return value, policy_dist

class ActorCritic(Agent):
    def __init__(self, state_space, action_space,
                model=None,
                hidden_sizes=[256],
                learning_rate=3e-4,
                gamma=0.99,
                optimizer=optim.Adam,
                training_schedule="episodic",
                save_outputs=False,
                epsilon=None,
                epsilon_min=0.05,
                epsilon_decay=0.99):
        """
        Inputs:
            - hidden_sizes [list[int]]: list of sizes for each hidden layer
            - optimizer [torch optimizer]
            - training_schedule [string, int]: If training is episodic, then use the string "episodic". Otherwise, use 
            an integer x, and the agent will train every x steps.
        """
        self.state_space = state_space
        self.action_space = action_space
        self.hidden_sizes = hidden_sizes
        self.learning_rate = learning_rate
        self.gamma = gamma
        self.training_schedule = training_schedule
        self.save_outputs = save_outputs
        self.epsilon = epsilon
        self.epsilon_min = epsilon_min
        self.epsilon_decay = epsilon_decay
        
        self.step = 0

        if model == None:
            self.model = ActorCriticNetworks(state_space, action_space, hidden_sizes)
        else:
            self.model = model

        self.optimizer = optimizer(self.model.parameters(), lr=self.learning_rate)
        
        # Track for batches
        self.log_probs = []
        self.values = []
        self.rewards = []
        self.entropy = 0 
        self.entropy_list = []
        self.rewards = []
        self.next_states = []
        self.done = False


        self.Q_values = [] # Probabilities in this case
        self.critic_values = []

    def choose_action(self, state):
        """ Return the agent's action for the next time-step """
        value, policy_dist = self.model.forward(np.squeeze(state))
        value = value.detach().numpy()[0,0]

        dist = policy_dist.detach().numpy() 

        if self.save_outputs == True:
            self.Q_values.append(dist[0])
            self.critic_values.append(value)

        action = np.random.choice(self.action_space, p=np.squeeze(dist))

        # Epsilon exploration
        if self.epsilon is not None:
            if np.random.rand() <= self.epsilon:
                action = random.randrange(self.action_space)
        
            if self.epsilon > self.epsilon_min:
                self.epsilon *= self.epsilon_decay

        log_prob = torch.log(policy_dist.squeeze(0)[action])
        # Testing entropy
        entropy = -np.sum(np.mean(dist) * np.log(dist)) # is mean(dist) an error?
        #entropy = -np.sum([p * np.log(p) for p in dist]) # Entropy test change
        
        # Save values for training
        self.values.append(value)
        self.log_probs.append(log_prob)
        self.entropy += entropy
        self.entropy_list.append(entropy)

        return action 

    def remember(self, state, action, reward, next_state, done):
        """ Store trajectory to in the agent's experience replay memory """
        self.step += 1
        self.next_states.append(next_state)
        self.rewards.append(reward)
        self.done = done

    def train(self):
        last_state = self.next_states[-1]
        Qval, _ = self.model.forward(np.squeeze(last_state)) # matching choose_action
        Qval = Qval.detach().numpy()[0,0]

        # compute Q values
        Qvals = np.zeros_like(self.values)
        for t in reversed(range(len(self.rewards))):
            Qval = self.rewards[t] + self.gamma * Qval
            Qvals[t] = Qval

        # Convert to tensors
        values_tensor = torch.FloatTensor(self.values)
        Qvals = torch.FloatTensor(Qvals)
        log_probs_tensor = torch.stack(self.log_probs)
        
        # Compute advantage
        advantage = Qvals - values_tensor

        # Create a joint loss function
        actor_loss = (-log_probs_tensor * advantage).mean()
        critic_loss = 0.5 * advantage.pow(2).mean()
        #ac_loss = actor_loss + critic_loss + 0.001 * self.entropy # test entropy
        ac_loss = actor_loss + critic_loss + 0.1 * self.entropy # Change for run15

        # Entropy test change
        """
        entropy = np.mean(self.entropy_list)
        ac_loss = actor_loss + critic_loss + 0.001 * entropy 
        """

        # Zero gradeints, compute new gradients, update model parameters with optimizer
        self.optimizer.zero_grad()
        ac_loss.backward()
        self.optimizer.step()

        # Since this is on-policy, reset these tracked quantities
        self.log_probs = []
        self.values = []
        self.rewards = []
        self.entropy = 0 
        self.rewards = []
        self.next_states = []

    def learn(self):
        """ Train the agent """
        if self.training_schedule == "episodic":
            if self.done == True:
                total_entropies.append(self.entropy)
                self.train()
        elif (self.step % self.training_schedule == 0):
            self.train()
       

def main():
  ## Config ##
  N_EPISODES = 5000
  ENV = "CartPole-v1"
  RANDOM_SEED = 1
  render_n = 1 

  # random seed (reproduciblity)
  np.random.seed(RANDOM_SEED)

  # set the env
  env = gym.make(ENV) # env to import
  env.seed(RANDOM_SEED)
  env.reset() # reset to en

  agent = ActorCritic(env.observation_space.shape[0], env.action_space.n,
                        epsilon=0.99)                  

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
  plt.title("Episode entropy")
  plt.plot(total_entropies)
  plt.show()

if __name__ == "__main__":
  main()