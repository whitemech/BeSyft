/*
* class LTLfFONDJokerSynthesizer
* implements LTLf Joker synthesis in FOND domains
*/

#ifndef LTLFFONDJOKERSYNTHESIZER_H
#define LTLFFONDJOKERSYNTHESIZER_H

#include<string>
#include<fstream>
#include<boost/algorithm/string/predicate.hpp>
#include<boost/algorithm/string/classification.hpp>
#include<boost/algorithm/string/split.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<boost/algorithm/string/replace.hpp>
#include<boost/algorithm/string.hpp>
#include<unordered_set>
#include<cuddObj.hh>
#include"SymbolicStateDfa.h"
#include"Domain.h"
#include"spotparser.h"
#include"ReachabilitySynthesizer.h"
#include"JokerReachabilitySynthesizer.h"
#include"Stopwatch.h"

namespace Syft {

    class LTLfFONDJokerSynthesizer {
        protected:
            // data members
            std::shared_ptr<VarMgr> var_mgr_;

            std::string domain_file_;
            std::string init_file_;
            std::string goal_file_;

            std::vector<double> running_times_;

            std::string parse_goal(const Domain& domain, std::string& ltlf_goal) const;

        public:
            LTLfFONDJokerSynthesizer(
                std::shared_ptr<VarMgr> var_mgr,
                const std::string& domain_file,
                const std::string& problem_file,
                const std::string& ltlf_file
            );

            SynthesisResult run(const bool inter);

            std::vector<double> get_running_times() const {
                return running_times_;
            }


            void interactive(const Domain& domain, const SymbolicStateDfa& product, const SynthesisResult& joker_result) const;
    };

} 
#endif // LTLFFONDJOKERSYNTHESIZER_H