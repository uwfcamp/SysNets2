//Curtis Duvall, Nathan Wilkins, and Julian Thayer
//Systems and Networking II
//Project 2
//readme.txt

\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\CONTENTS\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\\  I.    -   SETUP AND COMPILATION                                                          \\
\\  II.   -   RUNNING THE PROGRAM                                                            \\
\\  III.  -   USER INPUT                                                                     \\
\\  IV.   -   USER OUTPUT                                                                    \\
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

I. SETUP AND COMPILATION

  1.  Download and unzip the submission folder from eLearning onto a Linux testing environtment
  
  2.  This sumbission should contain the following:
    - client_framework.c
    - client_framework.h
    - c_admin_funct.c
    - c_admin_funct.h
    - c_login_funct.c
    - c_login_funct.h
    - c_menu_funct.c
    - c_menu_funct.h
    - server_framework.c
    - server_framework.h
    - s_admin_funct.c
    - s_admin_funct.h
    - s_login_funct.c
    - s_login_funct.h
    - s_menu_funct.c
    - s_menu_funct.h
    - makefile
    - parse.h
    - parse.c
    - Definitions.h
    - readme.txt (this file)
    
  3.  Environment: This program was developed and tested in Linux. It was also tested in the
      multiplatform environment.
      
  4.  Compiling: This program contains a makefile. At the command line in Linux omitting the
      quotes("), type the command "make". This command will produce two executable files named
      "server" and "client". "server" is an executable that runs the server side of the project 
      description and "client" is an executable that runs the client side of the project 
      description. After successful compilation. Put the client executable in a separate 
      directory from the server(This will be mentioned a few times to make sure this is done).


II. RUNNING THE PROGRAM

  1.  server: Begin the server side by executing the command "./server" in the directory the
      server executable is located in.
      
  2.  client: Before execution make sure your client is in a different directory than the
      server is in. Begin the client side by executing the command "./client" in the directory
      the client executable is located in.
      
III. USER INPUT

  1.  server: This process does not require any input files to run. Optionally the user can
      provide a logins.txt formatted appropriately with username, password, and a 0 for client
      not being banned or 1 for a client be banned separated by the ascii character that has
      the decimal value of DELIMITER in the Definitions.h file. On startup of the program, the user will be prompted to enter
      a username and password for the admin account on the server.
  
  2.  client: This process does not require any input files to run. The program will take
      user input based on requirements in the sheet. In most cases, there is an exit case for
      loops are either the letter 'q' or the string "_q". The specific case required will be 
      shown in each loop. For the file transfer to work, the client executable must be in a
      different directory than the server with the file to transfer residing in the clients
      directory.

IV. OUTPUT

  1.  server: Upon startup, the server will prompt the user to input the password and username
      for the server administrator. Then the server will print each transition, request and 
      ping to and from clients to the screen. Through execution of the program, a logins.txt,
      groupchat.txt, privatechat.txt, filelist.txt are created.
      
      a.  logins.txt: This files contains all of the usernames, passwords, and the details
          signifying which users are banned. These details are shown by line with each value
          separated by the ascii character with the decimal value of DELIMITER in the Definitions.h file. for the isbanned
          detail. a 0 signifies the user is not banned and a 1 signifies that the user is
          banned.
          
      b.  groupchat.txt: This file is the repository for all group communication. It contains
          all of the group chat logs in a running chat log. With a timestamp followed by the
          user who the broadcast came from preceeding the message. The maximum size that this
          file can be for the program to function properly is the size of BUFFER_SIZE ascii
          characters as specified in the Definitions.h file.
          
      c.  privatechat.txt: This file is the repository of all private communication. It
          it contains the timestamp a private message was sent, the sender, the reciever, then
          the actual message. All of these values are a unique line for each message and each
          value is separated by the delimiter ascii char with the decimal value of DELIMITER
          in the Definitions.h file.
      
      d.  filelist.txt: This file is a repository the reciepient of the file, the size of the
          file, and the name of the file with each value separated by the ascii character of
          the decimal value DELIMITER as specified in the Definitions.h file.

  2.  client: Ensure that the client executable is moved to a separate directory from the server 
      executable. Upon execution the client will prompt the user to select what it wants to do 
      based off of its menu options. The output will be based off of the requirements in 
      the project specifications sheet. The client interface is broken into several context menues
      which relate to the operation of the client side application.
      
      a.  Login Screen: this context mode relates to the creation and authentication of users for 
          the chat system. 
          1-Registration- defines a new user record and corresponding password stored in logins.txt
          on the server side. Upon successful registration the new user is automatically logged in
          to the system. 
          2-Login- is used to enter as an existing registered user found inlogins.txt. This will fail
          if the username given does not exist; the password does not match the recorded value, or 
          if the selected user account is banned by the server administrator.
          0-Quit- from this menu, selecting quit will close the client application returning the window 
          to the system commandline.
  
      b.  Main Menu: this is the primary interface for the client. The main menu acts as a hub connecting 
          all other context specific menues in the client application. All user, chat, history, file 
          and administrative functions are reached from this menu.
          
          1. View current online number- this action polls the server which prints out the list of 
          active users to the client screen.
          
          2. Enter the group chat- this submenu evokes the broadcast chat functionality of the 
          application. A message greets the user prompting to continue to the chat propper or 'Q' 
          to return to the main menu. Hitting enter at this screen will proceed to the active chat 
          window a new message explains that the escape sequence to quit the chat is "_q". The user 
          can type a single message followed by the enter key. Incoming messages from other users 
          will also post to this screen with the username of the sender. Hitting enter again will 
          ready the screen for a new message, else hitting 'Q' will return to the main menu.
          
          3. Enter the private chat- this submenu evokes the one-to-one private chat functionality 
          of the application. A message greets the user prompting to continue to the chat propper 
          or 'Q' to return to the main menu. Hitting enter at this screen will print the list of 
          current online users before prompting the user to enter one as a destination for the 
          message. when a valid target is identified proceed to the active chat window a new message 
          explains that the escape sequence to quit the chat is "_q". The user can type a single 
          message followed by the enter key. Incoming messages from other users will also post to 
          this screen with the username of the sender. Hitting enter again will return the user to 
          the recipient select prompt, else hitting 'Q' will return to the main menu.
          
          4. View chat history- this submenu prompts the user to choose between the following:
            1. Group Chat- will immediately print the public chat log to the terminal window. 
            2. Private Chat- will print all registered users to the screen and prompt the user to 
            select one to view the private conversation. If no private conversation exists between 
            those users, the prompt will notify to that fact. After the chat prints to the screen 
            the prompt returns to the chat history submenu.
            0. Quit- returns the focus to the main menu.
            
          5. File transfer- this submenu prompts the user to choose between the following: 
            1. Send a file- will immediately print the list of current active users to the terminal 
            window. the window will then prompt the user to select one as the destination, followed 
            by a prompt for the filename of the file to opload. 
            2. Recieve a file- will notify the user of any files addressed to themself and prompt 
            the user to select one for download. If no file exists for that user hitting enter will 
            returnt he user to the chat history submenu. 
            0. Quit- returns the focus to the main menu.
            
          6. Change the password- this function prompts the user for their current password for 
          re-authentication, followed by the desired new password entered twice for validation.
          
          7. Logout- this action safely logs the user out of the server and returns the interface 
          to the login screen.
          
          8. Administrator- this submenu prompts the user to authenticate with an administrative 
          account created by the server. Once authenticated the administrator can choose between: 
            1. Ban a member- will immediately print the list of registered users to the terminal 
            window. The window will then prompt the user to select one to be banned. 
            2. Kick a member- will immediately print the list of registered users to the terminal 
            window. The window will then prompt the user to select one to be kicked. 
            0. Exit to Main Menu- returns the focus to the main menu.
