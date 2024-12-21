# Step 1: Read the contents of both files
with open('a1.txt', 'r') as file1, open('a2.txt', 'r') as file2:
    lines1 = file1.readlines()
    lines2 = file2.readlines()

# Step 2: Parse the lines into a dictionary with the text path as the key and a set of patterns as the value
def parse_lines(lines):
    parsed = {}
    for line in lines:
        parts = line.strip().split()
        if parts:
            key = parts[0]
            patterns = set(parts[1:])
            parsed[key] = patterns
    return parsed

parsed1 = parse_lines(lines1)
parsed2 = parse_lines(lines2)

# Step 3: Compare the dictionaries
all_match = True
for key in parsed1:
    if key in parsed2:
        if parsed1[key] != parsed2[key]:
            all_match = False
            print(f"Patterns for {key} do not match:")
            print(f"a1.txt: {parsed1[key]}")
            print(f"a2.txt: {parsed2[key]}")
    else:
        all_match = False
        print(f"{key} found in a1.txt but not in a2.txt")

for key in parsed2:
    if key not in parsed1:
        all_match = False
        print(f"{key} found in a2.txt but not in a1.txt")

# Step 4: Print success message if all lines match
if all_match:
    print("All lines match between a1.txt and a2.txt.")