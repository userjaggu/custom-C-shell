# foreground and background process
    background process doesn't run properly
    example lets say when a command sleep 10& is given at the same time a command is given to change the directory then after 10 seconds the directory will still be in the previous directory only it doesn't changes to new path

# reveal
    In the reveal it doesn't deal with the process whether it runs in foreground or background i.e, + or -

# I/O redirection and pipes
    I have assumed the case where only '<' or '>' or '>>' occurs i.e, only single comamnd input same is the case with pipes when more than 1 pipe occured then it fails to get executed

# echo
    whenever the user provide a text with echo command it takes everything starting after the echo command.
    i.e, if i give a command:
                echo "jaggu" >> jaggu.txt
        then the print contains "jaggu" >> jaggu.txt
    
# activities
    I have sorted all the processes based on their commands alphabetically but not according to their PIDs 
    It not only shows if the process is Running or stopped but also shows if it gets terminated or not


