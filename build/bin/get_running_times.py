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


if (args.benchmark == "robot"):
    BASE_DIR = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23"
    # Configurable range
    robot_start = 4
    robot_end = 4            # e.g., robot_1 to robot_4
    coffee_start = 2
    coffee_end = 16          # e.g., new_coffee2 to new_coffee16

    # Fixed settings
    executable = "./syft4fond"
    domain_file = "domain-fond.pddl"
    timeout_seconds = "1000"

    if (args.solver == "joker"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/joker_moves_count.csv"
    elif (args.solver == "best_effort"):
        solver_id = "1"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/be_moves_count.csv"
    elif (args.solver == "winning"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/winning_running_times_final.csv"
    else:
        print("Error: No Synthesizer with this name.")
        sys.exit(1)


    # Run the command for each robot and coffee index
    for robot_num in range(robot_start, robot_end + 1):
        if (args.solver == "winning"):
            robot_dir = f"robot_{robot_num}_win"
        else:
            robot_dir = f"robot_{robot_num}"

        for coffee_num in range(coffee_start, coffee_end + 1):
            problem_file = f"new_coffee{coffee_num}.pddl"
            goal_file = f"new_new_coffee{coffee_num}.ltlf"

            cmd = [
                "timeout", timeout_seconds,
                executable,
                "-d", os.path.join(BASE_DIR, robot_dir, domain_file),
                "-p", os.path.join(BASE_DIR, robot_dir, problem_file),
                "-g", os.path.join(BASE_DIR, robot_dir, goal_file),
                "-s", solver_id,
                "-o", output_file
            ]

            print("Running for" + robot_dir + "with" + goal_file)
            result = subprocess.run(cmd, capture_output=True)
            if result.returncode == 124:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",TIMEOUT,TIMEOUT,TIMEOUT,TIMEOUT\n")
            elif result.returncode == -20:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",NONE,NONE,NONE,NONE\n")
            elif result.returncode != 0:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",MEM,MEM,MEM,MEM\n")
                

elif (args.benchmark == "rovers"):
    BASE_DIR = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23"
    # Configurable range
    rovers_start = 1
    rovers_end = 4            # e.g., rovers_1 to rovers_4
    p_start = 1
    p_end = 40           # e.g., p01 to p40

    # Fixed settings
    executable = "./syft4fond"
    domain_file = "domain-fond.pddl"
    timeout_seconds = "1000"
    if (args.solver == "joker"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/joker_running_times_rover.csv"
    elif (args.solver == "best_effort"):
        solver_id = "1"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/best_effort_running_times_rover.csv"
    elif (args.solver == "winning"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/winning_running_times_rover.csv"
    else:
        print("Error: No Synthesizer with this name.")
        sys.exit(1)

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
                "timeout", timeout_seconds,
                executable,
                "-d", os.path.join(BASE_DIR, robot_dir, domain_file),
                "-p", os.path.join(BASE_DIR, robot_dir, problem_file),
                "-g", os.path.join(BASE_DIR, robot_dir, goal_file),
                "-s", solver_id,
                "-o", output_file
            ]

            print(f"Running for rovers_{rovers_num} with p{p_num}...")
            result = subprocess.run(cmd, capture_output=True)
            if result.returncode == 124:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",TIMEOUT,TIMEOUT,TIMEOUT,TIMEOUT\n")
            elif result.returncode == -20:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",NONW,NONE,NONE,NONE\n")
            elif result.returncode != 0:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",MEM,MEM,MEM,MEM\n")

elif (args.benchmark == "block"):
    BASE_DIR = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23"
    # Configurable range
    block_start = 2
    block_end = 4            # e.g., rovers_1 to rovers_4
    p_start = 6
    p_end = 15          # e.g., p01 to p40

    # Fixed settings
    executable = "./syft4fond"
    domain_file = "domain-fond.pddl"
    timeout_seconds = "1000"
    if (args.solver == "joker"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/joker_running_times_block.csv"
    elif (args.solver == "best_effort"):
        solver_id = "1"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/best_effort_running_times_block.csv"
    elif (args.solver == "winning"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/non-deterministic/LTLf/BF23/winning_running_times_block.csv"
    else:
        print("Error: No Synthesizer with this name.")
        sys.exit(1)

    # Run the command for each robot and coffee index
    for rovers_num in range(block_start, block_end + 1):
        robot_dir = f"blocksworld_{rovers_num}"
        if rovers_num == 1:
            name = "a"
        elif rovers_num == 2:
            name = "b"
        elif rovers_num == 3:
            name = "c"
        else:
            name = "d" 

        for p_num in range(p_start, p_end + 1):
            
            if p_num < 10:
                problem_file = f"{name}0{p_num}.pddl"
                goal_file = f"{name}0{p_num}.ltlf"
            else:
                problem_file = f"{name}{p_num}.pddl"
                goal_file = f"{name}{p_num}.ltlf"

            cmd = [
                "timeout", timeout_seconds,
                executable,
                "-d", os.path.join(BASE_DIR, robot_dir, domain_file),
                "-p", os.path.join(BASE_DIR, robot_dir, problem_file),
                "-g", os.path.join(BASE_DIR, robot_dir, goal_file),
                "-s", solver_id,
                "-o", output_file
            ]

            print(f"Running for blocks_{rovers_num} with {name}{p_num}...")
            result = subprocess.run(cmd, capture_output=True)
            if result.returncode == 124:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",TIMEOUT,TIMEOUT,TIMEOUT,TIMEOUT\n")
            elif result.returncode == -20:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",NONE,NONE,NONE,NONE\n")
            elif result.returncode != 0:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",MEM,MEM,MEM,MEM\n")
