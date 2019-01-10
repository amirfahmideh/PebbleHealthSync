#include <pebble_worker.h>
#include <math.h>
#define DEBUG false
#define BATCH_SIZE 10

static uint32_t sleepCounterPerPeriod = 0;
static uint32_t otherCounterPerPeriod = 0;

static uint32_t totalSteps = 0;
static int minuteCounter = 0;
static uint32_t stepsPerPeriod = 0;
static uint32_t oldSteps = 0;
static int segmentsInactive = 0;
static uint32_t visibleSegments = 0;
static uint32_t activeMinutes = 0;
static int dayNumber = 0;
static int lastMinute;

static int daysNo = 1;
static int daysYes = 1;
static bool dailyGoalBuzzed = false;

static bool isMoving = false;
static bool isSleeping = false;
static bool needBuzz = false;
static int buzzNo = 0;

static uint32_t dailyGoal;
static uint32_t steps = 0;

static void updateSteps(void){
  AppWorkerMessage msg_data = {
    .data0 = (int)(totalSteps),
    .data1 = isSleeping,
    .data2 = needBuzz
  };
  APP_LOG(APP_LOG_LEVEL_DEBUG, "app_worker_send_message");
  app_worker_send_message(1, &msg_data);
}

float my_sqrt(const float num) {
  const uint MAX_STEPS = 30;
  const float MAX_ERROR = 1.0;

  float answer = num;
  float ans_sqr = answer * answer;
  uint step = 0;
  while((ans_sqr - num > MAX_ERROR) && (step++ < MAX_STEPS)) {
      if(answer != 0){
          answer = (answer + (num / answer)) / 2;
      }
    ans_sqr = answer * answer;
  }
  return answer;
}
static float lastEv = 0;
static int lastStepNo;
static uint32_t stepsInARow = 0;
static int totalEv = 0;
static char tmpStr[14];
// This one is better in false detection - almost no "sitting" steps and more accurate in walking steps counting
// Steady pace walking - accuracy 100% - tested on 200-step blocks.
// Sitting - almost no false steps.
// Driving - +30-70 steps per 30-min drive - I think it's acceptable.
// Misfit app counts about 30% more steps, however it's known for counting extra steps. It is very hard to avoid this -
// you count steps by detecting your hand moves...
void processAccelerometerData(AccelData* acceleration, uint32_t size)
{
    float evMax = 0;
    float evMin = 5000000;
    float evMean = 0;
    float ev[10];
    float evAv[10];
    for(uint32_t i=0;i<size&&i<10;i++){
        ev[i] = my_sqrt(acceleration[i].x*acceleration[i].x + acceleration[i].y*acceleration[i].y + acceleration[i].z*acceleration[i].z);
    }
    // Need to eliminate slow change and detect peaks...
    // 1. Find very simple moving average
    evAv[0] = (lastEv + ev[0])/2;
    evAv[1] = (lastEv + ev[0]+ev[1])/3;
    for(int i=2;i<10;i++){
        evAv[i] = (ev[i]+ev[i-1]+ev[i-2])/3;
    }
    lastEv = ev[9];
    /*
    // This one is OK, but requires more CPU
    evAv[0] = (lastEv + ev[0])/2;
    for(int i=1;i<10;i++){
        evAv[i] = 0.18*ev[i]+0.82*evAv[i-1];
    }
    lastEv = ev[9];
    */
    // 2. Find peaks above average line and higher than minimum energy
    for(int i=0;i<10;i++){
        // 3 steps per second = 180 steps per minute maximum, who can run faster?
        // Well, tests show that it drops steps somehow... Back to 1. 0.8 and 24.000 are pure empirical values.
        // Values greater than 24.000 give less false detections, but can skip steps if you, for example, have something
        // in your hand and don't move it much.
        // One strange thing - movements in horizontal plane even with considerable amplitude, give very small ev value.
        // So, walking in an elliptical trainer with moving handles counts only about 10% of real steps... Donna what to do
        // with that. Perhaps lowering evMeanMax would solve the problem, but will certainly give more false steps in other
        // conditions.
        evMean += ev[i];
        if(lastStepNo > 2 && ev[i] > evAv[i]*1.05 && evAv[i] > 70){ // evMean*1.1 && evMean > 20000){
            steps++;
            //snprintf(tmpStr, 31, "%d+%d", (int)ev[i],(int)evAv[i]);
            if(lastStepNo < 9)stepsInARow++; // if last step was detected less than 0.9 seconds before current, count it as a sequence
            else{
                stepsInARow = 0;
                steps = 0;
            }
            lastStepNo = 0;
        }else{
            //snprintf(tmpStr, 31, "%d;%d", (int)ev[i],(int)evAv[i]);
        }

        //static char tmpStr[32];
        //APP_LOG(APP_LOG_LEVEL_INFO, "Extr ", 0, tmpStr, mWindow);

        lastStepNo++;
    }
        /*
        snprintf(tmpStr, 63, "%d/%d, %d/%d", (int)ev[0],(int)evAv[0],(int)ev[1],(int)evAv[1]);
        APP_LOG(APP_LOG_LEVEL_INFO, "Extr ", 0, tmpStr, mWindow);
    */
    // 6. Count steps only if there are several steps in a row to avoid random movements. Downside is that
    // if you step less than 7 steps in a row or stop for a while it would not count them.
    if(stepsInARow > 7){
        totalSteps += steps;
        if(totalSteps >= dailyGoal && !dailyGoalBuzzed){
            dailyGoalBuzzed = true;
            // buzzAchieved();
            persist_write_int(11, true);
        }
        else {
            persist_write_int(11, false);
        }
        if(steps > 0){
            updateSteps();
        }else{

        }
        steps = 0;
    }
    // 7. Detect off-the-wrist condition not to buzz when nobody hears it. Or at night.
    // Off the wrist: Mean < 150 or Max < ~200-400
    // Seating: Mean up to 24.000
    // Walking: mean ~24.000
    // Jogging: ?

    //snprintf(tmpStr, 63, "%d", (int)evMean);
    //APP_LOG(APP_LOG_LEVEL_INFO, "Extr ", 0, tmpStr, mWindow);
    //if(evMax < 260){
    //if(evMean < 130){
    if(evMean < 10300){//10360
        sleepCounterPerPeriod++;
    }else{
        otherCounterPerPeriod++;
    }
    evMean = 0;

    //static char tmpStr2[32];
    //snprintf(tmpStr2, 32, "%5d:%5d", (int)evMeanMax, (int)evMax);
    //snprintf(tmpStr, 128, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", (int)ev[0],(int)ev[1],(int)ev[2],(int)ev[3],(int)ev[4],(int)ev[5],(int)ev[6],(int)ev[7]),(int)ev[8],(int)ev[9]);
	//APP_LOG(APP_LOG_LEVEL_INFO, "Extr ", 0, tmpStr2, mWindow);
    //APP_LOG(APP_LOG_LEVEL_INFO, "%d:%d", (int)evMeanMax, (int)evMax);

}

