Instructions for Running Program 4

1. Unzip the assignment into your preferred directory

2. Navigate to the folder containing the unzipped project files: owensas_program4

3. Compile the program by entering the following command:
    gcc --std=gnu99 -pthread -g -Wall -o line_processor main.c

4. Run the program by entering the following command:       
    ./line_processor

Note: You will not receive a prompt but are expected to enter up to 50
lines of input, each of length 1000.   

5. You can also run the program by redirecting input and/or output such as:
    ./line_processor < input1.txt
    ./line_processor < input1.txt > output1.txt
