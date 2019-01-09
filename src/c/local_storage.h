#include <pebble.h>
uint32_t GOAL_KEY_NUMBER = 10;
static void SaveGoalSteps(int goal_number)
{
    persist_write_int(GOAL_KEY_NUMBER, goal_number);
}
static int ReadGoalSteps(int default_value)
{
  int num_items;
  if (persist_exists(GOAL_KEY_NUMBER)) {
    num_items = persist_read_int(GOAL_KEY_NUMBER);
  }
  else {
    num_items = default_value;
  }
  return num_items;
}