elif (args.benchmark == "triangle"):
    BASE_DIR = "../../Examples/fond/Benchmarks/TriangleTireWorld"
    # Configurable range
    triangle_start = 4
    triangle_end = 4            # e.g., triangle_1 to triangle_4
    p_start = 2
    p_end = 11          # e.g., p3 to p12

    # Fixed settings
    executable = "./syft4fond"
    domain_file = "domain_triange.pddl"
    timeout_seconds = "1000"
    if (args.solver == "joker"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/TriangleTireWorld/joker_moves_counter.csv"
    elif (args.solver == "best_effort"):
        solver_id = "1"
        output_file = "../../Examples/fond/Benchmarks/TriangleTireWorld/be_moves_counter.csv"
    elif (args.solver == "winning"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/TriangleTireWorld/winning_running_times_triangle_tre_new.csv"
    else:
        print("Error: No Synthesizer with this name.")
        sys.exit(1)

    # Run the command for each robot and coffee index
    for rovers_num in range(triangle_start, triangle_end + 1):
        if (args.solver == "winning"):
            robot_dir = f"tire_{rovers_num}_new_win"
        else:
            robot_dir = f"tire_{rovers_num}_new"
        

        for p_num in range(p_start, p_end + 1):
            
            
            problem_file = f"p{p_num}.pddl"
            goal_file = f"tre_p{p_num}.ltlf"

            cmd = [
                "timeout", timeout_seconds,
                executable,
                "-d", os.path.join(BASE_DIR, robot_dir, domain_file),
                "-p", os.path.join(BASE_DIR, robot_dir, problem_file),
                "-g", os.path.join(BASE_DIR, robot_dir, goal_file),
                "-s", solver_id,
                "-o", output_file
            ]

            print(f"Running for tire_{rovers_num} with p{p_num}...")
            result = subprocess.run(cmd, capture_output=True)
            if result.returncode == 124:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",TIMEOUT,TIMEOUT,TIMEOUT,TIMEOUT\n")
            elif result.returncode == -20:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",NONW,NONE,NONE,NONE\n")
            elif result.returncode != 0:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",MEM,MEM,MEM,MEM\n")
elif (args.benchmark == "grid"):
    BASE_DIR = "../../Examples/fond/Benchmarks/pddl-generators-main/grid"
    # Configurable range
    triangle_start = 4
    triangle_end = 4            # e.g., triangle_1 to triangle_4
    p_start = 1
    p_end = 10          # e.g., p1 to p10

    # Fixed settings
    executable = "./syft4fond"
    domain_file = "domain.pddl"
    timeout_seconds = "1000"
    if (args.solver == "joker"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/pddl-generators-main/grid/joker_moves_count.csv"
    elif (args.solver == "best_effort"):
        solver_id = "1"
        output_file = "../../Examples/fond/Benchmarks/pddl-generators-main/grid/be_moves_count.csv"
    elif (args.solver == "winning"):
        solver_id = "2"
        output_file = "../../Examples/fond/Benchmarks/pddl-generators-main/grid/winning_running_times_grid_final.csv"
    else:
        print("Error: No Synthesizer with this name.")
        sys.exit(1)

    # Run the command for each robot and coffee index
    for rovers_num in range(triangle_start, triangle_end + 1):
        if (args.solver == "winning"):
            robot_dir = f"GRID_{rovers_num}_win"
        else:
            robot_dir = f"GRID_{rovers_num}"
        

        for p_num in range(p_start, p_end + 1):
            
            
            problem_file = f"new_p{p_num}.pddl"
            goal_file = f"new_p{p_num}.ltlf"

            cmd = [
                "timeout", timeout_seconds,
                executable,
                "-d", os.path.join(BASE_DIR, robot_dir, domain_file),
                "-p", os.path.join(BASE_DIR, robot_dir, problem_file),
                "-g", os.path.join(BASE_DIR, robot_dir, goal_file),
                "-s", solver_id,
                "-o", output_file
            ]

            print(f"Running for GRID_{rovers_num} with p{p_num}...")
            result = subprocess.run(cmd, capture_output=True) #
            if result.returncode == 124:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",TIMEOUT,TIMEOUT,TIMEOUT,TIMEOUT\n")
            elif result.returncode == -20:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",NONE,NONE,NONE,NONE\n")
            elif result.returncode != 0:
                with open(output_file, "a") as f:
                   f.write(os.path.join(BASE_DIR, robot_dir, domain_file) + "," + os.path.join(BASE_DIR, robot_dir, problem_file) + "," + os.path.join(BASE_DIR, robot_dir, goal_file) + ",MEM,MEM,MEM,MEM\n")
