#include <iostream>

#include <unistd.h>

#include <fcntl.h>

#include <errno.h>

#include <math.h>

#include <sys/stat.h>

#include <stdexcept>
 // read == input
// write == output
using namespace std;
extern int errno;
void print_permisions(struct stat stat_info, char * c) {
    if (stat_info.st_mode & S_IRUSR) {
        cout << "User has read permissions on " << c << ": Yes" << endl;
    } else {
        cout << "User has read permissions on " << c << ": No" << endl;
    }
    if (stat_info.st_mode & S_IWUSR) {
        cout << "User has write permission on " << c << ": Yes" << endl;
    } else {
        cout << "User has write permission on " << c << ": No" << endl;
    }
    if (stat_info.st_mode & S_IXUSR) {
        cout << "User has execute permission on " << c << ": Yes" << endl;
    } else {
        cout << "User has execute permission " << c << ": No" << endl;
    }
    // group permisions
    if (stat_info.st_mode & S_IRGRP) {
        cout << "Group has read permissions on " << c << ": Yes" << endl;
    } else {
        cout << "Group has read permissions on " << c << ": No" << endl;
    }
    if (stat_info.st_mode & S_IWGRP) {
        cout << "Group has write permission on " << c << ": Yes" << endl;
    } else {
        cout << "Group has write permission on " << c << ": No" << endl;
    }
    if (stat_info.st_mode & S_IXGRP) {
        cout << "Group has execute permission on " << c << ": Yes" << endl;
    } else {
        cout << "Group has execute permission on " << c << ": No" << endl;
    }
    // other permisions
    if (stat_info.st_mode & S_IROTH) {
        cout << "Others has read permissions on " << c << ": Yes" << endl;
    } else {
        cout << "Others has read permissions on " << c << ": No" << endl;
    }
    if (stat_info.st_mode & S_IWOTH) {
        cout << "Others has write permission on " << c << ": Yes" << endl;
    } else {
        cout << "Others has write permission on " << c << ": No" << endl;
    }
    if (stat_info.st_mode & S_IXOTH) {
        cout << "Others has execute permission on " << c << ": Yes" << endl;
    } else {
        cout << "Others has execute permission on " << c << ": No" << endl;
    }
}
void check_valid_inputs_2(int start, int end, int size) {
    if (start <= 0) {
        throw runtime_error("provide valid start index, start must be greater than 0");
    }
    if (start > end) {
        throw runtime_error("provide valid start and end index, start must be lesser than or equal to end");
    }
    if (end >= size - 1) {
        throw runtime_error("provide valid end index, end must be lesser than size-1");
    }
}
int string_int_helper(char * c) {
    int i = 0;
    while ( * (c + i) != '\0') {
        i++;
    }
    int num_digits = i;
    int res = 0;
    for (int k = 0; k < num_digits; k++) {
        res = res + ( * (c + k) - '0') * pow(10, num_digits - k - 1);
    }
    return res;

}
int chunk_size_helper(int size) {
    int file_chunk = size / 250; //0.4% of the file
    int chunk_size;
    if (file_chunk == 0) {
        chunk_size = 1;
    } else {
        int power_2 = (int)(log((double) file_chunk) / log(2.0));
        chunk_size = pow(2, power_2);
    }
    return chunk_size;
}
bool check_equal(char * read_input, char * read_output, int num_elements) {
    for (int i = 0; i < num_elements; i++) {
        if (read_output[i] != read_input[i]) {
            return false;
        }
    }
    return true;
}
bool check_reversal(char * read_input, char * read_output, int num_elements) {
    for (int i = 0; i < num_elements; i++) {
        if (read_output[i] != read_input[num_elements - 1 - i]) {
            return false;
        }
    }
    return true;
}
bool verify_0(int fd_output, int fd_input, int block_size, off_t buff_size_input, off_t buff_size_output) {
    //reseting the file_offset to the begining
    if (lseek(fd_input, 0, SEEK_SET) == -1) {
        perror("lseek failed for input file, routine: verify_0");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd_output, 0, SEEK_SET) == -1) {
        perror("lseek failed for output file, routine: verify_0");
        exit(EXIT_FAILURE);
    }
    if (buff_size_input == buff_size_output) {
        int buff_size = buff_size_input;
        int num_blocks = buff_size / block_size;
        int rem_num_ele = buff_size % block_size;
        int prev = 0;
        for (int i = 0; i < num_blocks; i++) {
            char * read_input = (char * ) malloc(block_size);
            if (read(fd_input, read_input, block_size) == -1) {
                perror("failed to read from input file for blocks, routine: verify_0");
                exit(EXIT_FAILURE);
            }
            char * read_output = (char * ) malloc(block_size);
            if (read(fd_output, read_output, block_size) == -1) {
                perror("failed to read from output file , routine: verify_0");
                exit(EXIT_FAILURE);
            }
            bool ret_reversal_check = check_reversal(read_input, read_output, block_size);
            if (ret_reversal_check == false) {
                return false;
            }
            prev = prev + block_size;
            if (lseek(fd_input, prev, SEEK_SET) == -1) {
                perror("lseek failed to set the file offset to prev for input file");
                exit(EXIT_FAILURE);
            }
            if (lseek(fd_output, prev, SEEK_SET) == -1) {
                perror("lseek failed to set the file offset to prev for output file");
                exit(EXIT_FAILURE);
            }
            free(read_input);
            free(read_output);
        }
        if (rem_num_ele > 0) {
            char * read_input_rem = (char * ) malloc(rem_num_ele);
            if (read(fd_input, read_input_rem, rem_num_ele) == -1) {
                perror("failed to read from input file for remaining elements, routine: verify_0");
                exit(EXIT_FAILURE);
            }
            char * read_output_rem = (char * ) malloc(rem_num_ele);
            if (read(fd_output, read_output_rem, rem_num_ele) == -1) {
                perror("failed to read from output file for remaining elements, routine: verify_0");
                exit(EXIT_FAILURE);
            }
            bool ret_reversal_check = check_reversal(read_input_rem, read_output_rem, rem_num_ele);
            if (ret_reversal_check == false) {
                return false;
            }
            free(read_input_rem);
            free(read_output_rem);

        }

    } else {
        return false;
    }
    return true;

}
bool verify_1(int fd_output, int fd_input, off_t buff_size_input, off_t buff_size_output) {
    //reseting the file_offset to the begining
    if (lseek(fd_input, 0, SEEK_SET) == -1) {
        perror("lseek failed for input file, routine: verify_1");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd_output, 0, SEEK_SET) == -1) {
        perror("lseek failed for output file, routine: verify_1");
        exit(EXIT_FAILURE);
    }
    if (buff_size_output == buff_size_input) {
        int buff_size = buff_size_input;
        int chunk_size = chunk_size_helper(buff_size);
        int number_chunks = buff_size / chunk_size;
        int left_elements = buff_size % chunk_size;
        if (left_elements > 0) {
            char * read_input = (char * ) malloc(left_elements);
            char * read_output = (char * ) malloc(left_elements);
            int last_ele_off = number_chunks * chunk_size;
            // reading from the input file , will have to read from backwards
            if (lseek(fd_input, last_ele_off, SEEK_SET) == -1) {
                perror("lseek failed for input file to last_ele_off, routine: verify_1");
                exit(EXIT_FAILURE);
            }
            if (read(fd_input, read_input, left_elements) == -1) {
                perror("read failed for input file for left_elements, routine: verify_1");
                exit(EXIT_FAILURE);
            }
            if (read(fd_output, read_output, left_elements) == -1) {
                perror("read failed for output file for left_elements, routine: verify_1");
                exit(EXIT_FAILURE);
            }
            bool ret_reversal_check = check_reversal(read_input, read_output, left_elements);
            if (ret_reversal_check == false) {
                return false;
            }
            free(read_input);
            free(read_output);
            if (lseek(fd_input, 0, SEEK_SET) == -1) {
                perror("lseek failed for input file to reset the file offset to 0, routine: verify_1");
                exit(EXIT_FAILURE);
            }
        }
        int ind = number_chunks - 1;
        while (ind >= 0) {
            char * read_input = (char * ) malloc(chunk_size);
            char * read_output = (char * ) malloc(chunk_size);
            int offset = ind * chunk_size;
            if (lseek(fd_input, offset, SEEK_SET) == -1) {
                perror("lseek failed for input file to set to offset, routine: verify_1");
                exit(EXIT_FAILURE);
            }
            if (read(fd_input, read_input, chunk_size) == -1) {
                perror("read failed for input file for chunk_size, routine: verify_1");
                exit(EXIT_FAILURE);
            }
            if (read(fd_output, read_output, chunk_size) == -1) {
                perror("read failed for output file for chunk_size, routine: verify_1");
                exit(EXIT_FAILURE);
            }
            bool ret_reversal_check = check_reversal(read_input, read_output, chunk_size);
            if (ret_reversal_check == false) {
                return false;
            }
            free(read_input);
            free(read_output);
            //reseting to the begining
            if (lseek(fd_input, 0, SEEK_SET) == -1) {
                perror("lseek failed for input file to set to 0, routine: verify_1");
                exit(EXIT_FAILURE);
            }
            ind = ind - 1;
        }


    } else {
        return false;
    }
    return true;
}
bool verify_2(int fd_output, int fd_input, int size_of_file, int start, int end) {
    //reseting the file_offset to the begining
    if (lseek(fd_input, 0, SEEK_SET) == -1) {
        perror("lseek failed for input file to set file offset to 0, routine: verify_2");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd_output, 0, SEEK_SET) == -1) {
        perror("lseek failed for output file to set file offset to 0, routine: verify_2");
        exit(EXIT_FAILURE);
    }
    int start_chunk_size = chunk_size_helper(start);
    int number_chunks = start / start_chunk_size;
    int left_elements = start % start_chunk_size;
    if (left_elements > 0) {
        char * read_input = (char * ) malloc(left_elements);
        char * read_output = (char * ) malloc(left_elements);
        int last_ele_off = number_chunks * start_chunk_size;
        if (lseek(fd_input, last_ele_off, SEEK_SET) == -1) {
            perror("lseek failed for input file to set file offset to last_ele_off, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_input, read_input, left_elements) == -1) {
            perror("read failed for left_elements in read_input buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_output, read_output, left_elements) == -1) {
            perror("read failed for left_elements in read_output buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        bool ret_reversal_check = check_reversal(read_input, read_output, left_elements);
        if (ret_reversal_check == false) {
            return false;
        }
        free(read_input);
        free(read_output);
        if (lseek(fd_input, 0, SEEK_SET) == -1) {
            perror("lseek failed for input file to reset the file offset to 0, routine: verify_2");
            exit(EXIT_FAILURE);
        }
    }
    int ind = number_chunks - 1;
    while (ind >= 0) {
        char * read_input = (char * ) malloc(start_chunk_size);
        char * read_output = (char * ) malloc(start_chunk_size);
        int offset = ind * start_chunk_size;
        if (lseek(fd_input, offset, SEEK_SET) == -1) {
            perror("lseek failed for input file to set file offset to offset according to start_chunk_size, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_input, read_input, start_chunk_size) == -1) {
            perror("read failed for start_chunk_size in read_input buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_output, read_output, start_chunk_size) == -1) {
            perror("read failed for start_chunk_size in read_output buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        bool ret_reversal_check = check_reversal(read_input, read_output, start_chunk_size);
        if (ret_reversal_check == false) {
            return false;
        }
        free(read_input);
        free(read_output);
        if (lseek(fd_input, 0, SEEK_SET) == -1) {
            perror("lseek failed for input file to reset the file offset to 0, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        ind = ind - 1;
    }
    // set the file offset of the input file to the start index
    if (lseek(fd_input, start, SEEK_SET) == -1) {
        perror("lseek failed to set the file offset for input file to start, routine: verify_2");
        exit(EXIT_FAILURE);
    }
    // simply from start to end index write it into the text simply
    int mid_file_size = end - start + 1;
    int mid_chunk_size = chunk_size_helper(mid_file_size);
    int mid_number_chunks = mid_file_size / mid_chunk_size;
    int mid_left_elements = mid_file_size % mid_chunk_size;
    int itr = 0;
    while (itr < mid_number_chunks) {
        char * read_input = (char * ) malloc(mid_chunk_size);
        char * read_output = (char * ) malloc(mid_chunk_size);
        if (read(fd_input, read_input, mid_chunk_size) == -1) {
            perror("read failed for mid_chunk_size in read_input buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_output, read_output, mid_chunk_size) == -1) {
            perror("read failed for mid_chunk_size in read_output buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        bool ret_equal_check = check_equal(read_input, read_output, mid_chunk_size);
        if (ret_equal_check == false) {
            return false;
        }
        // move the read_offset
        int to_set = start + (itr + 1) * mid_chunk_size;
        if (lseek(fd_input, to_set, SEEK_SET) == -1) {
            perror("lseek failed to set the file offset for input file to to_set, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        itr++;
        free(read_input);
        free(read_output);
    }
    if (mid_left_elements > 0) {
        char * read_input = (char * ) malloc(mid_left_elements);
        char * read_output = (char * ) malloc(mid_left_elements);
        if (read(fd_input, read_input, mid_left_elements) == -1) {
            perror("read failed for mid_left_elements in read_input buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_output, read_output, mid_left_elements) == -1) {
            perror("read failed for mid_left_elements in read_output buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        bool ret_equal_check = check_equal(read_input, read_output, mid_left_elements);
        if (ret_equal_check == false) {
            return false;
        }
        if (lseek(fd_input, end, SEEK_SET) == -1) {
            perror("lseek failed to set the file offset for input file to end, routine: verify_2");
            exit(EXIT_FAILURE);
        }
    }


    // need to reverse the last part
    int end_file_size = size_of_file - end - 1;
    int end_chunk_size = chunk_size_helper(end_file_size);
    int end_number_chunks = end_file_size / end_chunk_size;
    int end_left_elements = end_file_size % end_chunk_size;
    if (end_left_elements > 0) {
        char * read_input = (char * ) malloc(end_left_elements);
        char * read_output = (char * ) malloc(end_left_elements);
        int last_ele_off = end + 1 + end_number_chunks * end_chunk_size;
        if (lseek(fd_input, last_ele_off, SEEK_SET) == -1) {
            perror("lseek failed for input file to set file offset to last_ele_off from end, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_input, read_input, end_left_elements) == -1) {
            perror("read failed for end_left_elements in read_input buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_output, read_output, end_left_elements) == -1) {
            perror("read failed for end_left_elements in read_output buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        bool ret_reversal_check = check_reversal(read_input, read_output, end_left_elements);
        if (ret_reversal_check == false) {
            return false;
        }
        free(read_input);
        free(read_output);
        if (lseek(fd_input, 0, SEEK_SET) == -1) {
            perror("lseek failed for input file to reset the file offset to 0, routine: verify_2");
            exit(EXIT_FAILURE);
        }
    }
    int ind_e = end_number_chunks - 1;
    while (ind_e >= 0) {
        char * read_input = (char * ) malloc(end_chunk_size);
        char * read_output = (char * ) malloc(end_chunk_size);
        int offset = end + 1 + ind_e * end_chunk_size;
        if (lseek(fd_input, offset, SEEK_SET) == -1) {
            perror("lseek failed for input file to set file offset to offset according to end_chunk_size, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_input, read_input, end_chunk_size) == -1) {
            perror("read failed for end_chunk_size in read_input buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        if (read(fd_output, read_output, end_chunk_size) == -1) {
            perror("read failed for end_chunk_size in read_output buffer, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        bool ret_reversal_check = check_reversal(read_input, read_output, end_chunk_size);
        if (ret_reversal_check == false) {
            return false;
        }
        free(read_input);
        free(read_output);
        if (lseek(fd_input, 0, SEEK_SET) == -1) {
            perror("lseek failed for input file to reset the file offset to 0, routine: verify_2");
            exit(EXIT_FAILURE);
        }
        ind_e--;
    }
    return true;
}
int main(int argc, char * argv[]) {
    struct stat stat_info;
    int ret_val = stat(argv[3], & stat_info);
    if (ret_val == -1) {
        cout << "Directory is created: No" << endl;
    } else {
        mode_t file_type = stat_info.st_mode;
        //   0x41ED    0xF000      0x4000
        if ((file_type & S_IFMT) == S_IFDIR) {
            cout << "Directory is created: Yes" << endl;

        } else {
            cout << "Directory is created: No" << endl;
        }

    }
    // output_file_path is at argv[1]
    // input_file_path is at argv[2]
    int fd_input = open(argv[2], O_RDONLY);
    if (fd_input == -1) {
        perror("open sys call failed for input_file_path");
        exit(EXIT_FAILURE);
    }
    int fd_output = open(argv[1], O_RDONLY);
    if (fd_output == -1) {
        perror("open sys call failed for output_file_path");
        exit(EXIT_FAILURE);
    }
    off_t buff_size_input = lseek(fd_input, 0, SEEK_END);
    if (buff_size_input == -1) {
        perror("lseek failed for input file");
        exit(EXIT_FAILURE);
    }
    off_t buff_size_output = lseek(fd_output, 0, SEEK_END);
    if (buff_size_output == -1) {
        perror("lseek failed for output file");
        exit(EXIT_FAILURE);
    }

    //reseting the file_offset to the begining
    if (lseek(fd_input, 0, SEEK_SET) == -1) {
        perror("lseek failed to reset to begining of the input file");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd_output, 0, SEEK_SET) == -1) {
        perror("lseek failed to reset to begining of the output file");
        exit(EXIT_FAILURE);
    }
    if (buff_size_output == buff_size_input) {
        if ( * (argv[4] + 0) == '0') {
            // verification logic for flag 0
            int block_size = string_int_helper(argv[5]);
            bool verify_flag_0 = verify_0(fd_output, fd_input, block_size, buff_size_input, buff_size_output);
            if (verify_flag_0 == true) {
                cout << "Whether file contents are correctly processed: Yes" << endl;
            }
            else {
                cout << "Whether file contents are correctly processed: No" << endl;
            }
            cout << "Both Files Sizes are Same: Yes" << endl;

        }
        if ( * (argv[4] + 0) == '1') {
            //verification logic for flag 1
            bool verify_flag_1 = verify_1(fd_output, fd_input, buff_size_input, buff_size_output);
            if (verify_flag_1 == true) {
                cout << "Whether file contents are correctly processed: Yes" << endl;
            }
            else {
                cout << "Whether file contents are correctly processed: No" << endl;
            }
            cout << "Both Files Sizes are Same: Yes" << endl;
        }
        if ( * (argv[4] + 0) == '2') {
            //verification logic for falg 2
            int start = string_int_helper(argv[5]);
            int end = string_int_helper(argv[6]);
            int buff_size = buff_size_input;
            try {
                check_valid_inputs_2(start, end, buff_size);
                bool verify_flag_2 = verify_2(fd_output, fd_input, buff_size, start, end);
                if (verify_flag_2 == true) {
                    cout << "Whether file contents are correctly processed: Yes" << endl;
                }
                else {
                    cout << "Whether file contents are correctly processed: No" << endl;
                }
                cout << "Both Files Sizes are Same: Yes" << endl;
            } catch (runtime_error & err) {
                cerr << "Error: " << err.what() << endl;
            }

        }
    } else {
        cout << "Both Files Sizes are Same: No" << endl;
    }
    struct stat stat_newf_info;
    int return_val = stat(argv[1], & stat_newf_info);
    if (return_val == -1) {
        perror("stat failed for output file");
        exit(EXIT_FAILURE);
    }
    char newf[] = "newfile";
    print_permisions(stat_newf_info, newf);
    struct stat stat_oldf_info;
    int reti = stat(argv[2], & stat_oldf_info);
    if (reti == -1) {
        perror("stat failed for input file");
        exit(EXIT_FAILURE);
    }
    char oldf[] = "oldfile";
    print_permisions(stat_oldf_info, oldf);
    char dir[] = "directory";
    struct stat stat_info_dir;
    int ret_val_dir = stat(argv[3], & stat_info_dir);
    if (ret_val_dir == -1) {
        perror("stat failed for directory, Assignment1 directory is not yet created");
        exit(EXIT_FAILURE);
    }
    print_permisions(stat_info_dir, dir);
    close(fd_input);
    close(fd_output);
    return 0;

}