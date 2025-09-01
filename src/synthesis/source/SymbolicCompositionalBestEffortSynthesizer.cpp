/*
* This file defines the class SymbolicCompositionalBestEffortSynthesizer
* which implements the symbolic-compositional approach to best-effort synthesis
*/

#include "SymbolicCompositionalBestEffortSynthesizer.h"
#include <boost/algorithm/string.hpp>
#include <queue>

namespace Syft
{

    SymbolicCompositionalBestEffortSynthesizer::SymbolicCompositionalBestEffortSynthesizer(std::shared_ptr<VarMgr> var_mgr,
                                                 std::string agent_specification,
                                                 std::string environment_specification,
                                                 InputOutputPartition partition,
                                                 Player starting_player)    :   var_mgr_(var_mgr),
                                                                                agent_specification_(agent_specification),
                                                                                environment_specification_(environment_specification),
                                                                                partition_(partition),
                                                                                starting_player_(starting_player)
    {
        // 1. Step 1. Construct symbolic DFAs formulas {E -> phi, !E, E && phi}
        // Build MONA DFAs for agent and environment specifications
        Syft::Stopwatch ltlf2dfa;
        ltlf2dfa.start();

        ExplicitStateDfaMona agent_spec_dfa =
            ExplicitStateDfaMona::dfa_of_formula(agent_specification); // DFA A_{phi}
        ExplicitStateDfaMona environment_spec_dfa =
            ExplicitStateDfaMona::dfa_of_formula(environment_specification); // DFA A_{E}
        ExplicitStateDfaMona tautology_dfa =
            ExplicitStateDfaMona::dfa_of_formula("true"); // DFA A_{tt}. Accepts non-empty traces only

        // DFA A_{phi}
        std::cout << std::endl;
        std::cout << "Agent goal DFA\n";
        agent_spec_dfa.dfa_print();
        std::cout << std::endl;

        // DFA A_{E}
        std::cout << std::endl;
        std::cout << "Environment Specification DFA\n";
        environment_spec_dfa.dfa_print();
        std::cout << std::endl;

        // tautoloty DFA
        std::cout << std::endl;
        std::cout << "Tautology DFA\n";
        tautology_dfa.dfa_print();
        std::cout << std::endl;

        double t_ltlf2dfa = ltlf2dfa.stop().count() / 1000.0;
        running_times_.push_back(t_ltlf2dfa);
        std::cout << "[BeSyft] MONA DFA construction DONE in: " << t_ltlf2dfa << " s" << std::endl;

        // Obtain parsed formulas (requirement to construct symbolic DFAs)
        Syft::Stopwatch dfa2sym;
        dfa2sym.start();

        std::string adversarial_formula = 
            "(" + environment_specification + ") -> (" + agent_specification +")"; 

        formula parsed_adversarial_formula = 
            parse_formula(adversarial_formula.c_str()); // parses (E -> phi)

        // Extract propositions from formula and partition
        var_mgr_->create_named_variables(get_props(parsed_adversarial_formula)); // (E -> phi) includes all problem variables
        var_mgr_->partition_variables(partition_.input_variables,
                                        partition_.output_variables);

        // Get explicit state DFA from MONA DFA
        ExplicitStateDfa explicit_agent_dfa =
            ExplicitStateDfa::from_dfa_mona(var_mgr_, agent_spec_dfa);
        ExplicitStateDfa explicit_env_dfa =
            ExplicitStateDfa::from_dfa_mona(var_mgr_, environment_spec_dfa); 
        ExplicitStateDfa explicit_tau_dfa =
            ExplicitStateDfa::from_dfa_mona(var_mgr_, tautology_dfa);

        // Get Symbolic State DFA from Explicit DFA
        symbolic_dfas_.push_back(SymbolicStateDfa::from_explicit(std::move(explicit_agent_dfa)));
        symbolic_dfas_.push_back(SymbolicStateDfa::from_explicit(std::move(explicit_env_dfa)));
        symbolic_dfas_.push_back(SymbolicStateDfa::from_explicit(std::move(explicit_tau_dfa)));
        
        // f_{phi} is stored in symbolic_dfas_[0].final_states()
        // f_{E} is stored in symbolic_dfas_[1].final_states()


        // Step 2. Construct symbolic arena for best-effort synthesis through product
        SymbolicStateDfa arena = 
            SymbolicStateDfa::product(symbolic_dfas_);
        arena_.push_back(arena);

        double t_dfa2sym = dfa2sym.stop().count() / 1000.0;
        running_times_.push_back(t_dfa2sym);
        std::cout << "[BeSyft] Symbolic DFA construction DONE in " << t_dfa2sym << " s" << std::endl;
    }