elif (args.benchmark == "counter_reach"):
    BASE_DIR = "CounterGames/"
    starting_player = "1"
    # Configurable range
    counter_start = 1
    counter_end = 10            # e.g., triangle_1 to triangle_4

    # Fixed settings
    executable = "./BeSyft"
    
    timeout_seconds = "1000"
    if (args.solver == "joker"):
        solver_id = "6"
        output_file = "../../Benchmarks/CounterGames/joker_times.csv"
    elif (args.solver == "best_effort"):
        solver_id = "3"
        output_file = "../../Benchmarks/CounterGames/BE_times.csv"
    else:
        print("Error: No Synthesizer with this name.")
        sys.exit(1)

    # Run the command for each robot and coffee index
    for ag_id in range(counter_start, counter_end + 1):
        
        for env_id in range(counter_start, counter_end + 1):
             
            agent_file = f"goal_{ag_id}.ltlf"
            part_file = f"part_{ag_id}.part"
            env_file = f"env_{env_id}.ltlf"

            cmd = [
                "timeout", timeout_seconds,
                executable,
                "-a", os.path.join(BASE_DIR, agent_file),
                "-e", os.path.join(BASE_DIR, env_file),
                "-p", os.path.join(BASE_DIR, part_file),
                "-s", starting_player,
                "-t", solver_id,
                "-f", output_file
            ]

            print(f"Running for counter_{ag_id} with env_{env_id} ...")
            result = subprocess.run(cmd, capture_output=True)
            if result.returncode == 124:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, agent_file) + "," + os.path.join(BASE_DIR, env_file) + ",TIMEOUT,TIMEOUT,TIMEOUT,TIMEOUT\n")
            elif result.returncode == -20:
                with open(output_file, "a") as f:
                    f.write(os.path.join(BASE_DIR, agent_file) + "," + os.path.join(BASE_DIR, env_file) + ",NONE,NONE,NONE,NONE\n")
            elif result.returncode != 0:
                with open(output_file, "a") as f:
                   f.write(os.path.join(BASE_DIR, agent_file) + "," + os.path.join(BASE_DIR, env_file) + ",MEM,MEM,MEM,MEM\n")
elif (args.benchmark == "counter_safe"):
    BASE_DIR = "CounterGames/"
    starting_player = "1"
    # Configurable range
    counter_start = 1
    counter_end = 10            # e.g., triangle_1 to triangle_4

    # Fixed settings
    executable = "./BeSyft"
    
    timeout_seconds = "1000"
    if (args.solver == "joker"):
        solver_id = "6"
        output_file = "../../Benchmarks/CounterGames/joker_safe_times.csv"
    elif (args.solver == "best_effort"):
        solver_id = "3"
        output_file = "../../Benchmarks/CounterGames/BE_safe_times.csv"
    else:
        print("Error: No Synthesizer with this name.")
        sys.exit(1)

    # Run the command for each robot and coffee index
    for ag_id in range(counter_start, counter_end + 1):
             
        agent_file = f"goal_{ag_id}.ltlf"
        part_file = f"part_{ag_id}.part"
        env_file = f"env_safety.ltlf"

        cmd = [
            "timeout", timeout_seconds,
            executable,
            "-a", os.path.join(BASE_DIR, agent_file),
            "-e", os.path.join(BASE_DIR, env_file),
            "-p", os.path.join(BASE_DIR, part_file),
            "-s", starting_player,
            "-t", solver_id,
            "-f", output_file
        ]

        print(f"Running for counter_{ag_id} with env_safety ...")
        result = subprocess.run(cmd, capture_output=True)
        if result.returncode == 124:
            with open(output_file, "a") as f:
                f.write(os.path.join(BASE_DIR, agent_file) + "," + os.path.join(BASE_DIR, env_file) + ",TIMEOUT,TIMEOUT,TIMEOUT,TIMEOUT\n")
        elif result.returncode == -20:
            with open(output_file, "a") as f:
                f.write(os.path.join(BASE_DIR, agent_file) + "," + os.path.join(BASE_DIR, env_file) + ",NONE,NONE,NONE,NONE\n")
        elif result.returncode != 0:
            with open(output_file, "a") as f:
                f.write(os.path.join(BASE_DIR, agent_file) + "," + os.path.join(BASE_DIR, env_file) + ",MEM,MEM,MEM,MEM\n")
else:
    print("Error: No benchmark with that name.")
    sys.exit(1)

##depot, childsnack, grid
