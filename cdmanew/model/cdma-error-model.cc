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
** File description: Error Model for CDMA
** File: model/cdma-error-model.cc
*/
#include <cmath>
#include <boost/numeric/ublas/matrix.hpp>
#include <iostream>
#include "cdma-error-model.h"
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/enum.h"
#include "ns3/integer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "cdma-phy.h"

using namespace boost::numeric::ublas;
namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("CDMA_ErrorModel");	// define a log component with the name "CDMA_ErrorModel"
 
    NS_OBJECT_ENSURE_REGISTERED (CDMA_ErrorModel);	// register CDMA_ErrorModel class with the TypeId system
    TypeId CDMA_ErrorModel::GetTypeId (void)	// returns meta-information about CDMA_ErrorModel class 
    { 										// including parent class, group name, constructor, and attributes
        static TypeId tid = TypeId ("ns3::CDMA_ErrorModel")
            .SetParent<RateErrorModel> ()
            .SetGroupName("Cdma")
            .AddConstructor<CDMA_ErrorModel> ()
            .AddAttribute ("ModulationOrder", "The modulation order (M)",
                            IntegerValue (0),
                            MakeIntegerAccessor (&CDMA_ErrorModel::mod_order),
                            MakeIntegerChecker<int> ())
            .AddAttribute ("SNR", "Signal-to-Noise Ratio",
                            DoubleValue (0.0),
                            MakeDoubleAccessor (&CDMA_ErrorModel::SNR),
                            MakeDoubleChecker<double> ())
            .AddAttribute ("RanVar1", "The decision variable attached to this error model.",
                            StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
                            MakePointerAccessor (&CDMA_ErrorModel::m_ranvar),
                            MakePointerChecker<RandomVariableStream> ())
            .AddAttribute ("ModulationScheme", "The modulation scheme used in this model", 
                            EnumValue (BPSK),
                            MakeEnumAccessor (&CDMA_ErrorModel::mod_scheme),
                            MakeEnumChecker (BPSK, "BPSK",
                                            QPSK, "QPSK"))
        ;
        return tid;
    }   

// constructor
CDMA_ErrorModel::CDMA_ErrorModel ()
{

	NS_LOG_FUNCTION (this);
	SNR = 0;
	mod_order = 0;
	mod_scheme = BPSK;

}

// destructor
CDMA_ErrorModel::~CDMA_ErrorModel ()
{
	NS_LOG_FUNCTION (this);
}

CDMA_ErrorModel::ModScheme		 
CDMA_ErrorModel::GetScheme (void) const	// returns the modulation scheme used
{
	NS_LOG_FUNCTION (this);
	return mod_scheme; 
}

void
CDMA_ErrorModel::SetScheme (ModScheme scheme)	// sets the value of the modulation scheme used
{
	NS_LOG_FUNCTION (this << scheme);
	mod_scheme = scheme;
	if (scheme == BPSK)
		mod_order = 2;
}


void CDMA_ErrorModel::SetScheme (std::string scheme){			// sets the value of the modulation scheme used
	NS_LOG_FUNCTION (this << scheme);
	if(scheme=="BPSK"){
		SetScheme(CDMA_ErrorModel::BPSK);
	}
	else if(scheme=="QPSK"){
		SetScheme(CDMA_ErrorModel::QPSK);
	}

}
// SetRandomVariable method assigns a random variable stream to be used by this model
void 
CDMA_ErrorModel::SetRandomVariable (Ptr<RandomVariableStream> ranvar)
{
	NS_LOG_FUNCTION (this << ranvar);
	m_ranvar = ranvar;
}

//AssignStreams method assigns a fixed stream number to the random variables used by this model
int64_t 
CDMA_ErrorModel::AssignStreams (int64_t stream)
{
	NS_LOG_FUNCTION (this << stream);
	m_ranvar->SetStream (stream);
	return 1;
}
  
