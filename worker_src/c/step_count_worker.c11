#include <pebble_worker.h>

#define PERSIST_KEY_STEPS 0x10222
#define LOGGING_KEY_STEPS 0x10223
#define WORKER_STEPS 0

static int s_step_count = 0;
static uint64_t last_step = 0;

static uint64_t mysqrt (uint64_t a)
{
  uint64_t min=0;
  //uint64_t max=1<<32;
  uint64_t max=((uint64_t) 1) << 32; //chux' bugfix
  while(1)
    {
       if (max <= 1 + min)
         return min;

       uint64_t sqt = min + (max - min)/2;
       uint64_t sq = sqt*sqt;

       if (sq == a)
         return sqt;

       if (sq > a)
         max = sqt;
       else
         min = sqt;
    }
}


static void data_handler(AccelData *data, uint32_t num_samples) {
  // Construct a data packet
  uint64_t accel1 = mysqrt((data[0].x*data[0].x) + (data[0].y*data[0].y) + (data[0].z*data[0].z));
  uint64_t accel2 = mysqrt((data[1].x*data[1].x) + (data[1].y*data[1].y) + (data[1].z*data[1].z));
  uint64_t accel3 = mysqrt((data[2].x*data[2].x) + (data[2].y*data[2].y) + (data[2].z*data[2].z));

  int step1 = (abs(accel2 - accel1) > 150) ? 1 : 0;
  int step2 = (abs(accel2 - accel3) > 150) ? 1 : 0;

  if((step1==1 || step2==1) && !(step1==1 && step2==1) && (data[0].timestamp - last_step > 300)) {
    s_step_count++;
    last_step = data[0].timestamp;

    AppWorkerMessage msg_data = {
      .data0 = abs(accel2 - accel1),
      .data1 = abs(accel2 - accel3),
      .data2 = s_step_count
    };

    // Send the data to the foreground app
    app_worker_send_message(WORKER_STEPS, &msg_data);
  }
}

static void worker_init() {
  APP_LOG(APP_LOG_LEVEL_INFO, "started worker");
  // Use the TickTimer Service as a data source
  //tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  uint32_t num_samples = 3;
  accel_data_service_subscribe(num_samples, data_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}

static void worker_deinit() {
  // Stop using the TickTimerService
  //tick_timer_service_unsubscribe();
}

int main(void) {
  worker_init();
  worker_event_loop();
  worker_deinit();
}
