#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  flywheel,       sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           rightFront,    tmotorVex393HighSpeed_HBridge, openLoop)
#pragma config(Motor,  port2,           leftFront,     tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           leftBack,      tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           conveyor,      tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           intake,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           leftLauncher,  tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           rightLauncher, tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port10,          rightBack,     tmotorVex393HighSpeed_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!

#include "drive.h"
#include "intakeAndConveyor.h"
#include "flywheelWorldTBH.h"

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////

void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task autonomous()
{
	// .....................................................................................
	// Insert user code here.
	// .....................................................................................

	AutonomousCodePlaceholderForTesting();  // Remove this function call once you have "real" code.
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task usercontrol()
{
	// User control code here, inside the loop

	while (true)
	{
				if (rpmBtn)
		{
			rpmMode = !rpmMode;
			rpmIndex = 0;
			index = 0;
			wait1Msec(500);
		}

		if(getTaskState(flywheelSpeedSelector) == taskStateStopped && launcherBtn)
			startTask(flywheelSpeedSelector);

		if(getTaskState(flywheelSpeedAdjuster) == taskStateStopped && (speedUpBtn || speedDownBtn)) {
			if (speedUpBtn) {
				speedUp = true;
			}
			else {
				speedDown = true;
			}
			startTask(flywheelSpeedAdjuster);
			wait1Msec(100);
		}

		if(rpmMode) {
			if (currentRpm != 0) {
				setTBHConstants();
				TBHlaunch(currentRpm);
			}
			else {
				flywheelPower = 0.0;

			}
			setFlywheels(flywheelPower);
		}


		if (getTaskState(driveTask) == taskStateStopped)
			startTask(driveTask);
		if (getTaskState(rollerIntake) == taskStateStopped)
			startTask(rollerIntake);
		if (getTaskState(conveyorBelt) == taskStateStopped)
			startTask(conveyorBelt);


		// This is the main execution loop for the user control program. Each time through the loop
		// your program should update motor + servo values based on feedback from the joysticks.

		// .....................................................................................
		// Insert user code here. This is where you use the joystick values to update your motors, etc.
		// .....................................................................................

		UserControlCodePlaceholderForTesting(); // Remove this function call once you have "real" code.
	}
}
