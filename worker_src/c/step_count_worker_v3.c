#include <pebble_worker.h>
// #include <pebble.h>
#include <math.h>
// Total Steps (TS)
#define TS 1
// Total Steps Default (TSD)
#define TSD 1
// Timer used to determine next step check
static AppTimer *timer;
// interval to check for next step (in ms)
const int ACCEL_STEP_MS = 475;
// value to auto adjust step acceptance
const int PED_ADJUST = 2;
// steps required per calorie
const int STEPS_PER_CALORIE = 22;
// value by which step goal is incremented
const int STEP_INCREMENT = 50;
// values for max/min number of calibration options
const int MAX_CALIBRATION_SETTINGS = 3;
const int MIN_CALIBRATION_SETTINGS = 1;

int X_DELTA = 35;
int Y_DELTA, Z_DELTA = 185;
int YZ_DELTA_MIN = 175;
int YZ_DELTA_MAX = 195;
int X_DELTA_TEMP, Y_DELTA_TEMP, Z_DELTA_TEMP = 0;
int lastX, lastY, lastZ, currX, currY, currZ = 0;
int sensitivity = 1;

long stepGoal = 0;
long pedometerCount = 0;
long caloriesBurned = 0;
long tempTotal = 0;

bool did_pebble_vibrate = false;
bool validX, validY, validZ = false;
bool SID;

// stores total steps since app install
static long totalSteps = TSD;

char* determineCal(int cal){
	switch(cal){
		case 2:
		X_DELTA = 45;
		Y_DELTA = 235;
		Z_DELTA = 235;
		YZ_DELTA_MIN = 225;
		YZ_DELTA_MAX = 245;
		return "Not Sensitive";
		case 3:
		X_DELTA = 25;
		Y_DELTA = 110;
		Z_DELTA = 110;
		YZ_DELTA_MIN = 100;
		YZ_DELTA_MAX = 120;
		return "Very Sensitive";
		default:
		X_DELTA = 35;
		Y_DELTA = 185;
		Z_DELTA = 185;
		YZ_DELTA_MIN = 175;
		YZ_DELTA_MAX = 195;
		return "Regular Sensitivity";
	}
}

void autoCorrectZ(){
	if (Z_DELTA > YZ_DELTA_MAX){
		Z_DELTA = YZ_DELTA_MAX;
	} else if (Z_DELTA < YZ_DELTA_MIN){
		Z_DELTA = YZ_DELTA_MIN;
	}
}

void autoCorrectY(){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "auto correct y calling");
	if (Y_DELTA > YZ_DELTA_MAX){
		Y_DELTA = YZ_DELTA_MAX;
	} else if (Y_DELTA < YZ_DELTA_MIN){
		Y_DELTA = YZ_DELTA_MIN;
	}
}

void pedometer_update() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Pedometer update calling");
		X_DELTA_TEMP = abs(abs(currX) - abs(lastX));
		if (X_DELTA_TEMP >= X_DELTA) {
			validX = true;
		}
		Y_DELTA_TEMP = abs(abs(currY) - abs(lastY));
		if (Y_DELTA_TEMP >= Y_DELTA) {
			validY = true;
			if (Y_DELTA_TEMP - Y_DELTA > 200){
				autoCorrectY();
				Y_DELTA = (Y_DELTA < YZ_DELTA_MAX) ? Y_DELTA + PED_ADJUST : Y_DELTA;
			} else if (Y_DELTA - Y_DELTA_TEMP > 175){
				autoCorrectY();
				Y_DELTA = (Y_DELTA > YZ_DELTA_MIN) ? Y_DELTA - PED_ADJUST : Y_DELTA;
			}
		}
		Z_DELTA_TEMP = abs(abs(currZ) - abs(lastZ));
		if (abs(abs(currZ) - abs(lastZ)) >= Z_DELTA) {
			validZ = true;
			if (Z_DELTA_TEMP - Z_DELTA > 200){
				autoCorrectZ();
				Z_DELTA = (Z_DELTA < YZ_DELTA_MAX) ? Z_DELTA + PED_ADJUST : Z_DELTA;
			} else if (Z_DELTA - Z_DELTA_TEMP > 175){
				autoCorrectZ();
				Z_DELTA = (Z_DELTA < YZ_DELTA_MAX) ? Z_DELTA + PED_ADJUST : Z_DELTA;
			}
		}
}

void resetUpdate() {
	lastX = currX;
	lastY = currY;
	lastZ = currZ;

	validX = false;
	validY = false;
	validZ = false;
}

void update_ui_callback() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "new step calling step number is %d",(int)pedometerCount);
	if ((validX && validY && !did_pebble_vibrate) || (validX && validZ && !did_pebble_vibrate)) {
		pedometerCount++;
		tempTotal++;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "app_worker_send_message");
    AppWorkerMessage msg_data = {
      .data0 = (int)(pedometerCount),
      .data1 = false,
      .data2 = false
    };
    app_worker_send_message(1, &msg_data);

		if (stepGoal > 0 && pedometerCount == stepGoal) {
        //Recive to goal
		}
	}
	resetUpdate();
}

static void timer_callback(void *data) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "timer calling back");
	AccelData accel = (AccelData ) { .x = 0, .y = 0, .z = 0 };
	accel_service_peek(&accel);
	currX = accel.x;
	currY = accel.y;
	currZ = accel.z;
	did_pebble_vibrate = accel.did_vibrate;
	pedometer_update();
  update_ui_callback();
	timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void foreground_message_handler(uint16_t type, AppWorkerMessage *data) {
  if ((int)type == 2)
  {
    int newGoalNumner = data->data0;
    stepGoal = newGoalNumner;
    // dailyGoalBuzzed = false;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "New goal recive from Foreground %d",newGoalNumner);
  }
}

static void worker_init(void) {
  //Subscripe for changing goal number from foreground;
  app_worker_message_subscribe(foreground_message_handler);
  tempTotal = totalSteps = persist_exists(TS) ? persist_read_int(TS) : TSD;
  determineCal(1);
  accel_data_service_subscribe(0, NULL);
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void worker_deinit(void) {
  totalSteps += pedometerCount;
	persist_write_int(TS, totalSteps);
	accel_data_service_unsubscribe();
}

int main(void) {
  worker_init();
  worker_event_loop();
  worker_deinit();
}
