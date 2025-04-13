#include "JokerReachabilitySynthesizer.h"
#include <cassert>

namespace Syft {

  JokerReachabilitySynthesizer::JokerReachabilitySynthesizer(SymbolicStateDfa spec,
						Player starting_player, Player protagonist_player,
						 CUDD::BDD goal_states,
             CUDD::BDD goal_moves, //ELISA CHANGED
						 CUDD::BDD state_space)
    : JokerDfaGameSynthesizer(spec, starting_player, protagonist_player)
    , goal_states_(goal_states), goal_moves_(goal_moves), state_space_(state_space) //ELISA CHANGED
{}


SynthesisResult JokerReachabilitySynthesizer::run() const { ///ELISA CHANGED
  SynthesisResult result;
  CUDD::BDD winning_states = state_space_ & goal_states_;
  CUDD::BDD winning_moves = goal_moves_; //ELISA CHANGED
  CUDD::BDD env_error_bdd;
  int env_error_index;
  bool PDDL_domain = false;
  std::unordered_map<int, std::string> id_to_var = var_mgr_->get_index_to_name(); 
  for (int i = 0; i < id_to_var.size(); ++i) {
      std::string var = id_to_var[i];
      if (var == "env_err") {
            PDDL_domain = true;
            env_error_index = i;
            env_error_bdd = var_mgr_->name_to_variable("env_err");
            break;
      }
  } 
  
  if (!PDDL_domain)
      env_error_bdd = var_mgr_->cudd_mgr()->bddZero();
 
  int k = 1;
  
  while (true) {
    CUDD::BDD new_winning_moves_star = winning_moves |                                           
                                  (state_space_ & (!winning_states) & preimage_star(winning_states & !env_error_bdd));    //t_star

    CUDD::BDD new_winning_states_star = project_into_states(new_winning_moves_star);        //w_star

    CUDD::BDD winning_moves_hat = new_winning_moves_star;
    CUDD::BDD winning_states_hat = new_winning_states_star;

    CUDD::BDD new_winning_moves;
    CUDD::BDD new_winning_states;
    while (true) {
      new_winning_moves = winning_moves_hat |
                                    (state_space_ & (!winning_states_hat) & preimage(winning_states_hat)| env_error_bdd);
  
      new_winning_states = project_into_states(new_winning_moves);
  
      if (includes_initial_state(new_winning_states) || new_winning_states == winning_states_hat) {
          break;
      }
  
      winning_moves_hat = new_winning_moves;
      winning_states_hat = new_winning_states;
    }

    if (includes_initial_state(new_winning_states)) {
        result.realizability = true;
        result.winning_states = new_winning_states;
        result.winning_moves = new_winning_moves;
        std::unordered_map<int, CUDD::BDD> strategy = synthesize_strategy(
              new_winning_moves);

        result.transducer = std::make_unique<Transducer>(
              var_mgr_, initial_vector_, strategy, spec_.transition_function(),
              starting_player_, protagonist_player_);
        result.cost = k;
        return result;

    } else if (new_winning_states == winning_states) {
        result.realizability = false;
        result.winning_states = new_winning_states;
        result.winning_moves = new_winning_moves;
        // result.transducer = nullptr;
        std::unordered_map<int, CUDD::BDD> strategy = synthesize_strategy(
          new_winning_moves);

        result.transducer = std::make_unique<Transducer>(
              var_mgr_, initial_vector_, strategy, spec_.transition_function(),
              starting_player_, protagonist_player_);
        return result;
    
    }

    winning_moves = new_winning_moves;
    winning_states = new_winning_states;
    k++;
  }

}

}
