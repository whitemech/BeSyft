#include"LTLfFONDSynthesizer.h"

namespace Syft {
    LTLfFONDSynthesizer::LTLfFONDSynthesizer(
        std::shared_ptr<VarMgr> var_mgr,
        const std::string& domain_file,
        const std::string& init_file,
        const std::string& goal_file
    ) : var_mgr_(var_mgr), domain_file_(domain_file), init_file_(init_file), goal_file_(goal_file) {}

    SynthesisResult LTLfFONDSynthesizer::run() {
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
        ExplicitStateDfa goal_dfa = ExplicitStateDfa::from_dfa_mona(var_mgr_, goal_mona_dfa);
        SymbolicStateDfa goal_sdfa = SymbolicStateDfa::from_explicit(goal_dfa);

        auto ltlf2dfa_t = ltlf2dfa.stop().count() / 1000.0;
        running_times_.push_back(ltlf2dfa_t);
        std::cout << "Done [" <<  ltlf2dfa_t << " s]" << std::endl;

        // 3. solve game
        std::cout << "[syft4fond] Synthesisizing a strategy..." << std::flush;
        Stopwatch synthesis;
        synthesis.start();

        std::vector<SymbolicStateDfa> game_sdfas = {domain_sdfa, goal_sdfa};
        SymbolicStateDfa dfa_game = SymbolicStateDfa::domain_compose(game_sdfas);
        CUDD::BDD invariant_bdd = domain.get_invariants_bdd();

        ReachabilitySynthesizer synthesizer(
            dfa_game,
            Player::Agent,
            Player::Agent,
            dfa_game.final_states(),
            invariant_bdd
        );

        SynthesisResult result = synthesizer.run();
        auto synthesis_t = synthesis.stop().count() / 1000.0;
        running_times_.push_back(synthesis_t);
        std::cout << "Done [" << synthesis_t << " s]" <<  std::endl;

        return result;
    }

