# Chess Web Application
CS 3307: Group 2

Members: Bradley David Reese McGlynn, Christopher Brent Hosang, Riley Emma Gavigan, Satvir Singh Uppal, Nicholas Raymond Corcoran

## Running the Application
It's recommended that you run the application on Gaul and connect through an SSH tunnel for the port you select.
```sh
# Build the application
make clean && make

# Run the application on the desired port
./main_app --docroot . --http-listen 0.0.0.0:<port-number>

# Alternatively, just use the shell script provided to do everything in one line
./run.sh <port>
```
Use any web browser to connect to http://127.0.0.1:port-number to play the PiChess application.

## Playing the Application Online
Navigate to [chess.rileygavigan.com](https://chess.rileygavigan.com)

## Gaul Connection Instructions
```sh
# In a terminal window on your local computer - establish the tunnel
ssh -N -L <port-number>:cs3307.gaul.csd.uwo.ca:<port-number> <uwo-alias>@cs3307.gaul.csd.uwo.ca
```

## Testing the Application
```sh
# Build the application for testing
make clean && make runtest

# Run the test application executable to view unit test results
./test_main
```

## Testing User Interface
There is a test account registered in the database that you can use:
* Username: test
* Password: test

## Generating Documentation
Doxygen documentation can be generated and exported by running the following command.
```sh
make doc
```

To view the documentation, open `./docs/html/index.html`