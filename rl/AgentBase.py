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
** File: rl/AgentBase.py

'''





from abc import ABC, abstractmethod

class Agent(ABC):
    @abstractmethod
    def __init__(self, state_space, action_space):
        pass

    @abstractmethod
    def choose_action(self, state):
        """ Return the agent's action for the next time-step """
        pass

    @abstractmethod
    def remember(self, state, action, reward, next_state, done):
        """ Store trajectory in the agent's experience replay memory """
        pass

    @abstractmethod
    def learn(self):
        """ Train the agent """
        pass