// GetModulationOrder method returns the modulation order (M)
int
CDMA_ErrorModel::GetModulationOrder (void) const 
{ 
	NS_LOG_FUNCTION (this);
	return mod_order; 
}

// SetModulationOrder method sets the modulation order value
void 
CDMA_ErrorModel::SetModulationOrder (int m_order) 
{ 
	NS_LOG_FUNCTION (this << m_order);
	mod_order = m_order; 
}


// GetSNR method returns the signal-to-noise ratio (SNR) value
double
CDMA_ErrorModel::GetSNR (void) const 
{ 
	NS_LOG_FUNCTION (this);
	return SNR; 
}

// SetSNR method sets the SNR value
void 
CDMA_ErrorModel::SetSNR (double snr) 
{ 
	NS_LOG_FUNCTION (this << snr);
	SNR = snr; 
}

// CalculateErrorRate calculates BER value according to modulation scheme
double
CDMA_ErrorModel::CalculateErrorRate (void)
{
	NS_LOG_FUNCTION (this);
	if (mod_scheme == BPSK)
		return CalculateBER_BPSK();
	/*else if (mod_scheme == QPSK)
		return CalculateBER_QPSK();*/
	return 0;
}

// CalculateBER method calculates the BER of VPPM model using alpha, beta and SNR
double CDMA_ErrorModel::CalculateBER_BPSK (void)
{
	NS_LOG_FUNCTION (this);
	int sgain = m_phy->GetSpreadingGain();
	// once NW layer is in place it would allow for nodeid to spreading seq mapping
	// for now using hardcoded values of spreading signature
	uint32_t srcnodeid = 1;
	boost::numeric::ublas::vector<std::complex<double>>srcsig = m_phy->GetUserSignature(srcnodeid);
	boost::numeric::ublas::vector<std::complex<double>>mysig = m_phy->GetSpreadingSeq();
	std::complex<double> norm_cross_corr = boost::numeric::ublas::inner_prod(mysig,srcsig); 
	//std::vector<std::complex<double>> rxsig
	double x; // the vaue to calculate Q(x)
	x = 1;
	// Calculate BER = Q(x)
	// this is just a dummy computation.. need replacing with actual ber approximation
	// Q-function is the tail probability of the standard normal distribution
	double Q = (1/sgain) *abs(norm_cross_corr) * 0.5 * erfc(x / std::sqrt(2));
	SetRate(Q);	// call SetRate method in parent class RateErrorModel to set m_rate to BER value
	SetUnit(ERROR_UNIT_BIT);
	return Q;
}

// DoCorrupt method determines if the packet is corrupted according to the error model used
bool CDMA_ErrorModel::IsCorrupt (Ptr<Packet> p)
{
	NS_LOG_FUNCTION (this << p);
	
	if (mod_scheme == BPSK || mod_scheme == QPSK)
	{
		// check if the model is enabled
		if (!IsEnabled ())
		{
			return false;
		}
		
		// computes pkt error rate from symbol error rate
		double symbol_size = log2(mod_order);	// symbol size in bits
		double symbols_per_pkt = static_cast<double>(8 * p->GetSize ()) / symbol_size; // no. of symbols per packet
		// Compute pkt error rate by finding the complement of the probablility 
		// that a packets is not corrupted
		// = (1 - the probability that all symbols in pkt are not corrupted)
		double per = 1 - std::pow (1.0 - GetRate(), symbols_per_pkt);
		// the pkt is corrupted according to PER
		// using random variable m_ranvar
		return (m_ranvar->GetValue () < per);
	}
	
	else
	{
		RateErrorModel::SetRandomVariable(m_ranvar);
		return RateErrorModel::IsCorrupt(p);
	}
}


// DoReset method does nothing
void CDMA_ErrorModel::DoReset (void)
{ 
	NS_LOG_FUNCTION (this);
	/* re-initialize any state; no-op for now */ 
}

} // namespace ns3
