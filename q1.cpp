#include <iostream>

#include <unistd.h>

#include <fcntl.h>

#include <errno.h>

#include <math.h>

#include <sys/stat.h>

using namespace std;
extern int errno;
void print_percentage_console(int number_of_bytes_written, int size_file) {
    double res = (double) number_of_bytes_written / (double) size_file;
    cout << "\r" << res * 100 << flush;
    struct timespec tspec;
    /*
        time_t tv_sec;
        long tv_nsec;
    */
    tspec.tv_sec = 0; 
    tspec.tv_nsec = (long)100000; // 200 ms  
    nanosleep(&tspec, NULL);
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
        if (c[i] == '-') {
            return -1;
        }
        i++;
    }
    int num_digits = i;
    int res = 0;
    for (int k = 0; k < num_digits; k++) {
        res = res + ( * (c + k) - '0') * pow(10, num_digits - k - 1);
    }
    return res;

}
char* path_helper(char * c_2, int size_input_filename, char * c_1) {
    char * res = (char *) malloc(14 + size_input_filename);
    int i = 0;
    while (i <= 13) {
        res[i] = c_2[i];
        i++;
    }
    int k = 0;
    while (i < 14 + size_input_filename) {
        res[i] = c_1[k];
        i++;
        k++;
    }
    return res;
}
char * get_path_name(char * argv_1,char flag) {
    int i = 0;
    bool is_path = false;
    int index_last;
    while ( * (argv_1 + i) != '\0') {
        if ( * (argv_1 + i) == '/') {
            is_path = true;
            index_last = i;
        }
        i++;
    }
    int size_input_filename;

    if (is_path) {
        size_input_filename = i - index_last - 2;
        char c_1[size_input_filename];
        int i = index_last + 1;
        int k = 0;
        while ( * (argv_1 + i) != '\0') {
            c_1[k] = * (argv_1 + i);
            i++;
            k++;
        }
        if (flag == '0') {
            char c_2[] = "Assignment1/0_";
            return path_helper(c_2, size_input_filename, c_1);
            

        }
        if (flag == '1') {
            char c_2[] = "Assignment1/1_";
            return path_helper(c_2, size_input_filename, c_1);
        }
        if (flag == '2') {
            char c_2[] = "Assignment1/2_";
            return path_helper(c_2, size_input_filename, c_1);
        }


    } else {
        size_input_filename = i;
        char * res = (char *) malloc(14 + size_input_filename);
        char c_1[size_input_filename];
        int k = 0;
        int i = 0;
        while ( * (argv_1 + i) != '\0') {
            c_1[k] = * (argv_1 + i);
            i++;
            k++;
        }
        if (flag == '0') {
            char c_2[] = "Assignment1/0_";
            for (int i = 0; i < 14; i++) {
                res[i] = c_2[i];
            }
            int j = 0;
            for (int k = 14; k < 14 + size_input_filename; k++) {
                res[k] = c_1[j];
                j++;
            }
            return res;
        }
        if (flag == '1') {
            char c_2[] = "Assignment1/1_";
            for (int i = 0; i < 14; i++) {
                res[i] = c_2[i];
            }
            int j = 0;
            for (int k = 14; k < 14 + size_input_filename; k++) {
                res[k] = c_1[j];
                j++;
            }
           return res;
        }
        if (flag == '2') {
            char c_2[] = "Assignment1/2_";
            for (int i = 0; i < 14; i++) {
                res[i] = c_2[i];
            }
            int j = 0;
            for (int k = 14; k < 14 + size_input_filename; k++) {
                res[k] = c_1[j];
                j++;
            }
            return res;
        }
    }
    return nullptr;

}
int main(int argc, char * argv[])
{
    /*
        S_IRUSR user-read
        S_IWUSR user-write
        S_IXUSR user-execute
    */
    char dir_name[] = "Assignment1";
    int return_mkdir = mkdir(dir_name, S_IRUSR | S_IWUSR | S_IXUSR);
    if (return_mkdir == -1) {
        perror("error creating directory");
    }
    // input file is at index 1
    // reading the input_file
    if(chmod(argv[1],S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)==-1){
        perror("error setting rw-r--r-- permissions for the input_file");
    }
    int fd_read = open(argv[1], O_RDONLY);
    if (fd_read == -1) {
        perror("error reading input_file");
        exit(EXIT_FAILURE);
    }

    off_t buff_size = lseek(fd_read, 0, SEEK_END);
    if (buff_size == -1) {
        perror("lseek failed for input file");
        exit(EXIT_FAILURE);
    }
    //reseting the file_offset to the begining
    if (lseek(fd_read, 0, SEEK_SET) == -1) {
        perror("lseek failed for input file to reset the file offset to the begining of the file");
        exit(EXIT_FAILURE);
    }
    int size_file = buff_size;
    // FLAG 0
    if ( * (argv[2] + 0) == '0') {
        char * write_pathname;
        write_pathname = get_path_name(argv[1], '0');
        int fd_write = open(write_pathname, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd_write == -1) {
            perror("error opening the output file");
            exit(EXIT_FAILURE);
        }
        int number_of_bytes_written = 0;
        int block_size = string_int_helper(argv[3]);
        if (block_size == -1) {
            cout << "error: invalid block size , must be > 0" << endl;
            exit(EXIT_FAILURE);
        }
        int num_blocks = buff_size / block_size;
        int rem_num_ele = buff_size % block_size;
        int prev = 0;
        for (int i = 0; i < num_blocks; i++) {
            char * read_buff = (char * ) malloc(block_size);
            char * to_write = (char * ) malloc(block_size);
            ssize_t reti = read(fd_read, read_buff, block_size);
            if (reti == -1) {
                perror("read failed for input file");
                exit(EXIT_FAILURE);
            }
            for (int k = 0; k < block_size; k++) {
                to_write[k] = '\0';
            }
            for (int t = 0; t < block_size; t++) {
                to_write[t] = read_buff[block_size - 1 - t];
            }
            int number_of_bytes = write(fd_write, to_write, block_size);
            if (number_of_bytes == -1) {
                perror("error in writing the file ");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            prev = prev + block_size;
            if (lseek(fd_read, prev, SEEK_SET) == -1) {
                perror("lseek failed in setting the file offset to prev");
                exit(EXIT_FAILURE);
            }

            free(read_buff);
            free(to_write);
        }
        if (rem_num_ele > 0) {
            char * read_buff_rem = (char * ) malloc(rem_num_ele);
            char * to_write_rem = (char * ) malloc(rem_num_ele);
            if (read(fd_read, read_buff_rem, rem_num_ele) == -1) {
                perror("error in reading the input file for rem_num_ele flag 0");
                exit(EXIT_FAILURE);
            }
            for (int k = 0; k < rem_num_ele; k++) {
                to_write_rem[k] = '\0';
            }
            for (int t = 0; t < rem_num_ele; t++) {
                to_write_rem[t] = read_buff_rem[rem_num_ele - 1 - t];
            }
            int number_of_bytes = write(fd_write, to_write_rem, rem_num_ele);
            if (number_of_bytes == -1) {
                perror("error in writing the file for rem_num_ele flag 0");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            free(read_buff_rem);
            free(to_write_rem);

        }
        off_t curr_file_offset_write = lseek(fd_write, 0, SEEK_CUR);
        if (curr_file_offset_write == -1) {
            perror("error setting the file offset to -1 after flag 0");
            exit(EXIT_FAILURE);
        }
        free(write_pathname);
        close(fd_write);
        close(fd_read);



    }
    if ( * (argv[2] + 0) == '1') {
        char * write_pathname;
        write_pathname= get_path_name(argv[1], '1');
        int fd_write = open(write_pathname, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd_write == -1) {
            perror("error opening the output file");
            exit(EXIT_FAILURE);
        }
        int number_of_bytes_written = 0;
        int file_chunk = buff_size / 250; //0.4% of the file
        int chunk_size;
        if (file_chunk == 0) {
            chunk_size = 1;
        } else {
            double in_pow_2 = (log((double) file_chunk) / log(2.0));
            int power_2 = (int) in_pow_2;
            chunk_size = pow(2, power_2);
        }
        int number_chunks = buff_size / chunk_size;
        int left_elements = buff_size % chunk_size;
        if (lseek(fd_read, 0, SEEK_SET) == -1) {
            perror("lseek failed to set file offset to 0 flag 1");
            exit(EXIT_FAILURE);
        }
        if (lseek(fd_write, 0, SEEK_SET) == -1) {
            perror("lseek failed to set file offset to 0 flag 1");
            exit(EXIT_FAILURE);
        }
        if (left_elements > 0) {
            char * read_buff = (char * ) malloc(left_elements);
            char * to_write = (char * ) malloc(left_elements);
            int last_ele_off = number_chunks * chunk_size;
            if (lseek(fd_read, last_ele_off, SEEK_SET) == -1) {
                perror("lseek failed to the read file offset to last_ele_off flag 1");
                exit(EXIT_FAILURE);
            }
            if (read(fd_read, read_buff, left_elements) == -1) {
                perror("read failed to the read file flag 1");
                exit(EXIT_FAILURE);
            }
            int i = 0;
            while (i < left_elements) {
                to_write[i] = read_buff[left_elements - 1 - i];
                i++;
            }
            int number_of_bytes = write(fd_write, to_write, left_elements);
            if (number_of_bytes == -1) {
                perror("write failed for left_elements in flag 1");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            free(read_buff);
            free(to_write);
            if (lseek(fd_read, 0, SEEK_SET) == -1) {
                perror("lseek failed for read file to set the offset to 0");
                exit(EXIT_FAILURE);
            }
        }
        int ind = number_chunks - 1;
        while (ind >= 0) {
            char * read_buff = (char * ) malloc(chunk_size);
            char * to_write = (char * ) malloc(chunk_size);
            int offset = ind * chunk_size;
            if (lseek(fd_read, offset, SEEK_SET) == -1) {
                perror("lseek failed for the read file");
                exit(EXIT_FAILURE);
            }
            if (read(fd_read, read_buff, chunk_size) == -1) {
                perror("read failed for the read file");
                exit(EXIT_FAILURE);
            }
            int i = 0;
            while (i < chunk_size) {
                to_write[i] = read_buff[chunk_size - 1 - i];
                i++;
            }
            int number_of_bytes = write(fd_write, to_write, chunk_size);
            if (number_of_bytes == -1) {
                perror("write failed for chunk_size in flag 1");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            free(read_buff);
            free(to_write);
            if (lseek(fd_read, 0, SEEK_SET) == -1) {
                perror("lseek failed for read file to set the offset to 0");
                exit(EXIT_FAILURE);
            }
            ind--;
        }
        free(write_pathname);
        close(fd_write);
        close(fd_read);


    }
    if ( * (argv[2] + 0) == '2') {
        char * write_pathname;
        write_pathname = get_path_name(argv[1], '2');
        int fd_write = open(write_pathname, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd_write == -1) {
            perror("error opening the output file");
            exit(EXIT_FAILURE);
        }
        //reverse the file from 0 to start-1
        //reverse the file from end+1 to end of file
        int number_of_bytes_written = 0;
        int start = string_int_helper(argv[3]);
        if (start == -1) {
            cout << "error: invalid start index , must be > 0" << endl;
            exit(EXIT_FAILURE);
        }
        int end = string_int_helper(argv[4]);
        if (end == -1) {
            cout << "error: invalid end index , must be > 0" << endl;
            exit(EXIT_FAILURE);
        }
        try {
            check_valid_inputs_2(start, end, buff_size);
        } catch (runtime_error & err) {
            cerr << "Error: " << err.what() << endl;
            exit(EXIT_FAILURE);
        }

        //reverse the file from 0 to start-1

        int file_chunk = start / 250; //0.4% of the file //ask this question
        int chunk_size;
        if (file_chunk == 0) {
            chunk_size = 1;
        } else {
            double in_pow_2 = (log((double) file_chunk) / log(2.0));
            int power_2 = (int) in_pow_2;
            chunk_size = pow(2, power_2);
        }
        int number_chunks = start / chunk_size;
        int left_elements = start % chunk_size;
        if (lseek(fd_read, 0, SEEK_SET) == -1) {
            perror("lseek failed read file to set to 0");
            exit(EXIT_FAILURE);
        }
        if (lseek(fd_write, 0, SEEK_SET) == -1) {
            perror("lseek failed write file to set to 0");
            exit(EXIT_FAILURE);
        }
        if (left_elements > 0) {
            char * read_buff = (char * ) malloc(left_elements);
            char * to_write = (char * ) malloc(left_elements);
            int last_ele_off = number_chunks * chunk_size;
            if (lseek(fd_read, last_ele_off, SEEK_SET) == -1) {
                perror("lseek failed read file to set to last_ele_off");
                exit(EXIT_FAILURE);
            }
            if (read(fd_read, read_buff, left_elements) == -1) {
                perror("read failed read file to set for left_elements");
                exit(EXIT_FAILURE);
            }
            int i = 0;
            while (i < left_elements) {
                to_write[i] = read_buff[left_elements - 1 - i];
                i++;
            }
            int number_of_bytes = write(fd_write, to_write, left_elements);
            if (number_of_bytes == -1) {
                perror("write failed for left_elements");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            free(read_buff);
            free(to_write);
            if (lseek(fd_read, 0, SEEK_SET) == -1) {
                perror("lseek failed to set the read file to 0");
                exit(EXIT_FAILURE);
            }
        }
        int ind = number_chunks - 1;
        while (ind >= 0) {
            char * read_buff = (char * ) malloc(chunk_size);
            char * to_write = (char * ) malloc(chunk_size);
            int offset = ind * chunk_size;
            if (lseek(fd_read, offset, SEEK_SET) == -1) {
                perror("lseek failed");
                exit(EXIT_FAILURE);
            }
            if (read(fd_read, read_buff, chunk_size) == -1) {
                perror("read failed");
                exit(EXIT_FAILURE);
            }
            int i = 0;
            while (i < chunk_size) {
                to_write[i] = read_buff[chunk_size - 1 - i];
                i++;
            }
            int number_of_bytes = write(fd_write, to_write, chunk_size);
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            free(read_buff);
            free(to_write);
            if (lseek(fd_read, 0, SEEK_SET) == -1) {
                perror("lseek failed");
                exit(EXIT_FAILURE);
            }
            ind--;
        }
        if (lseek(fd_read, start, SEEK_SET) == -1) {
            perror("lseek failed");
            exit(EXIT_FAILURE);
        }
        // simply from start to end index write it into the text simply
        int mid_file_size = end - start + 1;
        int mid_file_chunk = mid_file_size / 250; //0.2% of the file //ask this question
        int mid_chunk_size;
        if (mid_file_chunk == 0) {
            mid_chunk_size = 1;
        } else {
            double in_pow_2 = (log((double) mid_file_chunk) / log(2.0));
            int power_2 = (int) in_pow_2;
            mid_chunk_size = pow(2, power_2);
        }
        int mid_number_chunks = mid_file_size / mid_chunk_size;
        int mid_left_elements = mid_file_size % mid_chunk_size;
        int itr = 0;
        while (itr < mid_number_chunks) {
            char * read_buff = (char * ) malloc(mid_chunk_size);
            if (read(fd_read, read_buff, mid_chunk_size) == -1) {
                perror("read failed");
                exit(EXIT_FAILURE);
            }
            // write the read_buff in. the text
            int number_of_bytes = write(fd_write, read_buff, mid_chunk_size);
            if (number_of_bytes == -1) {
                perror("write failed");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            // move the read_offset
            int to_set = start + (itr + 1) * mid_chunk_size;
            if (lseek(fd_read, to_set, SEEK_SET) == -1) {
                perror("lseek failed");
                exit(EXIT_FAILURE);
            }
            itr++;
            free(read_buff);
        }
        if (mid_left_elements > 0) {
            char * read_buff = (char * ) malloc(mid_left_elements);
            if (read(fd_read, read_buff, mid_left_elements) == -1) {
                perror("read failed");
                exit(EXIT_FAILURE);
            }
            int number_of_bytes = write(fd_write, read_buff, mid_left_elements);
            if (number_of_bytes == -1) {
                perror("write failed");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            if (lseek(fd_read, end, SEEK_SET) == -1) {
                perror("lseek failed");
                exit(EXIT_FAILURE);
            }
        }


        // need to reverse the last part
        int end_file_size = buff_size - end - 1;
        int end_file_chunk = end_file_size / 250;
        int end_chunk_size;
        if (end_file_chunk == 0) {
            end_chunk_size = 1;
        } else {
            double in_pow_2 = (log((double) end_file_chunk) / log(2.0));
            int power_2 = (int) in_pow_2;
            end_chunk_size = pow(2, power_2);
        }
        int end_number_chunks = end_file_size / end_chunk_size;
        int end_left_elements = end_file_size % end_chunk_size;
        if (end_left_elements > 0) {
            char * read_buff = (char * ) malloc(end_left_elements);
            char * to_write = (char * ) malloc(end_left_elements);
            int last_ele_off = end + 1 + end_number_chunks * end_chunk_size;
            if (lseek(fd_read, last_ele_off, SEEK_SET) == -1) {
                perror("lseek failed");
                exit(EXIT_FAILURE);
            }
            if (read(fd_read, read_buff, end_left_elements) == -1) {
                perror("read failed");
                exit(EXIT_FAILURE);
            }
            int i = 0;
            while (i < end_left_elements) {
                to_write[i] = read_buff[end_left_elements - 1 - i];
                i++;
            }
            int number_of_bytes = write(fd_write, to_write, end_left_elements);
            if (number_of_bytes == -1) {
                perror("write failed");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            free(read_buff);
            free(to_write);
            if (lseek(fd_read, 0, SEEK_SET) == -1) {
                perror("lseek failed");
                exit(EXIT_FAILURE);
            }
        }
        int ind_e = end_number_chunks - 1;
        while (ind_e >= 0) {
            char * read_buff = (char * ) malloc(end_chunk_size);
            char * to_write = (char * ) malloc(end_chunk_size);
            int offset = end + 1 + ind_e * end_chunk_size;
            if (lseek(fd_read, offset, SEEK_SET) == -1) {
                perror("lseek failed");
                exit(EXIT_FAILURE);
            }
            if (read(fd_read, read_buff, end_chunk_size) == -1) {
                perror("read failed");
                exit(EXIT_FAILURE);
            }
            int i = 0;
            while (i < end_chunk_size) {
                to_write[i] = read_buff[end_chunk_size - 1 - i];
                i++;
            }
            int number_of_bytes = write(fd_write, to_write, end_chunk_size);
            if (number_of_bytes == -1) {
                perror("write failed");
                exit(EXIT_FAILURE);
            }
            number_of_bytes_written = number_of_bytes_written + number_of_bytes;
            print_percentage_console(number_of_bytes_written, size_file);
            free(read_buff);
            free(to_write);
            if (lseek(fd_read, 0, SEEK_SET) == -1) {
                perror("lseek failed");
                exit(EXIT_FAILURE);
            }
            ind_e--;
        }
        free(write_pathname);
        close(fd_write);
        close(fd_read);

    }



    return 0;
}