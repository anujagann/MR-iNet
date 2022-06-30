# Change Log

##
Author: Collin Farquhar
Date: 05-19-21

### Description
The delay when switching from TX to IDLE ensures that we model the behavior that when two nodes transmit to each other at the exact same time they cannot receive from each other. Previously, this was possible because the nodes switched from TX to IDLE instanteously, meaning that they could receive when the other nodes transmission arrived due to propagation delay.  

In the CdmaPhyDerived constructor, I set m_state_delay to be equal to the propagation delay over 1000m.

### Added
1. m_state_delay to cdma-phy-derived.h
2. ScheduledSetStateToIdle
3. CdmaPhyDerived::SetStateToIdle
4. Test file, pairless.cc

### Removed
1. m_last_state from CdmaPhyDerived
2. CdmaPhyDerived::IsLastStateTx
3. CdmaPhy::IsLastStateTx 

### Changed
1. cdma-phy-derived.h
2. cdma-phy-derived.cc
 
### Fixed / Altered
1. See above 

### Operation verification
Ran pairless.cc. When the nodes are transmitting at the same time and are less than 1000m apart they do not receive from each other. If they transmissions are offset in time and/or the nodes are fartherthen 1000m then they can receive from each other.
 
