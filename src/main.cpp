#include<sys/stat.h>
#include<cstring>
#include<iostream>
#include<istream>
#include<memory>
#include<CLI/CLI.hpp>
#include"Stopwatch.h"
#include"ExplicitStateDfaMona.h"
#include"SymbolicCompositionalBestEffortSynthesizer.h"
#include"MonolithicBestEffortSynthesizer.h"
#include"ExplicitCompositionalBestEffortSynthesizer.h"
#include"AdversarialSynthesizer.h"
#include"spotparser.h"
using namespace std;

// Function: sumVec
/**
 * @brief Compute the sum of all elements in a vector
 * 
 * @param[in] v - the input vector  
 * @return Sum of all elements in vector as a double
 */
double sumVec(const std::vector<double>& v) 
{
    double sum = 0;
    for (const auto& d: v) sum += d;
    return sum;
}

int main(int argc, char** argv) {

    CLI::App app {
        "BeSyft: a tool for Reactive and Best-Effort Synthesis with LTLf Goals and Assumptions"
    };

    string agent_file, environment_file, partition_filename, outfile="";
    int starting_flag, alg_id;

    bool print_dot = false;
    app.add_flag("-d,--print-dot", print_dot, "Print the output function(s)");
   
    CLI::Option* agent_formula_opt = 
        app.add_option("-a,--agent-file", agent_file, "File to agent specification")->
            required() -> check(CLI::ExistingFile);

    CLI::Option* environment_formula_file =
        app.add_option("-e,--environment-file", environment_file, "File to environment assumption")->
            required() -> check(CLI::ExistingFile);

    CLI::Option* partition_file =
        app.add_option("-p,--partition-file", partition_filename, "File to partition" )->
            required () -> check(CLI::ExistingFile);
    
    CLI::Option* starting_opt =
        app.add_option("-s,--starting-player", starting_flag, "Starting player:\nagent=1;\nenvironment=0.")->
            required();
    
    CLI::Option* alg_id_opt =
        app.add_option("-t,--algorithm", alg_id, "Specifies algorithm to use:\nMonolithic Best-Effort Synthesis=1;\nExplicit-Compositional Best-Effort Synthesis=2;\nSymbolic-Compositional Best-Effort Synthesis=3;\nAdversarial Reactive Synthesis=4") -> required();

    CLI::Option* outfile_opt =
        app.add_option("-f,--save-results", outfile, "If specified, save results in the passed file. Stores:\nAlgorithm;\nGoal file;\nEnvironment file;\nStarting player;\nLTLf2DFA (s);\nDFA2Sym (s);\nAdv Game (s);\nCoop Game (s); \t#best-effort synthesis algorithms only\nRun time(s);\nRealizability");

    CLI11_PARSE(app, argc, argv);

    string agent_specification;
    ifstream agent_spec_stream(agent_file);
    getline(agent_spec_stream, agent_specification);
    cout << "[BeSyft] Agent specification: " << agent_specification << endl;

    string environment_assumption;
    ifstream env_assumption_stream(environment_file);
    getline(env_assumption_stream, environment_assumption);
    cout << "[BeSyft] Environment assumption: " << environment_assumption << endl;

    Syft::InputOutputPartition partition =
        Syft::InputOutputPartition::read_from_file(partition_filename);

    Syft::Player starting_player;
    if (starting_flag == 1) {
        starting_player = Syft::Player::Agent;
    } else {
        starting_player = Syft::Player::Environment;
    }

    std::shared_ptr<Syft::VarMgr> v_mgr = std::make_shared<Syft::VarMgr>();

    cout << "[BeSyft] Ready to start best-effort synthesis" << endl;

    if (alg_id == 1) {
        Syft::MonolithicBestEffortSynthesizer best_effort_synthesizer(v_mgr, agent_specification, environment_assumption, partition, starting_player);
        auto result = best_effort_synthesizer.run();
        auto run_times = best_effort_synthesizer.get_running_times();
        std::cout << "[BeSyft] Running time: " << sumVec(run_times) << " s" << std::endl;
        if (result.first.realizability) {
            std::cout << "[BeSyft] Adversarially realizable. Computed winning strategy" << std::endl;
            if (print_dot) {std::cout << "[BeSyft] Printing output function" << std::endl; result.first.transducer.get() -> dump_dot("adv_outfunct.dot");}
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Monolithic Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Adv" << std::endl;
                }
            }
        else if (result.second.realizability) {
            std::cout << "[BeSyft] Cooperatively realizable. Computed best-effort strategy" << std::endl;
            if (print_dot) {std::cout << "[BeSyft] Printing output functions" << std::endl; result.first.transducer.get() -> dump_dot("adv_outfunct.dot"); result.second.transducer.get() -> dump_dot("coop_outfunct");}
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Monolithic Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Coop" << std::endl;
                }
            } 
        else if (!result.first.realizability && !result.second.realizability) { 
            std::cout << "[BeSyft] Unrealizable. Computed best-effort strategy" << std::endl;
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Monolithic Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Unr" << std::endl;
            }
        }
    } 
    else if (alg_id == 2) {
        Syft::ExplicitCompositionalBestEffortSynthesizer best_effort_synthesizer(v_mgr, agent_specification, environment_assumption, partition, starting_player);
        auto result = best_effort_synthesizer.run();
        auto run_times = best_effort_synthesizer.get_running_times();
        std::cout << "[BeSyft] Running time: " << sumVec(run_times) << " s" << std::endl;
        if (result.first.realizability) {
            std::cout << "[BeSyft] Adversarially realizable. Computed winning strategy" << std::endl;
            if (print_dot) {std::cout << "[BeSyft] Printing output function" << std::endl; result.first.transducer.get() -> dump_dot("adv_outfunct.dot");}
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Explicit-Compositional Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Adv" << std::endl;
                }
        } else if (result.second.realizability) {
            std::cout << "[BeSyft] Cooperatively realizable. Computed best-effort strategy" << std::endl;
            if (print_dot) {std::cout << "[BeSyft] Printing output functions" << std::endl; result.first.transducer.get() -> dump_dot("adv_outfunct.dot"); result.second.transducer.get() -> dump_dot("coop_outfunct");}
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Explicit-Compositional Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Coop" << std::endl;
                }
            }
        else if (!result.first.realizability && !result.second.realizability) { 
            std::cout << "[BeSyft] Unrealizable." << std::endl;
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Explicit-Compositional Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Unr" << std::endl;
                }
            }    
        } 
    else if (alg_id == 3) {
        Syft::SymbolicCompositionalBestEffortSynthesizer best_effort_synthesizer(v_mgr, agent_specification, environment_assumption, partition, starting_player);
        auto result = best_effort_synthesizer.run();
        auto run_times = best_effort_synthesizer.get_running_times();
        std::cout << "[BeSyft] Running time: " << sumVec(run_times) << " s" << std::endl;
        if (result.first.realizability) {
            std::cout << "[BeSyft] Adversarially realizable. Computed winning strategy" << std::endl;
            if (print_dot) {std::cout << "[BeSyft] Printing output function" << std::endl; result.first.transducer.get() -> dump_dot("adv_outfunct.dot");}
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Symbolic-Compositional Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Adv" << std::endl;
                }
        } else if (result.second.realizability) {
            std::cout << "[BeSyft] Cooperatively realizable. Computed best-effort strategy" << std::endl;
            if (print_dot) {std::cout << "[BeSyft] Printing output functions" << std::endl; result.first.transducer.get() -> dump_dot("adv_outfunct.dot"); result.second.transducer.get() -> dump_dot("coop_outfunct");}
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Symbolic-Compositional Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Coop" << std::endl;
                }
            }
        else if (!result.first.realizability && !result.second.realizability) { 
            std::cout << "[BeSyft] Unrealizable." << std::endl;
            if (outfile != "") {
                    std::ofstream outstream(outfile, std::ifstream::app);
                    outstream << "Symbolic-Compositional Best-Effort Synthesizer," << agent_file << "," << environment_file << ",";
                    if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                    outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << "," << run_times[3] << "," << sumVec(run_times) << ",Unr" << std::endl;
                }
        }
    }
    else if (alg_id == 4) {
        Syft::AdversarialSynthesizer adv_synth(v_mgr, agent_specification, environment_assumption, partition, starting_player);
        auto result = adv_synth.run();
        auto run_times = adv_synth.get_running_times();
        std::cout << "[BeSyft] Running time: " << sumVec(run_times) << " s" << std::endl;
        if (result.realizability) {
            std::cout << "[BeSyft] Adversarially realizable. Computed winning strategy" << std::endl;
            if (print_dot) {std::cout << "[BeSyft] Printing output function" << std::endl; result.transducer.get() -> dump_dot("adv_outfunct.dot");}
            if (outfile != "") {
                std::ofstream outstream(outfile, std::ifstream::app);
                outstream << "Adversarial Synthesizer," << agent_file << "," << environment_file << ",";
                if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << ",NA," << sumVec(run_times) << ",Adv" << std::endl;
            }
        } else {
            std::cout << "[BeSyft] Not adversarially realizable." << std::endl;
            if (outfile != "") {
                std::ofstream outstream(outfile, std::ifstream::app);
                outstream << "Adversarial Synthesizer," << agent_file << "," << environment_file << ",";
                if (starting_flag) outstream << "Agent,"; else outstream << "Environment,";
                outstream << run_times[0] << "," << run_times[1] << "," << run_times[2] << ",NA,"  << sumVec(run_times) << ",Unr" << std::endl;
            }
        }        
    } 
    else {
        std::cerr << "[BeSyft] Non-existing algorithm. Terminating" << std::endl;
        return 1;
    }

    return 0;

}
