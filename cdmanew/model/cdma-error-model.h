/* ------------------------------------------------------------------------------
* Copyright 2022 ANDRO Computational Solutions, LLC.  All Rights Reserved
 ------------------------------------------------------------------------------
*/
/* ------------------------------------------------------------------------------
This file is part of the MR-iNET software codebase released for public use under 
the [Creative Commons Attribution - NonCommercial - ShareAlike 4.0 (CC BY-NC-SA 4.0) License]
(https://creativecommons.org/licenses/by-nc-sa/4.0/).

This material is based upon work supported by the US Army Contract No. W15P7T-20-C-0006.
Any opinions, findings, and conclusions or recommendation expressed in this material 
are those of the author(s) and do not necessarily reflect the views of the US Army.
* ------------------------------------------------------------------------------
*/
/*
* I-ROAM
** Author(s): Anu Jagannath
** Email:  ajagannath@androcs.com
** File description: Error model header
** File: model/cdma-error-model.h
*/
#ifndef CDMA_ERROR_MODEL_H
#define CDMA_ERROR_MODEL_H
 
#include <list>
#include "ns3/error-model.h"
#include "ns3/random-variable-stream.h"
#include <cmath>
#include <math.h>
  
namespace ns3 {
 
class Packet;
class CdmaPhy;

class CDMA_ErrorModel : public RateErrorModel	// This class is a subclass of RateErrorModel class
{
	public:
		// Public methods
		static TypeId GetTypeId (void); 

		CDMA_ErrorModel ();	// constructor
		virtual ~CDMA_ErrorModel ();	//destructor
		
		enum ModScheme	// enumeration for the two modulation schemes
		{
			BPSK,
			QPSK
		};
		
		CDMA_ErrorModel::ModScheme GetScheme (void) const;	// returns the modulation scheme used

		void SetScheme (ModScheme scheme);			// sets the value of the modulation scheme used
		
		void SetScheme (std::string scheme);			// sets the value of the modulation scheme used

		void SetRandomVariable (Ptr<RandomVariableStream> ranVar);	// assigns a random variable stream to be used by this model

		int64_t AssignStreams (int64_t stream);	// assigns a fixed stream number to the random variables used by this model
		
		double GetSNR (void) const;		// returns the signal-to-noise ratio (SNR)

		void SetSNR (double snr);		// sets the SNR value

		double CalculateErrorRate (void);		// calculates the error rate value according to modulation scheme
		
		bool IsCorrupt (Ptr<Packet> pkt);	// determines if the packet is corrupted according to the error model
		
		// methods for PAM
		int GetModulationOrder (void) const;	// returns the modulation order (M)
		void SetModulationOrder (int m_order);	// sets the modulation order value

	private:
		Ptr<CdmaPhy> m_phy;
		// Private methods
		virtual void DoReset (void);	// overrides DoReset method from RateErrorModel class
		double CalculateBER_BPSK (void);		// calculates the BER value 
        

		// Private class members
		int mod_order;	// the modulation order (M) 
		double SNR;		// signal-to-noise ratio

		Ptr<RandomVariableStream> m_ranvar;		// random variable stream
		CDMA_ErrorModel::ModScheme mod_scheme;	// the modulation scheme used
};

} // namespace ns3
#endif

