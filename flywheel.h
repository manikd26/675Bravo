#define launcherBtn vexRT(Btn6D)
#define speedUpBtn vexRT(Btn8R)
#define speedDownBtn vexRT(Btn7L)
#define rpmBtn vexRT(Btn5D)
#define flyEncLeft nMotorEncoder[topLeftLaunch]
#define flyEncRight nMotorEncoder[topRightLaunch]
bool rpmMode = false;
bool launcherOn = false;
bool lastLauncherBtn = false;
static bool speedUp = false;
static bool speedDown = false;

//PINGAS
int index = 0;
int rpmIndex = 0;
int motorValues[4]= {0, 80, 60, 40};
int rpmValues[4] = {0, 1500, 700, 500};

//RPM Calculations
float lastTime;
float ticksPerRev = 392;
float launcherRatio = 9.8;

//Target RPMs and Motor Values
float rpmHigh = 2000;
float rpmMid = 1500;
float rpmLow = 1000;
float rpmGoal;
float highSpeed = 80;
float midSpeed = 65;
float lowSpeed = 50;
float currentSpeed = 0.0;
float currentRpm = 0.0;

//PID Constants
float Kp = 0.01;
float Ki = 0.00000;
float Kd = 0.0;
float KpL = Kp;
float KiL = Ki;
float KdL = Kd;
float KpR = Kp;
float KiR = Ki;
float KdR = Kd;

float rpmLeft;
float rpmRight;

float leftError;
float rightError;
float lastErrorLeft;
float lastErrorRight;

float pLeft;
float pRight;
float intRawLeft;
float intRawRight;
float intLeft;
float intRight;
float dLeft;
float dRight;

float integralActiveZone = 0; //value of rpm after which proportion stops taking effect

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
void PIDlaunch(float target)
{
	float deltaTime = abs(nSysTime - lastTime);
	float rpmConversion = ((launcherRatio * 60000) / deltaTime) / ticksPerRev;
	rpmLeft = abs(flyEncLeft * rpmConversion);
	rpmRight = abs(flyEncRight * rpmConversion);


	leftError = target - rpmLeft;
	rightError = target - rpmRight;

	//Proportion
	pLeft = KpL*leftError;
	pRight = KpR*rightError;

	//Integral
	if (abs(leftError) < integralActiveZone && leftError != 0)
	{
		intRawLeft = intRawLeft + leftError;
	}
	else //if error is so big and P is still taking effect, raw = 0
	{
		intRawLeft = 0;
	}

	if (abs(rightError) < integralActiveZone && rightError != 0)
	{
		intRawRight = intRawRight + rightError;
	}
	else
	{
		intRawRight = 0;
	}

	intLeft = KiL * intRawLeft;
	intRight = KiR * intRawRight;


	//Derivative
	dLeft = KdL*(leftError - lastErrorLeft);
	dRight = KdR*(rightError - lastErrorRight);
	lastErrorLeft = leftError;
	lastErrorRight = rightError;

	if (leftError == 0)
	{
		dLeft = 0;
	}
	if (rightError == 0)
	{
		dRight = 0;
	}


	//Final Power
	powerLeft += pLeft + intLeft + dLeft; //should be P+I+D
	powerRight += pRight + intRight + dRight;

	lastErrorLeft = leftError;
	lastErrorRight = rightError;
	flyEncLeft = 0;
	flyEncRight = 0;
	lastTime = nSysTime;

	wait1Msec(20);
}
task flywheelSpeedSelector()
{
	if (rpmMode) {
		currentRpm = rpmValues[rpmIndex];
		powerLeft = motorValues[rpmIndex];
		powerRight = motorValues[rpmIndex];
		//PIDlaunch(currentRpm);
		//setFlywheels(powerLeft, powerRight);
		rpmIndex++;
		if (rpmIndex > 3) {
			rpmIndex = 0;
		}
	}
	else {
		currentSpeed = motorValues[index];
		setFlywheels(motorValues[index],motorValues[index]);
		index++;
		if (index > 3) {
			index = 0;
			launcherOn = false;
			lastLauncherBtn = false;
		}
	}
	wait1Msec(500);
}

task flywheelSpeedAdjuster()
{
	if (rpmMode) {
		if (speedUp) {
			currentRpm += 20;
			//PIDlaunch(currentRpm);
			//setFlywheels(powerLeft, powerRight);
		}
		else {
			currentRpm -= 20;
			PIDlaunch(currentRpm);
			setFlywheels(powerLeft, powerRight);
		}
	}
	else {
		if (speedUp) {
			currentSpeed += 2;
			setFlywheels(currentSpeed, currentSpeed);
			speedUp = false;
		}
		if (speedDown) {
			currentSpeed -= 2;
			setFlywheels(currentSpeed, currentSpeed);
			speedDown = false;
		}
	}
	wait1Msec(500);
}
