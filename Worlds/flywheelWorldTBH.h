#define launcherBtn vexRT(Btn6D)
#define speedUpBtn vexRT(Btn8R)
#define speedDownBtn vexRT(Btn7L)
#define rpmBtn vexRT(Btn5D)
#define flyEnc nMotorEncoder[rightLauncher]
bool rpmMode = true;
bool launcherOn = false;
bool lastLauncherBtn = false;
static bool speedUp = false;
static bool speedDown = false;

//RPM Calculations
float lastTime;
float ticksPerRev = 392;

//Target RPMs and Motor Values
int index = 0;
int rpmIndex = 0;
int motorValues[3]= {0, 52, 82};
int rpmValues[3] = {0, 95, 150}; //1450 for preloads
float currentSpeed = 0.0;
float currentRpm = 0.0;

//TBH Constants
float Kg = 0.0;
float KgLow = 0.00006; //0.00006
float KgHigh = 0.0001; //(0.041/1450) or (0.0001/1500)

float drive;
float driveZero;
long cross;
float driveApprox;

//TBH Calculation Variables
float rpm;

float error;
float lastError;

//Launcher Power
float flywheelPower = 0.0;

void setFlywheels(float power)
{
	int pow = round(power);
	if (pow < 0)
		pow *=-1;
	motor[leftLauncher] = pow;
	motor[rightLauncher] = pow;

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
	float rpmConversion = ((60000) / deltaTime) / ticksPerRev;
	rpm = abs(flyEnc * rpmConversion);

	error = target - rpm;

	// Use Kp as gain
	drive =  drive + (error * Kg);


	// Clip - we are only going forwards
	if (drive > 1)
		drive = 1;
	if(drive < 0)
		drive = 0;

	// Z-cross LEFT
	if(sgn(error) != sgn(lastError)) {
		// First zero crossing after a new set velocity command
		if(cross) {
			// Set drive to the open loop approximation
			drive = driveApprox;
			cross = 0;
		}
		else
			drive = 0.5 * (drive + driveZero);

		// Save this drive value in the "tbh" variable
		driveZero = drive;
	}

	flywheelPower = (drive * 127) + 0.5;

	// Save last error
	lastError = error;
	flyEnc = 0;
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
		flywheelPower = motorValues[rpmIndex];
	}
	else {
		index++;
		if (index > 2) {
			index = 0;
			launcherOn = false;
			lastLauncherBtn = false;
		}
		currentSpeed = motorValues[index];
		setFlywheels(motorValues[index]);
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
			setFlywheels(currentSpeed);
			speedUp = false;
		}
		else if (speedDown) {
			currentSpeed -= 2;
			setFlywheels(currentSpeed);
			speedDown = false;
		}
	}
	wait1Msec(500);
}

task autoFlyWheel() {
	motor[intake] = 127;
	setFlywheels(82);
	while(true) {
		setTBHConstants();
		TBHlaunch(rpmValues[2]);
	}
}
