#ifndef MBE_REACHABILITY_SYNTHESIZER_H
#define MBE_REACHABILITY_SYNTHESIZER_H

#include "MBEDfaGameSynthesizer.h"

namespace Syft {

/**
 * \brief A synthesizer for reachability game given as a symbolic-state DFA.
 */
class MBEReachabilitySynthesizer : public MBEDfaGameSynthesizer {
 private:

  CUDD::BDD goal_states_;
  CUDD::BDD goal_moves_;
  CUDD::BDD state_space_;
  CUDD::BDD env_error_state_;
  
 public:

  /**
   * \brief Construct a synthesizer for the given reachability game.
   *
   * \param spec A symbolic-state DFA representing the reachability game.
   * \param starting_player The player that moves first each turn.
   * \param goal_states The set of states that the agent must reach to win.
   */
  MBEReachabilitySynthesizer(SymbolicStateDfa spec, SymbolicStateDfa neg_env, Player starting_player, Player protagonist_player,
			  CUDD::BDD goal_states, CUDD::BDD goal_moves, CUDD::BDD state_space, CUDD::BDD env_error_state);

    /**
     * \brief Solves the reachability game.
     *
     * \return The result consists of
     * realizability
     * a set of agent MBE winning states
     * a transducer representing a MBE strategy or nullptr if the game is unrealizable.
     */
  virtual SynthesisResult run() const final;
};

}

#endif // MBE_REACHABILITY_SYNTHESIZER_H
