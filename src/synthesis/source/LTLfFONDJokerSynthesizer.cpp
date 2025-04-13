#include"LTLfFONDJokerSynthesizer.h"

namespace Syft {
    LTLfFONDJokerSynthesizer::LTLfFONDJokerSynthesizer(
        std::shared_ptr<VarMgr> var_mgr,
        const std::string& domain_file,
        const std::string& init_file,
        const std::string& goal_file
    ) : var_mgr_(var_mgr), domain_file_(domain_file), init_file_(init_file), goal_file_(goal_file) {}

    SynthesisResult LTLfFONDJokerSynthesizer::run() {
        // 1. construct DFA of planning domain
        std::cout << "[syft4fond] Transforming PDDL into DFA...";
        Stopwatch pddl2dfa;
        pddl2dfa.start();

        Domain domain(var_mgr_, domain_file_, init_file_);
        SymbolicStateDfa domain_sdfa = domain.to_symbolic();
        domain.print_domain();

        auto pddl2dfa_t = pddl2dfa.stop().count() / 1000.0;
        running_times_.push_back(pddl2dfa_t);
        std::cout << "Done [" << pddl2dfa_t << " s]" << std::endl;

        // 2. construct DFA of LTLf formula
        // i. read LTLf goal from file
        std::cout << "[syft4fond] Transforming LTLf goal into DFA..." << std::flush;
        Stopwatch ltlf2dfa;
        ltlf2dfa.start();

        std::ifstream ltlf_stream(goal_file_);
        std::string ltlf_goal;
        std::getline(ltlf_stream, ltlf_goal);

        // ii. parse LTLf goal
        ltlf_goal = parse_goal(domain, ltlf_goal);

        // iii. LTLf -> DFA
        ExplicitStateDfaMona goal_mona_dfa = ExplicitStateDfaMona::dfa_of_formula(ltlf_goal);
        // std::cout << "PRINT GOAL DFA" << std::endl;
        // goal_mona_dfa.dfa_print();
        ExplicitStateDfa goal_dfa = ExplicitStateDfa::from_dfa_mona(var_mgr_, goal_mona_dfa);
        SymbolicStateDfa goal_sdfa = SymbolicStateDfa::from_explicit(goal_dfa);

        auto ltlf2dfa_t = ltlf2dfa.stop().count() / 1000.0;
        running_times_.push_back(ltlf2dfa_t);
        std::cout << "Done [" <<  ltlf2dfa_t << " s]" << std::endl;

        // 3. solve game
        std::cout << "[syft4fond] Synthesisizing a Joker strategy..." << std::endl;
        Stopwatch synthesis;
        synthesis.start();

        std::vector<SymbolicStateDfa> game_sdfas = {domain_sdfa, goal_sdfa};
        SymbolicStateDfa dfa_game = SymbolicStateDfa::domain_compose(game_sdfas);
        CUDD::BDD invariant_bdd = domain.get_invariants_bdd();

        SynthesisResult joker_result;

        ReachabilitySynthesizer synthesizer(
            dfa_game,
            Player::Agent,
            Player::Agent,
            dfa_game.final_states(),
            invariant_bdd
        );

        SynthesisResult result = synthesizer.run();

        if(result.realizability){
            std::cout << "[syft4fond] The game is realizable without Joker moves" << std::endl;
            double t_jokerGame = synthesis.stop().count() / 1000.0;
            running_times_.push_back(t_jokerGame);
            std::cout << "DONE in " << t_jokerGame << " s" << std::endl;
            if (result.realizability)
                interactive(domain, dfa_game, result);
            return result;
        } else {
            std::cout << "[syft4fond] The game is NOT realizable without Joker moves" << std::endl;
        
            CUDD::BDD winning_region = result.winning_states;
            CUDD::BDD winning_moves = result.winning_moves;
            JokerReachabilitySynthesizer joker_synthesizer(dfa_game,
                                                            Player::Agent,
                                                            Player::Agent,
                                                            winning_region,
                                                            winning_moves,
                                                            invariant_bdd); 
            joker_result = joker_synthesizer.run();
            double t_jokerGame = synthesis.stop().count() / 1000.0;
            running_times_.push_back(t_jokerGame);
            std::cout << "DONE in " << t_jokerGame << " s" << std::endl;

            if (joker_result.realizability){
                std::cout << "[syft4fond] Joker moves counter: " << joker_result.cost << std::endl;
                interactive(domain, dfa_game, joker_result);
            } else {
                std::cout << "[syft4fond] The game is NOT Joker realizable" << std::endl;
            }

                
        }
        
        return joker_result;
    }

