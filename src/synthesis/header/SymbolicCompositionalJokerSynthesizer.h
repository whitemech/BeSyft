/*
* This header declares the class SymbolicCompositionalJokerSynthesizer
* which implements the symbolic-compositional approach to Joker synthesis
*/

#ifndef SYFT_SYMBOLICCOMPOSITIONALJOKERSYNTHESIZER_H
#define SYFT_SYMBOLICCOMPOSITIONALJOKERSYNTHESIZER_H

#include"ExplicitStateDfaMona.h"
#include"ExplicitStateDfa.h"
#include"SymbolicStateDfa.h"
#include"ReachabilitySynthesizer.h"
#include"CoOperativeReachabilitySynthesizer.h"
#include"JokerReachabilitySynthesizer.h"
#include"InputOutputPartition.h"
#include"Stopwatch.h"
#include"spotparser.h"

namespace Syft {

	class SymbolicCompositionalJokerSynthesizer {
	
		protected:
			std::shared_ptr<Syft::VarMgr> var_mgr_;

			std::string agent_specification_;
			std::string environment_specification_;

			Player starting_player_;

			std::vector<SymbolicStateDfa> symbolic_dfas_;
			std::vector<SymbolicStateDfa> arena_;

			InputOutputPartition partition_;

			std::vector<double> running_times_;
		public:
		
			/**
			* \brief Construct an object presenting best-effort synthesis problem (E, Phi)
			* 
			* \param var_mgr Dictionary storing variables of the problem
			* \param agent_specification LTLf agent goal in Lydia syntax
			* \param environment_assumption LTLf environment specification in Lydia syntax
			* \param partition Partitioning of problem variables
			* \param starting_player Player who moves first each turn
			* 
			*/
			SymbolicCompositionalJokerSynthesizer(std::shared_ptr<VarMgr> var_mgr,
									std::string agent_specification,
									std::string environment_specification,
									InputOutputPartition partition,
									Player starting_player);
			
			/**
			 * @brief Solves symbolic DFA games to compute Joker winning strategies
			 * 
			 * @return SynthesisResult: Joker winning strategy.
			 */
			virtual SynthesisResult run() final;

			/**
			* \brief Merges two transducers into a best-effort strategy
			*
			* \param adversarial_result Reactive synthesis result
			* \param coopeartive_result Cooperative synthesis result
			* \param filename A path to the output .dot file
			* \return void. Prints implementation function of best-effort strategy into a .dot file
			*/
			// void merge_and_dump_dot(const SynthesisResult& adversarial_result, 
			// 						const SynthesisResult& cooperative_result, 
			// 						const string& filename) const;

			/**
			 * @brief Returns running times of major operations during synthesis
			 * 
			 * @return std::vector<double> storing running times  
			 */
			std::vector<double> get_running_times() const;

			/**
			 * @brief Executes interactively the synthesized Joker strategy; user plays as the env.
			 * 
			 */
			void interactive(const SynthesisResult& joker_result) const; 
	};
}
#endif