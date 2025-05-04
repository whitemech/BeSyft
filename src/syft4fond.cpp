#include<sys/stat.h>
#include<cstring>
#include<iostream>
#include<istream>
#include<memory>
#include<CLI/CLI.hpp>
#include"VarMgr.h"
#include"LTLfFONDSynthesizer.h"
#include"LTLfFONDJokerSynthesizer.h"
#include"LTLfFONDBestEffortSynthesizer.h"

using namespace std;

double sumVec(const std::vector<double>& v) 
{
    double sum = 0;
    for (const auto& d: v) sum += d;
    return sum;
}

int main(int argc, char** argv) {

    CLI::App app {
        "syft4fond-ltlf: a tool for LTLf reactive synthesis in FOND planning domains"
    };

    string domain_file, problem_file, goal_file, out_file;
    bool interactive = false;
    int synthesis_id;

    CLI::Option* domain_file_opt =
        app.add_option("-d,--domain-file", domain_file, "Path to PDDL domain file") ->
        required() -> check(CLI::ExistingFile);

    CLI::Option* problem_file_opt =
        app.add_option("-p,--problem-file", problem_file, "Path to PDDL problem file") ->
        required() -> check(CLI::ExistingFile);

    CLI::Option* goal_file_opt =
        app.add_option("-g,--goal-file", goal_file, "Path to LTLf goal file") ->
        required() -> check(CLI::ExistingFile);

     CLI::Option* interactive_opt =
         app.add_option("-i,--interactive", interactive, "Executes the synthesized strategy in interactive mode");

    CLI::Option* out_file_opt =
        app.add_option("-o,--out-file", out_file, "Path to output .csv file. Stores:\n1. PDDL domain file\n2. PDDL problem file\n3. PDDL parsing (secs)\n4. PDDL2DFA (secs)\n5. Synthesis (secs)\n6. Run time (secs)\n7. Realizability (0,1)");

    CLI::Option* synthesis_id_opt =
        app.add_option("-s,--synthesizer", synthesis_id, "Specifies type of synthesis to use:\nBest-Effort Synthesis=1;\nJoker Synthesis=2") -> required();

    CLI11_PARSE(app, argc, argv);

    std::shared_ptr<Syft::VarMgr> var_mgr = std::make_shared<Syft::VarMgr>();

    if (synthesis_id == 1){
        Syft::LTLfFONDBestEffortSynthesizer synthesizer(
            var_mgr,
            domain_file, 
            problem_file,
            goal_file); 
    
        std::pair<Syft::SynthesisResult, Syft::SynthesisResult> result = synthesizer.run(interactive);
    
        auto running_times = synthesizer.get_running_times();
        auto run_time = sumVec(running_times);
    
        if (out_file != "") {
            if (!(std::filesystem::exists(out_file))) {
                std::ofstream outstream(out_file);
                outstream << "PDDL domain,PDDL problem,LTLf goal,PDDL2DFA (s),LTLf2DFA (s),Synthesis (s),Runtime (s)"<<std::endl;
                outstream << domain_file << "," << problem_file << "," << goal_file << "," << running_times[0] << "," << running_times[1] << "," << running_times[2] << "," << sumVec(running_times) << std::endl;
            } else {
                std::ofstream outstream(out_file, std::ofstream::app);
                outstream << domain_file << "," << problem_file << "," << goal_file << "," << running_times[0] << "," << running_times[1] << "," << running_times[2] << "," << sumVec(running_times) << std::endl;
            }
        }
        
        return 0;

    } else{
        Syft::LTLfFONDJokerSynthesizer synthesizer(
            var_mgr,
            domain_file, 
            problem_file,
            goal_file); 
    
        Syft::SynthesisResult result = synthesizer.run(interactive);
    
        auto running_times = synthesizer.get_running_times();
        auto run_time = sumVec(running_times);
    
        if (result.realizability) {
            if (out_file != "") {
                if (!(std::filesystem::exists(out_file))) {
                    std::ofstream outstream(out_file);
                    outstream << "PDDL domain,PDDL problem,LTLf goal,PDDL2DFA (s),LTLf2DFA (s),Synthesis (s),Runtime (s)"<<std::endl;
                    outstream << domain_file << "," << problem_file << "," << goal_file << "," << running_times[0] << "," << running_times[1] << "," << running_times[2] << "," << sumVec(running_times) << std::endl;
                } else {
                    std::ofstream outstream(out_file, std::ofstream::app);
                    outstream << domain_file << "," << problem_file << "," << goal_file << "," << running_times[0] << "," << running_times[1] << "," << running_times[2] << "," << sumVec(running_times) << std::endl;
                }
            }
        
            return 0;
        }else 
            return -20;

    }
    
}