    std::string LTLfFONDJokerSynthesizer::parse_goal(const Domain& domain, std::string& goal) const {
        std::string parsed_goal = goal;

        // get maps from: action names to props; and var names to bdds
        std::unordered_map<std::string, std::string> action_names_to_props =
            domain.get_action_name_to_props();
        std::unordered_map<std::string, CUDD::BDD> var_name_to_bdd =
            var_mgr_->get_name_to_variable();

        // copy is needed because of mismatch between SPOT's and Lydia's syntax
        std::string copy = goal;
        boost::algorithm::replace_all(copy, "true", "tt");
    
        // parse formula with spot parser to get props
        // formula spot_intent = parse_formula(intent.c_str());
        formula spot_intent = parse_formula(copy.c_str());
        std::vector<std::string> props = get_props(spot_intent);

        // perform substituion
        for (auto& p : props) {
            if (p == "tt") continue;
            if (var_name_to_bdd.find(p) == var_name_to_bdd.end()) { // p is not a fluent
                auto it = action_names_to_props.find(p);
                if (it != action_names_to_props.end()) {
                    size_t pos = parsed_goal.find(p);
                    while (pos != std::string::npos) {
                        parsed_goal.replace(pos, p.size(), it->second);
                        pos = parsed_goal.find(p, pos + it->second.size());
                    } 
                }
                else throw std::runtime_error(p + " is neither a fluent nor an action name");
            }
        }    
        return parsed_goal;
    }

    // TODO (ELISA).
    // 1. Create LTLfFONDJokerSynthesizer (attenzione: Joker game must not reach the env. error)
    // See SymbolicStateDfa::domain_compose to retrieve env_error_bdd
    // 2. Create function interactive -- See https://github.com/GianmarcoDIAG/syft4fond/tree/tesi-antonio for further details
    // 3. See https://github.com/GianmarcoDIAG/syft4fond/tree/main
    // you can find triangle-tire world benchmarks
    // construct and test example


