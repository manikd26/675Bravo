#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, dgtl1,  liftDeploy,     sensorDigitalOut)
#pragma config(Sensor, I2C_1,  rightFlywheel,  sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  leftFlywheel,   sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           leftBack,      tmotorVex393HighSpeed_HBridge, openLoop, reversed, driveLeft)
#pragma config(Motor,  port2,           leftFront,     tmotorVex393HighSpeed_MC29, openLoop, driveLeft)
#pragma config(Motor,  port3,           rightFront,    tmotorVex393HighSpeed_MC29, openLoop, reversed, driveRight)
#pragma config(Motor,  port4,           intake,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           topRightLaunch, tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port6,           btmRightLaunch, tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           btmLeftLaunch, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           topLeftLaunch, tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port9,           conveyor,      tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port10,          rightBack,     tmotorVex393HighSpeed_HBridge, openLoop, reversed, driveRight)

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"
#include "flywheel.h"
#include "drive.h"
#include "intakeAndConveyor.h"

void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}

task autonomous()
{
	// .....................................................................................
	// Insert user code here.
	// .....................................................................................

	AutonomousCodePlaceholderForTesting();  // Remove this function call once you have "real" code.
}

task usercontrol()
{
	// User control code here, inside the loop

	while (true)
	{
		if(getTaskState(flywheelSpeedAdjuster) == taskStateStopped && launcherBtn)
			startTask(flywheelSpeedAdjuster);
		if (getTaskState(drive) == taskStateStopped)
			startTask(drive);
		if (getTaskState(rollerIntake) == taskStateStopped)
			startTask(rollerIntake);
		if (getTaskState(conveyorBelt) == taskStateStopped)
			startTask(conveyorBelt);
	}
}
