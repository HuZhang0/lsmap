import os
import subprocess
import csv
import sys
import re
import concurrent.futures
cmd1 = 'xxxxxx'

def extract_values(output_lines):
    mapping_info = []
    for line in output_lines:
        node_match = re.search(r"gates\s*=\s*(\d+)", line)
        level_match  = re.search(r"level\s*=\s*(\d+)", line)
        if node_match and level_match:
            mapping_info.append(int(node_match.group(1)))
            mapping_info.append(int(level_match.group(1)))
    return mapping_info

def process_aig_file(filename, folder_path, log_file):
    full_path = os.path.join(folder_path, filename)
    commlut = ["lsmap", "-c", f"read_aiger {full_path};{cmd1};quit"]
    process = subprocess.Popen(
        commlut, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True
    )
    output_lines = []
    while True:
        line = process.stdout.readline()
        if not line:
            break
        output_lines.append(line)
        print(line.strip())  # Display output in the terminal
        log_file.write(line)
        log_file.flush()
    process.wait()
    row = extract_values(output_lines)
    rows = [filename] + row
    return rows

def process_aig_files(folder_path, log_file):
    with open(output_csv, "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(["benchmarks", "and","lev","xxx"])

        aig_files = [filename for filename in os.listdir(folder_path) if filename.endswith(".aig")]

        with concurrent.futures.ThreadPoolExecutor(max_workers=8) as executor:
            # Process each AIG file concurrently
            futures = [
                executor.submit(process_aig_file, filename, folder_path, log_file)
                for filename in aig_files
            ]

            for future in concurrent.futures.as_completed(futures):
                try:
                    rows = future.result()
                    csvwriter.writerow(rows)
                    csvfile.flush()
                except Exception as e:
                    print(f"Error processing file: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <folder_path> <output_csv>")
        sys.exit(1)

    folder_path = sys.argv[1]
    output_csv = sys.argv[2]

    log_file = open(output_csv.replace(".csv", "_log.txt"), "w")

    process_aig_files(folder_path, log_file)
    log_file.close()