    void LTLfFONDJokerSynthesizer::interactive( 
        const Domain& domain,
        const SymbolicStateDfa& dfa_game,
        const SynthesisResult& joker_result
    ) const {
        // keep in mind the order of variables
        // i.e., (F, Act, React, Z)

        //domain.print_domain();
        //var_mgr_->print_mgr();
        
        std::vector<int> state = dfa_game.initial_state();
        std::vector<CUDD::BDD> transition_function = dfa_game.transition_function();
        CUDD::BDD final_states = dfa_game.final_states();
        int number_of_fluents = domain.get_vars().size() + 2;

        bool is_reaction_valid = false;
        std::cout << "##########################################" << std::endl;
        std::cout << "[INTERACTIVE] Planning domain interactive execution" << std::endl;

        // // Obtain the indexes for the agent error state and the environment error state
        std::vector<std::string> vars_ = domain.get_vars();
        std::size_t agent_error_index = vars_.size();
        std::size_t env_error_index = vars_.size() + 1;

        // // Obtain the agent error bdd and the environment error bdd from the transition function
        CUDD::BDD agent_error_bdd = dfa_game.transition_function()[agent_error_index];
        CUDD::BDD env_error_bdd = dfa_game.transition_function()[env_error_index];

        // std::cout << "[pddl2dfa] env_error: " << env_error_bdd << std::endl;
        // std::cout << "[pddl2dfa] env_error2: " << env_error_bdd2 << std::endl;
        // CUDD::BDD env_error_bdd = var_mgr_->name_to_variable("env_err");
        // CUDD::BDD agent_error_bdd = var_mgr_->name_to_variable("ag_err");
        

        CUDD::BDD winning_states =  joker_result.winning_states; //* !env_error_bdd;
        std::unordered_map<int, CUDD::BDD> output_function;
        
        
        while (true) {
          	std::vector<int> transition;
            std::vector<int> Z_fluents;
            std::vector<int> actions_bit;
            std::vector<int> reactions_bit;
            std::vector<int> goal_ltlf;
			std::vector<int> valid_actions;
			std::vector<int> valid_reactions;
			std::vector<int> legal_reactions;
            std::vector<int> state_eval;
            is_reaction_valid = false;

            std::cout << "[INTERACTIVE] State vector: ";
            for (const auto& v : state) std::cout << v;
            std::cout << std::endl;
            std::string string_state = "{";
            for (int i = 0; i < vars_.size(); ++i)
                if (state[i] == 1) string_state += vars_[i] + ", ";
            string_state = string_state.substr(0, string_state.size() - 2) + "}";
            std::cout << "[INTERACTIVE] State vars: " << string_state << std::endl;
            std::cout << "[INTERACTIVE] Final states: " << final_states << std::endl;

            state_eval.insert(state_eval.end(), state.begin(), state.begin() + number_of_fluents);
            
            for(int i = 0; i < var_mgr_->output_variable_count(); ++i) {
              state_eval.push_back(1);
            }
            for(int i = 0; i < var_mgr_->input_variable_count(); ++i) {
              state_eval.push_back(1);
            }
            state_eval.insert(state_eval.end(), state.begin() + number_of_fluents, state.end());

            if (winning_states.Eval(state_eval.data()).IsOne() && joker_result.cost == 0){
                output_function = joker_result.transducer.get()->get_output_function();
                std::cout << "[INTERACTIVE] Agent follows winning strategy." << std::endl; 
            } else if(winning_states.Eval(state_eval.data()).IsOne() && joker_result.cost > 0){
                output_function = joker_result.transducer.get()->get_output_function();
                std::cout << "[INTERACTIVE] Agent follows Joker strategy." << std::endl; 

            } else {
                std::cout << "[INTERACTIVE] Agent in losing region." << std::endl; 
                
                if (var_mgr_->state_variable(dfa_game.automaton_id(), vars_.size()).Eval(state_eval.data()).IsOne()){ //agent_error_bdd.Eval(state_eval.data()).IsOne()){//
                    std::cout << "- AGENT ERROR STATE -";
                    std::cout << "[INTERACTIVE] Termination" << std::endl;
                    return;
                }
                    
                if (var_mgr_->state_variable(dfa_game.automaton_id(), vars_.size()+1).Eval(state_eval.data()).IsOne()){ //env_error_bdd.Eval(state_eval.data()).IsOne()){//
                    std::cout << "- ENVIRONMENT ERROR STATE -";
                    std::cout << "[INTERACTIVE] Termination" << std::endl;
                    return;
                }
                return;
            }

            std::cout << "[INTERACTIVE] The current state is: ";
            if (var_mgr_->state_variable(dfa_game.automaton_id(), vars_.size()).Eval(state_eval.data()).IsOne()){ //agent_error_bdd.Eval(state_eval.data()).IsOne()){//
                std::cout << "- AGENT ERROR STATE -";
                std::cout << "Termination" << std::endl;
                return;
            }
                
            else if (var_mgr_->state_variable(dfa_game.automaton_id(), vars_.size()+1).Eval(state_eval.data()).IsOne()){ //env_error_bdd.Eval(state_eval.data()).IsOne()){//
                std::cout << "- ENVIRONMENT ERROR STATE -";
                std::cout << "Termination" << std::endl;
                return;
            } 
            else if (final_states.Eval(state_eval.data()).IsOne()){
                std::cout << "- FINAL -"<< std::endl;
                std::cout << "[INTERACTIVE] The goal has been reached. Termination" << std::endl;
                return;
            } 
            else std::cout << "- NOT FINAL -";
            std::cout << std::endl;

            Z_fluents.insert(Z_fluents.end(), state.begin(), state.begin() + number_of_fluents);

            goal_ltlf.insert(goal_ltlf.end(), state.begin() + number_of_fluents, state.end());

            transition.insert(transition.end(), Z_fluents.begin(), Z_fluents.end());



            ////////////// ACTION SELECTION
            std::unordered_map<int, std::string> id_to_var = var_mgr_->get_index_to_name(); 
            for (int i = 0; i < id_to_var.size(); i++) {
                std::string var = id_to_var[i];
                int agent_eval;
                if (var_mgr_->is_output_variable(var)) {
                    std::cout << "Variable: " << var;
                    std::cout << ". Agent output (0 = false, 1 = true): ";
                    agent_eval = output_function[i].Eval(state_eval.data()).IsOne();
                    std::cout << agent_eval << std::endl;
                    actions_bit.push_back(agent_eval);
                }
            }
            // std::cout << "Actions_bit:" << std::endl;
            // for (int e : actions_bit) std::cout << e;
            // std::cout << "" << std::endl;
            auto id_to_action_name = domain.get_id_to_action_name();
            int selected_act_id = -1;
            for (const auto& act_id : id_to_action_name) {

                std::vector<int> iter_action;
                iter_action = domain.to_bits(act_id.first, var_mgr_->output_variable_count());

                if(std::equal(iter_action.begin(), iter_action.end(), actions_bit.begin())) {
                    std::cout << "[INTERACTIVE] ID: " << act_id.first << " - Action: " << act_id.second << std::endl;
                    selected_act_id = act_id.first;
                    for (auto b : actions_bit) transition.push_back(b);
                    break;
                } else continue;
            	
  
            }
            
            if (selected_act_id == -1){
                std::cout << "ERROR IN THE COMPUTATION OF THE STRATEGY!!" << std::endl;
                return;
            }

            //////////////////////////////// REACTION  INSERTION
            auto id_to_reaction_name = domain.get_id_to_reaction_name();
            int react_id;
           	while(!is_reaction_valid) {
                valid_reactions.clear();
                std::cout << "[INTERACTIVE] Valid reactions:" << std::endl;
              	for(const auto& id_react : id_to_reaction_name) {
              		std::vector<int> check_reaction;
              		check_reaction.insert(check_reaction.end(), Z_fluents.begin(), Z_fluents.end());
                    check_reaction.insert(check_reaction.end(), actions_bit.begin(), actions_bit.end());
              		for (const auto& b : domain.to_bits(id_react.first, var_mgr_->input_variable_count())) 
                        check_reaction.push_back(b);

                    check_reaction.insert(check_reaction.end(), goal_ltlf.begin(), goal_ltlf.end());

              		if(!(env_error_bdd.Eval(check_reaction.data()).IsOne())) {
                        valid_reactions.push_back(id_react.first);
              			std::cout << "ID: " << id_react.first << " - Reaction: " << id_react.second << std::endl;
              		}
            	}
            	std::cout << "[INTERACTIVE] Insert ID of environment action: ";
            	std::cin >> react_id;
            	if(std::count(valid_reactions.begin(), valid_reactions.end(), react_id) > 0) {
              		is_reaction_valid = true;
              		for (const auto& b : domain.to_bits(react_id, var_mgr_->input_variable_count())) 
                        transition.push_back(b);
            	} else if (react_id == -1){
                    std::cout << "[INTERACTIVE] STOPPING INTERACTIVE STRATEGY." << std::endl;
                    return;
                } else {
                  	std::cout << "[INTERACTIVE] Chosen Rection is not valid. Choose again." << std::endl;
            	}
                std::cout << std::endl;
           	}
            
            //////////////////////// TRANSITION
            transition.insert(transition.end(), goal_ltlf.begin(), goal_ltlf.end());

            std::vector<int> new_state;
            for (int i = 0; i < transition_function.size(); ++i) {
                new_state.push_back(transition_function[i].Eval(transition.data()).IsOne());
            }
            state = new_state;
            std::cout << std::endl;
        }
        return;
    }
    
}