# Advanced OS - Assignment 1

## Overview
This program reads an input file and writes a reversed version of its content into a new file inside the `Assignment1` directory.  
It supports **three operation modes** based on the `flag` argument:

- **Flag 0** → Reverse the entire file in fixed-size blocks.
- **Flag 1** → Reverse the file in power-of-two chunk sizes (cache-aware).
- **Flag 2** → Reverse only parts of the file based on start and end indices, keeping the middle section unchanged.

The output files are named `0_<filename>`, `1_<filename>`, or `2_<filename>` depending on the mode.

---

## Usage Q1

```bash
./a.out <input_file> <flag> [block_size/start] [end]
# Reverse file in blocks of 4 KB
./a.out sample.txt 0 4096

# Reverse file using cache-friendly chunks
./a.out sample.txt 1

# Reverse all except bytes from start=100 to end=200
./a.out sample.txt 2 100 200
```



## System Call Design
The program directly uses low-level POSIX system calls for fine-grained control over file handling:

- **open()** → Opens the file with specific permissions.
- **read() / write()** → Transfers raw bytes between memory and disk.
- **lseek()** → Moves the file pointer without unnecessary reads.
- **stat()** → Retrieves file/directory metadata (permissions, size).
- **chmod()** → Sets permissions if required.
- **mkdir()** → Creates the output directory.
- **perror()** → Handles and displays detailed error messages.
- **nanosleep()** → Adds tiny delays to smooth progress printing.

## Cache Optimization
### Power-of-Two Chunk Sizes (Flags 1 & 2)
The chunk size is computed as the largest power of two ≤ (file_size / 250).  
Power-of-two sizes align with CPU cache lines and memory pages, minimizing cache misses.

### Block-by-Block Processing
Instead of reading the whole file, the program processes small blocks to save memory and improve performance.

### Selective Reading/Writing
Using **lseek()** allows jumping directly to offsets, avoiding unnecessary data loads.

---
## Flag 0 – Fixed Block Size Reversal
- The file is reversed in chunks of a user-specified block size (e.g., 4 KB).  
- Useful for controlling memory usage and I/O performance manually.  
- Reads a block from the end, reverses it in memory, and writes it to the output.

## Flag 1 – Cache-Friendly Reversal
- Automatically calculates an optimal chunk size: largest power of two ≤ (file_size / 250).  
- This aligns with CPU cache lines and memory pages, reducing cache misses.  
- No user-provided block size needed.

## Flag 2 – Partial Reversal Diagram
When using Flag 2, the program:

1. Reverses the first part (before start index)  
2. Keeps the middle section (start to end) unchanged  
3. Reverses the last part (after end index)  


# Q2 – Verification & Permission Checking Program

## Overview
This program verifies:
1. **Permissions** of the new file, original file, and output directory.
2. **Content correctness** based on the reversal logic used in Q1 for `flag 0`, `flag 1`, and `flag 2`.
3. **File size matching** between original and new file.

It ensures the processing is correct and files follow the required permission codes.

---

## Features
- **Permission Checking**
  - Uses `stat()` to retrieve permissions.
  - Prints read, write, and execute permissions for **user**, **group**, and **others**.
  - Checks whether permissions match the expected values:
    - Directory: `700`
    - New file: `600`
    - Original file: `644`
- **Content Verification**
  - **Flag 0 (Block-wise Reversal)**  
    Reads in fixed-size blocks and checks that each block in the new file is the reverse of the corresponding block in the old file.
  - **Flag 1 (Full File Reversal)**  
    Reads original file from start and new file from end, comparing reversed chunks.
  - **Flag 2 (Partial Range Reversal)**  
    - Bytes before `start`: reversed.
    - Bytes between `start` and `end`: unchanged.
    - Bytes after `end`: reversed.

---

## Expected Permission Codes
- **Directory (Assignment1/):** `700` → User: read+write+execute; Group/Others: none.
- **New file (Assignment1/X_input.txt):** `600` → User: read+write; Group/Others: none.
- **Original file:** `644` → User: read+write; Group: read; Others: read.

---

## Output Format
The output contains:
- Directory creation status.
- Verification results (content correctness, file size match).
- Permissions for **user**, **group**, and **others** on:
  - New file
  - Old file
  - Directory  
  (Total 30 lines of permission checks).
## Usage Q2
```bash
# Flag 0 – Block-wise reversal
./a.out Assignment1/0_input.txt input.txt Assignment1 0 1024

# Flag 1 – Full file reversal
./a.out Assignment1/1_input.txt input.txt Assignment1 1

# Flag 2 – Partial range reversal
./a.out Assignment1/2_input.txt input.txt Assignment1 2 5 10





