# Robotics Assessment
The project is made up of one program for controlling the Zumo Robot around a maze to find given objects. 

## Setting up the Zumo Robot 
1. Open the Zumo application into Visual Studio Code or your Ardunio IDE
2. Ensure that Zumo32U4 Library is installed
3. Connect the Zumo Robot to the PC via USB
4. Press the Upload button to run the given code

## Running the Zumo Robot 
1. Turn the Zumo on to activate the Robot
2. Press the "A" button to operate the Robot for calibration
3. The Robot will begin to navigate around the Maze

## Pause the Zumo Robot
1. Press the "A" button
2. The Zumo Robot will paused and pressing the "A" button again will re-activate the Zumo Robot.

## Detecting an Object 
Once the Zumo as detected an Object, the Zumo will stop moving and play a sound informing you it has detected a Parcel. The Zumo will be paused for 4 seconds and the **detected Parcel must be removed from the Maze**. After 4 seconds, the Zumo will be begin to move around the Maze again. 
