#define launcherBtn vexRT(Btn6D)
#define speedUpBtn vexRT(Btn8R)
#define speedDownBtn vexRT(Btn7L)
#define rpmBtn vexRT(Btn5D)
#define flyEncLeft nMotorEncoder[topLeftLaunch]
#define flyEncRight nMotorEncoder[topRightLaunch]
bool rpmMode = true;
bool launcherOn = false;
bool lastLauncherBtn = false;
static bool speedUp = false;
static bool speedDown = false;

//RPM Calculations
float lastTime;
float ticksPerRev = 392;
float launcherRatio = 9.8;

//Target RPMs and Motor Values
int index = 0;
int rpmIndex = 0;
int motorValues[3]= {0, 40, 80};
int rpmValues[3] = {0, 1090, 1600};
float currentSpeed = 0.0;
float currentRpm = 0.0;

//TBH Constants
float Kg = 0.0;
float KgLow = 0.00016500;
float KgHigh = 0.057;

float leftDrive;
float rightDrive;
float leftDriveZero;
float rightDriveZero;
long crossLeft;
long crossRight;
float leftDriveApprox;
float rightDriveApprox;

//TBH Calculation Variables
float rpmLeft;
float rpmRight;

float leftError;
float rightError;
float lastErrorLeft;
float lastErrorRight;

//Launcher Power
float powerLeft = 0.0;
float powerRight = 0.0;

void setFlywheels(float left, float right)
{
	int l = round(left);
	int r = round(right);
	if (l < 0)
		l *=-1;
	if (r < 0)
		r *=-1;
	motor[btmLeftLaunch] = l;
	motor[topLeftLaunch] = l;
	motor[btmRightLaunch] = r;
	motor[topRightLaunch] = r;
}

void setTBHConstants()
{
	if (currentRpm == rpmValues[2])
	{
		Kg = KgHigh;
	}
	if (currentRpm == rpmValues[1])
	{
		Kg = KgLow;
	}
}

void TBHlaunch(float target)
{
	float deltaTime = abs(nSysTime - lastTime);
	float rpmConversion = ((launcherRatio * 60000) / deltaTime) / ticksPerRev;
	rpmLeft = abs(flyEncLeft * rpmConversion);
	rpmRight = abs(flyEncRight * rpmConversion);

	leftError = target - rpmLeft;
	rightError = target - rpmRight;

	// Use Kp as gain
	leftDrive =  leftDrive + (leftError * Kg);
	rightDrive = rightDrive + (rightError *Kg);

	// Clip - we are only going forwards
	if(leftDrive > 1)
		leftDrive = 1;
	if(leftDrive < 0)
		leftDrive = 0;

	if(rightDrive > 1)
		rightDrive = 1;
	if(rightDrive < 0)
		rightDrive = 0;

	// Z-cross LEFT
	if(sgn(leftError) != sgn(lastErrorLeft)) {
		// First zero crossing after a new set velocity command
		if(crossLeft) {
			// Set drive to the open loop approximation
			leftDrive = leftDriveApprox;
			crossLeft = 0;
		}
		else
			leftDrive = 0.5 * (leftDrive + leftDriveZero);

		// Save this drive value in the "tbh" variable
		leftDriveZero = leftDrive;
	}

	// Z-cross LEFT
	if(sgn(rightError) != sgn(lastErrorRight)) {
		// First zero crossing after a new set velocity command
		if(crossRight) {
			// Set drive to the open loop approximation
			rightDrive = rightDriveApprox;
			crossRight = 0;
		}
		else
			rightDrive = 0.5 * (rightDrive + rightDriveZero);

		// Save this drive value in the "tbh" variable
		rightDriveZero = rightDrive;
	}

	powerLeft = (leftDrive * 127) + 0.5;
	powerRight = (rightDrive * 127) + 0.5;

	// Save last error
	lastErrorLeft = leftError;
	lastErrorRight = rightError;
		flyEncLeft = 0;
	flyEncRight = 0;
	lastTime = nSysTime;

	wait1Msec(25);
}

task flywheelSpeedSelector()
{
	if (rpmMode) {
		rpmIndex++;
		if (rpmIndex > 2) {
			rpmIndex = 0;
		}
		currentRpm = rpmValues[rpmIndex];
		powerLeft = motorValues[rpmIndex];
		powerRight = motorValues[rpmIndex];
	}
	else {
		index++;
		if (index > 2) {
			index = 0;
			launcherOn = false;
			lastLauncherBtn = false;
		}
		currentSpeed = motorValues[index];
		setFlywheels(motorValues[index],motorValues[index]);
	}
	wait1Msec(500);
}

task flywheelSpeedAdjuster()
{
	if (rpmMode) {
		if (speedUp) {
			currentRpm += 20;
		}
		else if (speedDown){
			currentRpm -= 20;
		}
	}
	else {
		if (speedUp) {
			currentSpeed += 2;
			setFlywheels(currentSpeed, currentSpeed);
			speedUp = false;
		}
		else if (speedDown) {
			currentSpeed -= 2;
			setFlywheels(currentSpeed, currentSpeed);
			speedDown = false;
		}
	}
	wait1Msec(500);
}

task autoFlyWheel() {
	motor[intake] = 127;
	setFlywheels(80, 80);
	while(true) {
		TBHlaunch(1650);
	}
}