    BestEffortSynthesisResult SymbolicCompositionalBestEffortSynthesizer::run() {

        BestEffortSynthesisResult best_effort_result;

        CUDD::BDD adv_goal = ((!symbolic_dfas_[1].final_states()) + symbolic_dfas_[0].final_states()) * (!arena_[0].initial_state_bdd()); // f_{E} -> f_{Phi}
        // CUDD::BDD adv_goal = (!(symbolic_dfas_[1].final_states() * (!symbolic_dfas_[0].final_states()))) * (!arena_[0].initial_state_bdd());
        CUDD::BDD neg_goal = ((!symbolic_dfas_[1].final_states()) * (!arena_[0].initial_state_bdd())); // ! f_{E}
        CUDD::BDD coop_goal = (symbolic_dfas_[1].final_states()) * (symbolic_dfas_[0].final_states()) * (!arena_[0].initial_state_bdd()); // F{E} /\ f_{Phi}

        // Step 3. Compute a winning strategy in the adversarial game
        Stopwatch advGame;
        advGame.start();
        std::cout << "[BeSyft] Constructing and solving adversarial game...";
        ReachabilitySynthesizer adv_synthesizer(arena_[0],
                                                starting_player_,
                                                Player::Agent,
                                                adv_goal, // Lifting
                                                var_mgr_->cudd_mgr()->bddOne());
        best_effort_result.adversarial = adv_synthesizer.run();
        double t_advGame = advGame.stop().count() / 1000.0;
        running_times_.push_back(t_advGame);
        std::cout << "DONE in " << t_advGame << " s" << std::endl;

        // Step 4. Compute environment's winning region in negation of environment game
        Stopwatch coopGame;
        coopGame.start();
        std::cout << "[BeSyft] Constructing and solving cooperative game...";
        ReachabilitySynthesizer neg_env_synthesizer(arena_[0],
                                                    starting_player_,
                                                    Player::Agent,  // gets env winning region from agent's
                                                    neg_goal, // Lifting
                                                    var_mgr_->cudd_mgr()->bddOne());
        SynthesisResult env_result = neg_env_synthesizer.run();
        CUDD::BDD non_environment_winning_region = env_result.winning_states;

        // Step 5. Restrict arena to environemt winning region.
        // i.e. all states that are in non_environment_winning_region have to be pruned as invalid
        arena_.push_back(arena_[0].restriction(non_environment_winning_region));

        // Step 6. Compute a cooperatively winning strategy in restricted game
        CoOperativeReachabilitySynthesizer coop_synthesizer(arena_[1],
                                                            starting_player_,
                                                            Player::Agent,
                                                            coop_goal, // Lifting
                                                            var_mgr_->cudd_mgr()->bddOne()); 
        best_effort_result.cooperative = coop_synthesizer.run();
        double t_coopGame = coopGame.stop().count() / 1000.0;
        running_times_.push_back(t_coopGame);
        std::cout << "DONE in " << t_coopGame << " s" << std::endl; 

        return best_effort_result;
    }

    void SymbolicCompositionalBestEffortSynthesizer::merge_and_dump_dot(const SynthesisResult& adversarial_result, const SynthesisResult& cooperative_result, const string& filename) const {
        
        Syft::Stopwatch merge;
        merge.start();
        std::cout << "[BeSyft] Merging strategies...";

        std::vector<std::string> output_labels = var_mgr_->output_variable_labels(); // i.e. Y variables

        std::size_t output_count = cooperative_result.transducer.get()->output_function_.size();
        std::vector<CUDD::ADD> output_vector(output_count);

        // Cooperatively only winning states, i.e. states in cooperatively, but not reactively, winning region
        CUDD::BDD cooperative_only_winning_states = (!adversarial_result.winning_states) * cooperative_result.winning_states;
        for(std::size_t i=0; i < output_count; ++i) {
            std::string label = output_labels[i];
            int index = var_mgr_->name_to_variable(label).NodeReadIndex();
            // i. For winning states use adversarial output function
            CUDD::BDD restricted_adversarial_bdd = 
                adversarial_result.transducer.get()->output_function_.at(index) * adversarial_result.winning_states; 
            // ii. For cooperatively only winning states use cooperative output function
            CUDD::BDD restricted_cooperative_bdd = 
                cooperative_result.transducer.get()->output_function_.at(index) * cooperative_only_winning_states; 
            /// iii. For any state keep best-effort output
            CUDD::BDD merged_bdd = restricted_adversarial_bdd + restricted_cooperative_bdd;
            output_vector[i] = merged_bdd.Add();
        }
        var_mgr_->dump_dot(output_vector, output_labels, filename);

        double t_merge = merge.stop().count() / 1000.0;
        std::cout << "DONE in " <<  t_merge << " s" << std::endl;
    }