    std::string LTLfFONDSynthesizer::parse_goal(const Domain& domain, std::string& goal) const {
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
    // 2. Create function interactive
    // 3. See https://github.com/GianmarcoDIAG/syft4fond/tree/main
    // you can find triangle-tire world benchmarks
    // construct and test example
    // void LTLfFONDDomain::interactive( see https://github.com/GianmarcoDIAG/syft4fond/tree/tesi-antonio for further details
    //     const Domain& domain,
    //     const SymbolicStateDfa& product,
    //     const MaxSet& max_set
    // ) const {
    //     // keep in mind the order of variables
    //     // i.e., (F, Act, React, Z)
    //     // var_mgr_->print_varmgr();
    //     domain.print_domain();
    //     var_mgr_->print_varmgr();

    //     std::vector<int> state = product.initial_state();
    //     std::vector<CUDD::BDD> transition_function = product.transition_function();
    //     CUDD::BDD final_states = product.final_states();
    //     int number_of_fluents = domain.get_vars().size() + 2;

    //     bool is_action_valid = false;

    //     std::cout << "[pddl2dfa] Planning domain interactive debug" << std::endl;

    //     std::cout << "[pddl2dfa] Agent actions: " << std::endl;
    //     for (const auto& id_to_act : domain.get_id_to_action_name())
    //         std::cout << "ID: " << id_to_act.first << ". Action: " << id_to_act.second << std::endl;
    //     std::cout << std::endl;

    //     std::cout << "[pddl2dfa] Environment reactions: " << std::endl;
    //     for (const auto& id_to_react : domain.get_id_to_reaction_name())
    //         std::cout << "ID: " << id_to_react.first << ". Reaction: " << id_to_react.second << std::endl;
    //     std::cout << std::endl;

    //     // Obtain the indexes for the agent error state and the environment error state
    //     std::vector<std::string> vars_ = domain.get_vars();
    //     std::size_t agent_error_index = vars_.size();
    //     std::size_t env_error_index = vars_.size() + 1;

    //     // Obtain the agent error bdd and the environment error bdd from the transition function
    //     CUDD::BDD agent_error_bdd = product.transition_function()[agent_error_index];
    //     CUDD::BDD env_error_bdd = product.transition_function()[env_error_index];

    //     // Construct the strategy to not fall into the environment error bdd
    //     CUDD::BDD max_env_strategy = max_set.deferring_strategy * !env_error_bdd;

    //     while (true) {
    //       	std::vector<int> transition;
    //         std::vector<int> first_part;
    //         std::vector<int> actions_bit;
    //         std::vector<int> reactions_bit;
    //         std::vector<int> second_part;
	// 		std::vector<int> valid_actions;
	// 		std::vector<int> valid_reactions;
	// 		std::vector<int> legal_reactions;
    //         std::vector<int> state_prime;
    //         is_action_valid = false;

    //         std::cout << "[pddl2dfa] State vector: ";
    //         for (const auto& v : state) std::cout << v;
    //         std::cout << std::endl;
    //         std::string string_state = "{";
    //         for (int i = 0; i < vars_.size(); ++i)
    //             if (state[i] == 1) string_state += vars_[i] + ", ";
    //         string_state = string_state.substr(0, string_state.size() - 2) + "}";
    //         std::cout << "[pddl2dfa] State vars: " << string_state << std::endl;
    //         std::cout << "[pddl2dfa] Final states: " << final_states << std::endl;

    //         state_prime.insert(state_prime.end(), state.begin(), state.begin() + number_of_fluents);
    //         for(int i = 0; i < var_mgr_->output_variable_count(); ++i) {
    //           state_prime.push_back(1);
    //         }
    //         for(int i = 0; i < var_mgr_->input_variable_count(); ++i) {
    //           state_prime.push_back(1);
    //         }
    //         state_prime.insert(state_prime.end(), state.begin() + number_of_fluents, state.end());

    //         std::cout << "[pddl2dfa] The current state is: ";
    //         if (var_mgr_->state_variable(product.automaton_id(), vars_.size()).Eval(state_prime.data()).IsOne())
    //             std::cout << "- AGENT ERROR STATE -";
    //         if (var_mgr_->state_variable(product.automaton_id(), vars_.size()+1).Eval(state_prime.data()).IsOne())
    //             std::cout << "- ENVIRONMENT ERROR STATE -";
    //         if (final_states.Eval(state_prime.data()).IsOne()) std::cout << "- FINAL -";
    //         else std::cout << "- NOT FINAL -";
    //         std::cout << std::endl;

    //         first_part.insert(first_part.end(), state.begin(), state.begin() + number_of_fluents);
    //         second_part.insert(second_part.end(), state.begin() + number_of_fluents, state.end());
    //         transition.insert(transition.end(), first_part.begin(), first_part.end());

    //         auto id_to_action_name = domain.get_id_to_action_name();
    //         int act_id;

    //         // Show valid actions until the user chooses a valid one
    //        	while(!is_action_valid) {
    //             valid_actions.clear();
    //             std::cout << "[pddl2dfa] Valid actions:" << std::endl;
    //           	for(const auto& id_act : id_to_action_name) {
    //           		std::vector<int> check_action;
    //           		check_action.insert(check_action.end(), state.begin(), state.begin() + number_of_fluents);
    //           		for (const auto& b : to_bits(id_act.first, var_mgr_->output_variable_count())) check_action.push_back(b);

    //           		if(!agent_error_bdd.Eval(check_action.data()).IsOne()) {
    //                     valid_actions.push_back(id_act.first);
    //           			std::cout << "ID: " << id_act.first << " - Action: " << id_act.second << std::endl;
    //           		}
    //         	}
    //         	std::cout << "[pddl2dfa] Insert ID of agent action: ";
    //         	std::cin >> act_id;
    //         	if(std::count(valid_actions.begin(), valid_actions.end(), act_id) > 0) {
    //           		is_action_valid = true;
    //           		for (const auto& b : to_bits(act_id, var_mgr_->output_variable_count())) transition.push_back(b);
    //         	} else {
    //               	std::cout << "[pddl2dfa] Chosen Action is not valid." << std::endl;
    //         	}
    //             std::cout << std::endl;
    //        	}

    //         std::cout << "[pddl2dfa] Possible cooperative reactions:" << std::endl;

    //         // Check the validity for each possible reaction related to the chosen action
    //         for (const auto& id_to_react : domain.get_id_to_reaction_name()) {
    //           	std::vector<int> eval;

    //             eval.insert(eval.end(), first_part.begin(), first_part.end());

    //             actions_bit = to_bits(act_id, var_mgr_->output_variable_count());
    //             eval.insert(eval.end(), actions_bit.begin(), actions_bit.end());

    //             reactions_bit = to_bits(id_to_react.first, var_mgr_->input_variable_count());
    //             eval.insert(eval.end(), reactions_bit.begin(), reactions_bit.end());

    //             eval.insert(eval.end(), second_part.begin(), second_part.end());

    //             if(max_env_strategy.Eval(eval.data()).IsOne()) {
    //               std::cout << "\t - Reaction: " << id_to_react.first << ": " << domain.get_id_to_reaction_name()[id_to_react.first] << " - VALID" << std::endl;
    //               valid_reactions.push_back(id_to_react.first);
    //             } else {
    //               std::cout << "\t - Reaction: " << id_to_react.first << ": " << domain.get_id_to_reaction_name()[id_to_react.first] << " - NOT VALID" << std::endl;
    //             }
    //         }

    //         // Extract one of valid reaction with a PRNG
    //         if (!valid_reactions.empty()) {
    //             std::random_device rd;  // Seed for randomness
    //             std::mt19937 gen(rd()); // Mersenne Twister PRNG
    //             std::uniform_int_distribution<size_t> dist(0, valid_reactions.size() - 1);

    //             size_t randomIndex = dist(gen);
    //             auto& randomElement = valid_reactions[randomIndex];
	// 			std::cout << std::endl;
	// 			std::cout << "[pddl2dfa] Chosen Reaction is: " << randomElement << ": " << domain.get_id_to_reaction_name()[randomElement] << std::endl;
    //             for (const auto& b : to_bits(randomElement, var_mgr_->input_variable_count())) transition.push_back(b);
    //         } else {
    //           	std::cout << std::endl;
    //           	std::cout << "[pddl2dfa] No cooperative reaction found" << std::endl;
    //           	std::cout << "[pddl2dfa] Possible legal reactions:" << std::endl;
    //           	for (const auto& id_to_react : domain.get_id_to_reaction_name()) {
    //           		std::vector<int> eval;

    //             	eval.insert(eval.end(), first_part.begin(), first_part.end());

    //             	actions_bit = to_bits(act_id, var_mgr_->output_variable_count());
    //             	eval.insert(eval.end(), actions_bit.begin(), actions_bit.end());

    //             	reactions_bit = to_bits(id_to_react.first, var_mgr_->input_variable_count());
    //             	eval.insert(eval.end(), reactions_bit.begin(), reactions_bit.end());

    //             	eval.insert(eval.end(), second_part.begin(), second_part.end());

    //             	if(!env_error_bdd.Eval(eval.data()).IsOne()) {
    //               		std::cout << "\t - Reaction: " << id_to_react.first << ": " << domain.get_id_to_reaction_name()[id_to_react.first] << " - LEGAL" << std::endl;
    //               		legal_reactions.push_back(id_to_react.first);
    //             	} else {
    //               		std::cout << "\t - Reaction: " << id_to_react.first << ": " << domain.get_id_to_reaction_name()[id_to_react.first] << " - NOT LEGAL" << std::endl;
    //             	}
    //         	}

    //             std::random_device rd;  // Seed for randomness
    //             std::mt19937 gen(rd()); // Mersenne Twister PRNG
    //             std::uniform_int_distribution<size_t> dist(0, legal_reactions.size() - 1);

    //             size_t randomIndex = dist(gen);
    //             auto& randomElement = legal_reactions[randomIndex];
	// 			std::cout << std::endl;
	// 			std::cout << "[pddl2dfa] Chosen Reaction is: " << randomElement << ": " << domain.get_id_to_reaction_name()[randomElement] << std::endl;
    //             for (const auto& b : to_bits(randomElement, var_mgr_->input_variable_count())) transition.push_back(b);
    //         }

    //         transition.insert(transition.end(), second_part.begin(), second_part.end());

    //         std::vector<int> new_state;
    //         for (int i = 0; i < transition_function.size(); ++i) {
    //             new_state.push_back(transition_function[i].Eval(transition.data()).IsOne());
    //         }
    //         state = new_state;
    //         std::cout << std::endl;
    //     }
    //     return;
    
}