// This one is pretty accurate, giving about 5-8% less steps, but counts some false steps that compensates it.
void processAccelerometerDataWorking(AccelData* acceleration, uint32_t size)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Find me in code if you see me dude!!!");
    float evMax = 0;
    float evMin = 5000000;
    float evMean = 0;
    float ev[10];

    for(uint32_t i=0;i<size&&i<10;i++){
        ev[i] = my_sqrt(acceleration[i].x*acceleration[i].x + acceleration[i].y*acceleration[i].y + acceleration[i].z*acceleration[i].z);
        ev[i] *= ev[i]; // make peaks sharper
        if(ev[i] > evMax) evMax = ev[i];
        if(ev[i] < evMin) evMin = ev[i];
        evMean += ev[i];
    }
    evMean /= 10;//min(10,step);
    evMean -= evMin;
    evMax -= evMin;

    /*
    char tmpStr[64];
    snprintf(tmpStr,64, "%5d-%5d-%5d-%5d-%5d-%5d-%5d-%5d-%5d-%5d", (int) ev[0],(int) ev[1],(int) ev[2],(int) ev[3],(int) ev[4],(int) ev[5],(int) ev[6],(int) ev[7],(int) ev[8],(int) ev[9]);
    APP_LOG(APP_LOG_LEVEL_INFO, "energy",0, tmpStr, mWindow);
    */

    // filter out too frequent peaks, anyway, only 3 steps per second seem sane
    // the filter is a bit rough, but who cares!
    for(int i=0;i<9;i++){
        if(ev[i+1] == 0)continue;
        float t = ev[i]/ev[i+1];
        if(t<1.2 && t>=1){
            ev[i+1] = evMin;
        }else if(t<1 && t>0.8){
            ev[i] = evMin;
        }
    }
    //int stepCounted = 0;
    for(int i=0;i<10;i++){
        //ev[i] -= evMin; // well, I should do it, but it works better without! Or I need to tweak the next line...
        if(ev[i] > evMean+(evMax-evMean)*0.5 && evMean > 575000){
            steps++;
        }
    }
    totalSteps += steps == 1?1:steps/2;
    if(steps > 0){
        //mCurrentType = 2;
        updateSteps();
    }else{

    }
    steps = 0;
    // Normalized squared:
    // Sleeping: Mean < 30.000
    // Seating: Mean
    // Walking: mean ~600.000
    // Jogging:
    if(evMean < 20000){
        sleepCounterPerPeriod++;
    }else{
        otherCounterPerPeriod++;
    }
    /*
    char tmpStr[64];
    snprintf(tmpStr, 64, "%d-%d-%d:%d,%d,%d,%d,%d,%d", (int) evMin/100, (int)evMean/100, (int)evMax/100, (int)ev[1]/100,(int)ev[2]/100,(int)ev[3]/100,(int)ev[4]/100,(int)ev[5]/100,(int)ev[6]/100);
	APP_LOG(APP_LOG_LEVEL_INFO, "Extr ", 0, tmpStr, mWindow);
    */
}