    std::vector<double> SymbolicCompositionalBestEffortSynthesizer::get_running_times() const {
        return running_times_;
    }



    void SymbolicCompositionalBestEffortSynthesizer::interactive(
        const BestEffortSynthesisResult& best_effort_result
    ) const {
        std::cout << "[BeSyft][interactive] Interactive strategy execution" << std::endl;

        // var_mgr_->print_varmgr();

        // initial state. Order of variables is: X \/ Y, Z_{phi}, Z_{E}, Z_{tau}
        std::vector<int> vars_init(var_mgr_->get_index_to_name().size(), 0);
        std::vector<int> goal_init = symbolic_dfas_[0].initial_state();
        std::vector<int> env_init = symbolic_dfas_[1].initial_state();
        std::vector<int> tau_init = symbolic_dfas_[2].initial_state();

        std::vector<int> state;
        state.insert(state.end(), vars_init.begin(), vars_init.end());
        state.insert(state.end(), goal_init.begin(), goal_init.end());
        state.insert(state.end(), env_init.begin(), env_init.end());
        state.insert(state.end(), tau_init.begin(), tau_init.end());

        CUDD::BDD winning_region = best_effort_result.adversarial.winning_states;
        CUDD::BDD cooperative_region = best_effort_result.cooperative.winning_states;
        std::unordered_map<int, CUDD::BDD> output_function;
        std::unordered_map<int, CUDD::BDD> alternative_output_function;

        std::unordered_map<int, std::string> id_to_var = var_mgr_->get_index_to_name(); 

        bool running = true;
        while (running) {

            std::cout << "[BeSyft][interactive] Current state: ";
            for (const auto&b : state) std::cout << b;
            // std::cout << " Size. " << state.size() << std::endl;
            std::cout << std::endl;
        
            // gets output function and alternative output function if a state is a witness
            bool state_is_witness = false;
            if (winning_region.Eval(state.data()).IsOne()) {
                std::cout << "[BeSyft][interactive] Agent in winning region uses winning strategy" << std::endl;
                output_function = best_effort_result.adversarial.transducer.get()->get_output_function();
            } else if (cooperative_region.Eval(state.data()).IsOne()) {
                std::cout << "[BeSyft][interactive] Agent in cooperative region uses cooperative strategy" << std::endl;
                output_function = best_effort_result.cooperative.transducer.get()->get_output_function();
                // if (dominance_check_ & !best_effort_result.dominant) {
                //     if (witness_region.Eval(state.data()).IsOne()) {
                //         state_is_witness = true;
                //         std::cout << "[BeSyft][interactive] State witnesses that no dominant strategy exists" << std::endl;
                //         alternative_output_function = best_effort_result.dominance.witness_transducer.get()->get_output_function();
                //         // std::cout << "alternative output function selected..." << std::endl;
                //     }                                                        
                // }
            } else {
                std::cout << "[BeSyft][interactive] Agent in losing region. Termination" << std::endl;
                return;
            }

            std::vector<int> transition = state;
        
            if (starting_player_ == Player::Agent) {

                // agent turn. Agent moves first
                std::cout << "[BeSyft][interactive] Agent move: " << std::endl;
                for (int i = 0; i < id_to_var.size(); ++i) {
                    std::string var = id_to_var[i];
                    int agent_eval;
                    if (var_mgr_->is_output_variable(var)) {
                        std::cout << "Variable: " << var;
                        std::cout << ". Agent output (0 = false, 1 = true): ";
                        agent_eval = output_function[i].Eval(state.data()).IsOne();
                        std::cout << agent_eval << std::endl;
                        transition[i] = agent_eval;
                    }
                }

                // shows witness if current state has one
                if (state_is_witness) {
                    std::cout << "[BeSyft][interactive] Alternative agent move (witness no dominant strategy exist): " << std::endl;
                    for (int i = 0; i < id_to_var.size(); ++i) {
                        std::string var = id_to_var[i];
                        int agent_eval;
                        if (var_mgr_->is_output_variable(var)) {
                            std::cout << "Variable: " << var;
                            std::cout << ". Agent output (0 = false, 1 = true): ";
                            agent_eval = alternative_output_function[i].Eval(state.data()).IsOne();
                            std::cout << agent_eval << std::endl;
                            // transition[i] = agent_eval; // do not update transitions
                        }
                    }
                } 

                // environment turn
                std::cout << "[BeSyft][interactive] Environment move (type 1 if var is true, else 0): " << std::endl;
                for (int i = 0; i < id_to_var.size(); ++i) {
                    std::string var = id_to_var[i];
                    int env_eval;
                    if (var_mgr_->is_input_variable(var)) {
                        std::cout << "Variable: " << var;
                        std::cout << ". Env Input (0=false, 1=true): ";
                        std::cin >> env_eval;
                        transition[i] = env_eval;
                    } 
                }

            } else if (starting_player_ == Player::Environment) {

                // environment turn. Environment moves first
                std::cout << "[BeSyft][interactive] Environment move (type 1 if var is true, else 0): " << std::endl;
                for (int i = 0; i < id_to_var.size(); ++i) {
                    std::string var = id_to_var[i];
                    int env_eval;
                    if (var_mgr_->is_input_variable(var)) {
                        std::cout << "Variable: " << var;
                        std::cout << ". Env Input (0=false, 1=true): ";
                        std::cin >> env_eval;
                        state[i] = env_eval; // if environment moves first, state must be updated
                        transition[i] = env_eval;
                    } 
                }

                // agent turn
                std::cout << "[BeSyft][interactive] Agent move: " << std::endl;
                for (int i = 0; i < id_to_var.size(); ++i) {
                    std::string var = id_to_var[i];
                    int agent_eval;
                    if (var_mgr_->is_output_variable(var)) {
                        std::cout << "Variable: " << var;
                        std::cout << ". Agent output (0 = false, 1 = true): ";
                        agent_eval = output_function[i].Eval(state.data()).IsOne();
                        std::cout << agent_eval << std::endl;
                        transition[i] = agent_eval;
                    }
                }

                // shows witness if current state has one
                if (state_is_witness) {
                    std::cout << "[BeSyft][interactive] Alternative agent move (witness no dominant strategy exist): " << std::endl;
                    for (int i = 0; i < id_to_var.size(); ++i) {
                        std::string var = id_to_var[i];
                        int agent_eval;
                        if (var_mgr_->is_output_variable(var)) {
                            std::cout << "Variable: " << var;
                            std::cout << ". Agent output (0 = false, 1 = true): ";
                            agent_eval = alternative_output_function[i].Eval(state.data()).IsOne();
                            std::cout << agent_eval << std::endl;
                            // transition[i] = agent_eval; // do not update transitions
                        }
                    } 
                }
            }

            std::cout << "[BeSyft][interactive] Input to transitions: ";
            for (const auto&b : transition) std::cout << b;
            // std::cout << " Size. "<< transition.size() << std::endl;
            std::cout << std::endl;
            // successor state
            int curr_state_var = id_to_var.size();
            std::vector<int> new_state = state;
            for (int i = 0; i < symbolic_dfas_[0].transition_function().size(); ++i) {
                new_state[curr_state_var] = symbolic_dfas_[0].transition_function()[i].Eval(transition.data()).IsOne();
                ++curr_state_var;
            }
            for (int i = 0; i < symbolic_dfas_[1].transition_function().size(); ++i) {
                new_state[curr_state_var] = symbolic_dfas_[1].transition_function()[i].Eval(transition.data()).IsOne();
                ++curr_state_var;
            }
            for (int i = 0; i < symbolic_dfas_[2].transition_function().size(); ++i) {
                new_state[curr_state_var] = symbolic_dfas_[2].transition_function()[i].Eval(transition.data()).IsOne();
                ++curr_state_var;
            }
            std::cout << "[BeSyft][interactive] Successor state: ";
            for (const auto& b: new_state) std::cout << b;
            std::cout << std::endl;

            // update state
            state = new_state;

            // evaluate whether we can stop the loop
            if (symbolic_dfas_[0].final_states().Eval(state.data()).IsOne()) {
                std::cout << "[BeSyft][interactive] The goal has been reached. Termination" << std::endl;
                running = false;
            }
            if (!(symbolic_dfas_[1].final_states().Eval(state.data()).IsOne())) {
                std::cout << "[BeSyft][interactive] The environment has been negated. Termination" << std::endl;
                running = false; 
            }
        }
    }
}
