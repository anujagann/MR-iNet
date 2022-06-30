# 3 pairs doing TX power control with 3 power levels 

## Topology:
3 pairs positioned in a 2-D grid.
  
## Mobility model:
  - Constant

## Input space: 
  - 2 distances from Tx to receivers
  - SINR
  - Interference-caused
  - # successful transmissions (in the last time step) 
  - TxPower
  - Interference-sensed
  - buffer size

## Action space:  
  - 3 power levels. The agents transmit on every-time step.

## Notes:
  - run1: DQN, 10,000 steps
