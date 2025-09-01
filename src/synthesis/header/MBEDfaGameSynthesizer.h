// This header defines the interface of
// MBEDfaGameSynthesizer type
// Used to perfrom co operative synthesis

#ifndef MBE_DFA_GAME_SYNTHESIZER_H
#define MBE_DFA_GAME_SYNTHESIZER_H

#include "Quantification.h"
#include "SymbolicStateDfa.h"
#include "Synthesizer.h"
#include "Transducer.h"

namespace Syft {

    class MBEDfaGameSynthesizer : public Synthesizer<SymbolicStateDfa> {

        protected:
            std::shared_ptr<VarMgr> var_mgr_;
            std::shared_ptr<VarMgr> var_mgr_neg_; //ELISA ENV
            Player starting_player_;
            Player protagonist_player_;
            std::vector<int> initial_vector_;
            std::vector<CUDD::BDD> transition_vector_;
            std::vector<CUDD::BDD> transition_vector_star_; //ELISA ENV
            std::unique_ptr<Quantification> quantify_independent_variables_;
            std::unique_ptr<Quantification> quantify_independent_variables_star_;
            std::unique_ptr<Quantification> quantify_non_state_variables_;

            CUDD::BDD preimage(const CUDD::BDD &winning_states) const;  // Used to compute function t_hat in symbolic synthesis

            CUDD::BDD preimage_star(const CUDD::BDD &winning_states) const;  // Used to compute function t_star in symbolic synthesis


            CUDD::BDD project_into_states(const CUDD::BDD &winning_moves) const;    // Used to compute function w in symbolic synthesis

            std::unordered_map<int, CUDD::BDD> synthesize_strategy(const CUDD::BDD &winning_moves) const;

            bool includes_initial_state(const CUDD::BDD &winning_states) const;

            public: //ELISA ENV
                MBEDfaGameSynthesizer(SymbolicStateDfa spec, SymbolicStateDfa neg_env, Player starting_player, Player protagonist_player);

                virtual SynthesisResult run()
                    const override = 0;
    };

} 
#endif // MBE_DFA_GANE_SYNTHESIZER_H