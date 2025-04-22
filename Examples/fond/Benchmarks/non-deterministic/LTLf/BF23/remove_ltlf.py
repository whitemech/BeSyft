import os
import argparse

def remove_ltlf_files_from_folder(folder):
    count = 0
    for filename in os.listdir(folder):
        if filename.endswith(".ltlf"):
            path = os.path.join(folder, filename)
            os.remove(path)
            count += 1
    return count

parser = argparse.ArgumentParser(description="Remove all .ltlf files from one or more folders.")
parser.add_argument("folders", nargs="+", help="Paths to folders to clean .ltlf files from.")
args = parser.parse_args()

total = 0
for folder in args.folders:
    if os.path.isdir(folder):
        removed = remove_ltlf_files_from_folder(folder)
        print(f"🗑️  Removed {removed} .ltlf files from '{folder}'")
        total += removed
    else:
        print(f"⚠️  Skipping '{folder}' (not a valid directory)")

print(f"\n✅ Done. Total .ltlf files removed: {total}")