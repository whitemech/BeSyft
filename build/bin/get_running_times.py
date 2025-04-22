import subprocess
import os
import argparse
import sys


# Set up argument parser
parser = argparse.ArgumentParser(description="Executes all the examples")
parser.add_argument(
    "--benchmark",
    type=str,
    required=True,
    help="Desired benchmark."
)

parser.add_argument(
    "--solver",
    type=str,
    required=True,
    help="Synthesis algorithm."
)


args = parser.parse_args()

# Root path to the benchmarks folder
BASE_DIR = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23"

if (args.benchmark == "robot"):
    # Configurable range
    robot_start = 1
    robot_end = 4            # e.g., robot_1 to robot_4
    coffee_start = 2
    coffee_end = 16           # e.g., new_coffee2 to new_coffee16

    # Fixed settings
    executable = "./syft4fond"
    domain_file = "domain-fond.pddl"
    if (args.solver == "joker"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/joker_running_times.csv"
    elif (args.solver == "best_effort"):
        solver_id = "1"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/best_effort_running_times.csv"
    else:
        print("Error: No Synthesizer with this name.")
        sys.exit(1)


    # Run the command for each robot and coffee index
    for robot_num in range(robot_start, robot_end + 1):
        robot_dir = f"robot_{robot_num}"

        for coffee_num in range(coffee_start, coffee_end + 1):
            problem_file = f"new_coffee{coffee_num}.pddl"
            goal_file = f"new_coffee{coffee_num}.ltlf"

            cmd = [
                executable,
                "-d", os.path.join(BASE_DIR, robot_dir, domain_file),
                "-p", os.path.join(BASE_DIR, robot_dir, problem_file),
                "-g", os.path.join(BASE_DIR, robot_dir, goal_file),
                "-s", solver_id,
                "-o", output_file
            ]

            print(f"🚀 Running for robot_{robot_num} with new_coffee{coffee_num}...")
            subprocess.run(cmd)

elif (args.benchmark == "rovers"):
    # Configurable range
    rovers_start = 1
    rovers_end = 4            # e.g., robot_1 to robot_4
    p_start = 1
    p_end = 40           # e.g., new_coffee2 to new_coffee16

    # Fixed settings
    executable = "./syft4fond"
    domain_file = "domain-fond.pddl"
    solver_id = "2"
    output_file = "joker_running_times.csv"

    # Run the command for each robot and coffee index
    for rovers_num in range(rovers_start, rovers_end + 1):
        robot_dir = f"rovers_{rovers_num}"

        for p_num in range(p_start, p_end + 1):
            if p_num < 10:
                problem_file = f"p0{p_num}.pddl"
                goal_file = f"p0{p_num}.ltlf"
            else:
                problem_file = f"p{p_num}.pddl"
                goal_file = f"p{p_num}.ltlf"

            cmd = [
                executable,
                "-d", os.path.join(BASE_DIR, robot_dir, domain_file),
                "-p", os.path.join(BASE_DIR, robot_dir, problem_file),
                "-g", os.path.join(BASE_DIR, robot_dir, goal_file),
                "-s", solver_id,
                "-o", output_file
            ]

            print(f"Running for rovers_{rovers_num} with p{p_num}...")
            subprocess.run(cmd)
else:
    print("Error: No benchmark with that name.")
    sys.exit(1)

