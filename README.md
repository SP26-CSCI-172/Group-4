\# Tech Transformers - Final Robot Project



\## Problem Statement



For our final project, our group made a robot that can work in a few different modes instead of only doing one thing. The main idea was to make a small patrol/search robot using the Osoyoo robot kit. It can drive around, avoid objects, follow something in front of it, act like a guard alarm, and also follow a line. This is supposed to be like a simple real-world robot that could move around an area and react to what it sees with its sensors.



\## Sensors We Used



Sensor / Part, Why we used it



Ultrasonic sensor, To measure distance in front of the robot and tell if something is too close.

Servo motor, To turn the ultrasonic sensor left and right so the robot can scan around.

Left and right IR obstacle sensors, To help detect objects on the sides/front of the robot.

5-channel line tracker, To follow a line and also detect floor markers.

IR remote receiver, To control the robot and switch between the different modes.

Buzzer, To make sounds for warnings, mode changes, startup, and the alarm mode.



\## Pin Map



| Pin | Part |



| D2 | Left IR obstacle sensor |

| D3 | Motor driver ENA |

| D4 | Right IR obstacle sensor |

| D5 | Ultrasonic trigger |

| D6 | Motor driver ENB |

| D7 | Motor driver IN3 |

| D8 | Motor driver IN4 |

| D9 | Servo motor |

| D10 | IR remote receiver |

| D11 | Motor driver IN2 |

| D12 | Motor driver IN1 |

| D13 | Buzzer |

| A0 | Line tracker S1 |

| A1 | Line tracker S2 |

| A2 | Line tracker S3 |

| A3 | Line tracker S4 |

| A4 | Line tracker S5 |

| A5 | Ultrasonic echo |



\## Code Overview



The code is all in one Arduino file:



`Group\_4\_Robot\_Code\_Final\_Project.ino` | Main code for the robot. It has the pin setup, remote controls, motor functions, sensor readings, buzzer sounds, and all the robot modes.



The robot is controlled using modes. The mode changes when a button is pressed on the IR remote.



| Mode | Button | What happens |



| Stopped | OK | Stops the robot. |

| Manual drive | 1 | Lets the user drive with the arrow buttons. |

| Patrol | 2 | Robot drives on its own and tries to avoid obstacles. |

| Follow | 3 | Robot tries to follow an object in front of it. |

| Guard | 4 | Robot stays still and sounds the buzzer if it detects something. |

| Line follow | 5 | Robot follows a line using the line tracker. |



Some important functions in the code are:



\- `setup()` sets up the pins, servo, IR receiver, and runs a startup test.

\- `loop()` keeps checking the remote and runs whatever mode the robot is in.

\- `handleRemote()` reads the remote buttons.

\- `patrolMode()` is the autonomous driving mode.

\- `followMode()` makes the robot follow an object.

\- `guardMode()` makes the alarm behavior.

\- `lineFollowMode()` reads the line tracker and adjusts the motors.

\- `readDistanceCM()` gets the ultrasonic distance.

\- `avoidObstacleWithScan()` turns the servo and checks which direction is more open.

\- `startupSelfTest()` moves the servo and prints sensor values so we can check if things are working.



\## Innovation Features



For the innovation part, we added a few things that make the robot feel more complete:



\- The IR remote can switch between several modes.

\- The buzzer makes different sounds for startup, warnings, mode changes, and alarm mode.

\- The robot has a startup self-test so we can see if the sensors are responding.

\- The ultrasonic sensor is on a servo, so the robot can scan left and right before turning.

\- Manual mode still checks for obstacles before driving forward.

\- Line following remembers the last direction it saw the line, which helps when it loses the line for a moment.

\- Guard mode uses the sensors like a simple security robot.



\## Challenges and How We Fixed Them



One challenge was getting all the sensors to work together without the code becoming too confusing. We fixed this by splitting the code into different modes and helper functions.



Another challenge was obstacle avoidance. At first, only checking straight ahead was not very useful because the robot did not know where to turn. Mounting the ultrasonic sensor on the servo helped because now it can check left and right.



The line follower was also not perfect. Sometimes it lost the line during turns. We added code that remembers the last direction of the line so the robot can try to turn back toward it.



We also had to make the robot easier to test during the demo. The remote helped a lot because we could quickly stop the robot or switch modes without re-uploading code.



\## Members



Carter Saladin

Emily Oulton

Ethan Martez

Jack Harrington

Keira Varela

Trish Huynh



\## How to Run the Robot



1\. Build the Osoyoo robot with the Arduino, motor driver, motors, sensors, servo, IR receiver, and buzzer.

2\. Connect the wires using the pin map above.

3\. Open the Arduino IDE.

4\. Make sure the `Servo` and `IRremote` libraries are installed.

5\. Open `Group\_4\_Robot\_Code\_Final\_Project.ino`.

6\. Select `Arduino Uno` as the board.

7\. Select the correct USB port.

8\. Upload the code.

9\. Open the Serial Monitor at 9600 baud if you want to see sensor readings.

10\. Put the robot on the floor or test area.

11\. Turn on the robot power.

12\. Use the remote to pick a mode.



\## IR Remote Controls



| Button | Action |



| OK | Stop |

| 0 | Horn |

| 1 | Manual drive |

| 2 | Patrol mode |

| 3 | Follow mode |

| 4 | Guard mode |

| 5 | Line follow mode |

| Up | Forward in manual mode |

| Down | Backward in manual mode |

| Left | Turn left in manual mode |

| Right | Turn right in manual mode |

| Star | Sensor demo |

| Hash | Center the servo |



\## Demo Notes



Before the final demo, we should check:



\- The robot turns on and does the startup test.

\- The buzzer works.

\- The remote can stop the robot.

\- Manual driving works.

\- Patrol mode avoids objects.

\- Follow mode reacts to an object in front.

\- Guard mode makes the alarm sound.

\- Line follow mode works on the test line.

\- We have our backup video ready just in case the robot has problems.