static void update_time(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Update_Time() runs in background!");
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  if(tick_time->tm_min == lastMinute){
      return;
  }
  lastMinute = tick_time->tm_min;
  //isSleeping = (totalEv/10 < 1600); //sleepCounterPerPeriod > otherCounterPerPeriod*1.4; // or make it = otherCounterPerPeriod > 20?
  //totalEv = 0;
  isSleeping = (sleepCounterPerPeriod > 56);
  minuteCounter++;
  stepsPerPeriod = totalSteps - oldSteps;
  oldSteps = totalSteps;
  if(stepsPerPeriod > 50){ // count active if you made more than 40 steps per minute
      segmentsInactive -= stepsPerPeriod/4;// 30;
      activeMinutes++;
      isMoving = true;
      if(segmentsInactive <= 0){
          segmentsInactive = 0;
      }
      needBuzz = false;
      buzzNo = 0;
  }else if(stepsPerPeriod < 30 && !isSleeping){ // less than 20 steps - you're inactive
      segmentsInactive++;
      //segmentsInactive += 14; // TEST
      isMoving = false;
  }else{
      isMoving = false;
  }
  stepsPerPeriod = 0;
  sleepCounterPerPeriod = 0;
  otherCounterPerPeriod = 0;
  if(segmentsInactive > 120){
      segmentsInactive = 120; // to reset the timer you need 360 steps max
  }
  if(!needBuzz && segmentsInactive > 59 && !isMoving){
      // buzz
      needBuzz = true;
      minuteCounter = 0;
  }
  if(!isSleeping && needBuzz && minuteCounter % 6 == 0){
      //buzz(); xxx
      minuteCounter = 0;
      persist_write_bool(10, true);
  }
  else {
      persist_write_bool(10, false);
  }
  if(dayNumber != tick_time->tm_yday){
      // Next day, reset all
      if(totalSteps < dailyGoal){ // reduce your next daily goal by 5%
          dailyGoal *= 0.95;
          daysNo++;
          persist_write_int(1, daysNo);
      }else{
          dailyGoal *= 1.05;
          daysYes++;
          persist_write_int(2, daysYes);
      }
      dailyGoal = ceil(dailyGoal/10)*10;

      dayNumber = tick_time->tm_yday;
      totalSteps = 0;
      oldSteps = 0;
      activeMinutes = 0;
      dailyGoalBuzzed = false;
  }
  updateSteps();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void foreground_message_handler(uint16_t type, AppWorkerMessage *data) {
  if ((int)type == 2)
  {
    int newGoalNumner = data->data0;
    dailyGoal = newGoalNumner;
    dailyGoalBuzzed = false;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "New goal recive from Foreground %d",newGoalNumner);
  }
}

static void worker_init(void) {
	   // Load persistent values
    daysNo = persist_exists(1) ? persist_read_int(1) : 1;
    daysYes = persist_exists(2) ? persist_read_int(2) : 1;

	  totalSteps = persist_exists(5) ? persist_read_int(5) : 0;
    segmentsInactive = persist_exists(6) ? persist_read_int(6) : 0;
    if(segmentsInactive > 90){
        segmentsInactive = 90;
    }
    //segmentsInactive = 0;
    activeMinutes = persist_exists(7) ? persist_read_int(7) : 0;
    oldSteps = persist_exists(8) ? persist_read_int(8) : 0;
    dayNumber = persist_exists(9) ? persist_read_int(9) : 0;
    dailyGoal = persist_exists(111) ? persist_read_int(111) : 8250;

    // APP_LOG(APP_LOG_LEVEL_DEBUG, "Goal number which read by worker is %d",(int)dailyGoal);

	  // Setup accelerometer API
	  accel_data_service_subscribe(BATCH_SIZE, &processAccelerometerData);
	  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    // battery_state_service_subscribe(battery_handler);

    //snprintf(buffer, 6, "%.5d", (int) (mCounter.steps));
    //text_layer_set_text(s_steps_layer, buffer);
    updateSteps();
    update_time();

    //Subscripe for changing goal number from foreground;
    app_worker_message_subscribe(foreground_message_handler);
}

static void worker_deinit(void) {
	if (DEBUG) {
		char msg[] = "deinit() called";
	}
	// Save persistent values
  persist_write_int(5, totalSteps);
  persist_write_int(6, segmentsInactive);
  persist_write_int(7, activeMinutes);
  persist_write_int(8, oldSteps);
  persist_write_int(9, dayNumber);
  // persist_write_int(10, dailyGoal);
	accel_data_service_unsubscribe();
  tick_timer_service_unsubscribe();
  app_worker_message_unsubscribe();
  // battery_state_service_unsubscribe();
}

int main(void) {
  worker_init();
  worker_event_loop();
  worker_deinit();
}
