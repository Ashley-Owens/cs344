Instructions for Running Program 5: One Time Pads

1. Unzip the assignment into your preferred directory.      

2. Navigate to the folder containing the unzipped project files: `cd owensas_program5`           

3. Compile the program by running the compileall script:         
    `bash ./compileall` or `./compileall`         
Note: you may need to set execute permissions on the script `chmod +x ./compileall`       

4. Test the program by running the testing script:       
    `./p5testscript <port number> <port number>`       
Note: you may need to set execute permissions on the script `chmod +x ./p5testscript`       

5. You can also run the program by redirecting testing output such as:
    `./p5testscript 59876 56897 > mytestresults 2>&1`         
