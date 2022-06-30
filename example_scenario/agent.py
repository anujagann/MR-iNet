#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import csv
import gym
import numpy as np
import sys
# Add the directory containing the RL algorithms to your path
#sys.path.insert(1, '<PATH TO RL FOLDER>/rl') # Relative import
from DQN import KerasDQN
import matplotlib.pyplot as plt
import argparse
from ns3gym import ns3env

from ActorCritic import ActorCritic 
from PPO import PPO

# --------------------> Save data to csv <--------------------
save = True

# --------------------> Initialize environment <--------------------
parser = argparse.ArgumentParser(description='Start simulation script on/off')
parser.add_argument('--start',
                    type=int,
                    default=0,
                    help='Start ns-3 simulation script 0/1, Default: 0') # Use 0 if running simulation in different terminal
parser.add_argument('--iterations',
                    type=int,
                    default=1,
                    help='Number of iterations, Default: 1')

args = parser.parse_args()
startSim = bool(args.start)
iterationNum = int(args.iterations)
iterationNum = 1

port = 5555
simTime = 1e4 # seconds
stepTime = 1  # seconds
seed = 0
simArgs = {"--simTime": simTime,
           "--stepTime": stepTime,
           "--testArg": 123}
debug = False

env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim, simSeed=seed, simArgs=simArgs, debug=debug) 
#env = ns3env.Ns3Env() # To run with one terminal

observation_space = env.observation_space
action_space = env.action_space
n_observations = observation_space.shape[0]

# -----------------------> Set up parameters <--------------------------
# Number of actions for RL model. Don't include the -1 action in the count 
# -1 is passed to by ns3-gym to indicate the RL is not to be used on this step.
n_actions = 3
n_agents = 3

# n_inputs = Distance to all other receivers, sinr, interfence-caused, # successful transmissions, tx power, 
# interference-sensed, and buffer size. 
n_inputs = n_agents + 6 

max_distance = 50 # For normalization
# -----------------------> End set up parameters <--------------------------

# --------------------> Create Agents <--------------------
# DQN
agents = [KerasDQN(n_inputs, n_actions, 
                   hidden_layer_one_dims=128, 
                   hidden_layer_two_dims=256,
                   batch_size=64,
                   epsilon_min=0.05) for _ in range(n_agents)]

# Actor Critic
"""
agents = [ActorCritic(n_inputs, n_actions) for _ in range(n_agents)]
"""

"""
# PPO
agents = [PPO(n_inputs, n_actions, training_schedule=10) for _ in range(n_agents)]
"""
# --------------------> End Create Agents <--------------------

def distance(x1, y1, x2, y2):
    """ Returns the 2D Euclidean distance between two points. """ 
    return np.sqrt((x1 - x2)**2 + (y1 - y2)**2)

def state_to_observations(state):
    """
    Positions are [Rx0_x, Rx0_y, Tx1_x, Tx1_y, ...], where the number is the node id. Trasmitter i communicates with 
    receiver (i - 1).
    """
    positions = state[:4*n_agents]
    other_obs = state[4*n_agents:]
    other_obs_per_agent = len(other_obs) // n_agents

    agent_states_list = []
    for i in range(n_agents):
        agent_state = []
        
        # Get distances from transmitter to receivers
        for j in range(len(positions) // 4):
            transmitter_id = 4*i + 2 # id of x-position of transmitter i in positions list; y-position is next element.
            receiver_id = 4*j # id of x-position of receiver j in positions list; y-position is next element.
            transmitter_x = positions[transmitter_id]
            transmitter_y = positions[transmitter_id + 1]
            receiver_x = positions[receiver_id]
            receiver_y = positions[receiver_id + 1]
            tx_to_rx_distance = distance(transmitter_x, transmitter_y, receiver_x, receiver_y)
            agent_state.append(tx_to_rx_distance / max_distance)

        obs_idx = i*other_obs_per_agent
        agent_state += other_obs[obs_idx: obs_idx + other_obs_per_agent] 

        agent_state = np.array(agent_state).reshape(1, -1) 
        agent_states_list.append(agent_state)

    return agent_states_list


def info_string_parser(info):
    """
    Converts a sting of comma separated values to a list of floats
    Input:
        - info (string): comma separated values
    Returns:
        - A list of floats
    """
    return [float(x) for x in info.split(',')]

# --------------------> Main loop <--------------------
currIt = 0
try:
    while True:
        stepIdx = 0
        rewards = []
        action_list = []
        states = []
        scores = [[] for _ in range(n_agents)]
        rewards = [] 

        state = env.reset()
        state = [np.zeros(n_inputs).reshape(1, -1) for _ in range(n_agents)]
        
        while True:
            actions = []
            action_probs = []
            for i in range(n_agents):
            # If the buffer is zero, don't use RL
                if state[i][0][-1] == 0:
                    actions.append(-1)
                    action_probs.append(-1)
                else:
                    action = agents[i].choose_action(state[i]) 
                    actions.append(action)

            next_state, reward, done, info = env.step(actions)
            next_state = state_to_observations(next_state)

            info_list = info_string_parser(info)

            for i in range(n_agents):
                agent_action = actions[i]
                if agent_action == -1: # RL agent not invoked. Do not save transition to memory
                    print("RL not used")
                    continue
                agent_state = state[i]
                agent_next_state = next_state[i]
                agent_reward = info_list[i]
                agents[i].remember(agent_state, agent_action, agent_reward, agent_next_state, done) 

                agents[i].learn()
            
            for i in range(n_agents):
                scores[i].append(info_list[i])

            rewards.append(info_list)
            action_list.append(actions)
            states.append(state)

            state = next_state
            
            stepIdx += 1
            if stepIdx % 100 == 0:
                print("Step: ", stepIdx)
                for i in range(n_agents):
                    print("mean (last 100)", np.mean(scores[i][-100:]))
                    if i == (n_agents - 1):
                        print()

            if done:
                # Record data in CSV
                if save == True:
                    data = [list(reward) + list(action) + list(np.array(state).flatten()) for reward, action, state in zip(rewards, action_list, states)]
                    with open("data" + ".csv", "w", newline="") as f:
                        writer = csv.writer(f)
                        writer.writerows(data)
                break

        currIt += 1
        if currIt == iterationNum:
            break

except KeyboardInterrupt:
    print("Ctrl-C -> Exit")
    
finally:
    env.close()
    print("RL Done")
