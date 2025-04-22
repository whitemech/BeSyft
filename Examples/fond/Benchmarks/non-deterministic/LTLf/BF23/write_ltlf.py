import json
import os
import re
import argparse

# Set up argument parser
parser = argparse.ArgumentParser(description="Generate .ltlf files from a JSON file.")
parser.add_argument(
    "--input-folder",
    type=str,
    required=True,
    help="Path to the folder containing the JSON file."
)
parser.add_argument(
    "--json-filename",
    type=str,
    default="tegs.json",
    help="Name of the JSON file (default: coffee_formulas.json)."
)

modal_pattern = re.compile(r'([FGXU])\(([^()]+)\)')

def lowercase_inside(match):
    op = match.group(1)            # e.g. 'F' or 'G' or 'U'
    content = match.group(2)       # e.g. 'on D Table'
    return f"{op}({content.lower()})"


args = parser.parse_args()

# Build full path to the JSON file
json_path = os.path.join(args.input_folder, args.json_filename)

# Load JSON content
with open(json_path, "r") as f:
    data = json.load(f)

# Write .ltlf files
for name, formula in data.items():
    sanitized_formula = formula.replace(" ", "_")
    final_formula = modal_pattern.sub(lowercase_inside, sanitized_formula)
    file_path = os.path.join(args.input_folder, f"{name}.ltlf")
    with open(file_path, "w") as f:
        f.write(final_formula)

print(f"Generated {len(data)} .ltlf files in '{args.input_folder}'")