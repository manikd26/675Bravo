#define intakeBtn vexRT(Btn7R)
#define intakeOutBtn vexRT(Btn7U)
#define conveyorBtn vexRT(Btn6U)
#define conveyorOutBtn vexRT(Btn5U)

bool intakeOn = true;
bool lastIntakeBtn = false;

task rollerIntake()
{
	if (intakeBtn && !lastIntakeBtn)
	{
		intakeOn = !intakeOn;
		lastIntakeBtn = true;
	}

	if (intakeBtn == 0)
		lastIntakeBtn = false;



	if (intakeOutBtn == 1)
		motor[intake] = -127;
	else
		motor[intake] = intakeOn * 127;
}

task conveyorBelt()
{
	if (conveyorBtn)
		motor[conveyor] = 95;
	else if (conveyorOutBtn)
		motor[conveyor] = -127;
	else
		motor[conveyor] = 0;
}
