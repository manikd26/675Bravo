#define launcherBtn vexRT(Btn6D)
#define flyEncLeft nMotorEncoder[topLeftLaunch]
#define flyEncRight nMotorEncoder[topRightLaunch]

bool launcherOn = false;
bool lastLauncherBtn = false;

int index = 0;
int motorValues[4]= {0, 80, 60, 40};

//RPM Calculations
float lastTime;
float ticksPerRev = 392;
float launcherRatio = 9.8;

//Target RPMs and Motor Values
float rpmHigh = 2000;
float rpmMid;
float rpmLow;
float rpmGoal;
float highSpeed = 80;
float midSpeed = 65;
float lowSpeed = 50;

//PID Constants
float Kp = 0.025;
float Ki = 0;
float Kd = 0;
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

float powerLeft = 60.0;
float powerRight = 60.0;

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

task flywheelSpeedAdjuster()
{
setFlywheels(motorValues[index],motorValues[index]);
		index++;
		if (index > 3) {
		    	 index = 0;
		    	 launcherOn = false;
		    	 lastLauncherBtn = false;
    }
    wait1Msec(1000);
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
	powerLeft = pLeft + intLeft + dLeft; //should be P+I+D
	powerRight = pRight + intRight + dRight;

	lastErrorLeft = leftError;
	lastErrorRight = rightError;
	lastTime = nSysTime;

	wait1Msec(20);
}
