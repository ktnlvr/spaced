#ifndef __H__SYSTEMS_SCHEDULER__
#define __H__SYSTEMS_SCHEDULER__

#include "../list.h"
#include "require.h"

typedef void (*system_runner_f)(system_req_t payload,
                                allocator_t temporary_allocator);

typedef enum {
  SCHEDULER_STRATEGY_RANDOM,
} scheduler_strategy_t;

typedef struct {
  system_req_t reqs;
  system_runner_f runner;
} scheduler_system_t;

typedef struct {
  allocator_t persistent_allocator;
  allocator_t temporary_allocator;
  scheduler_strategy_t strategy;

  bool is_requirement_list_set;
  bool is_schedule_planned;
  bool is_ticked;
  bool is_running;

  system_req_t requirements;
  list_t scheduler_systems;
} scheduler_t;

static scheduler_t scheduler_new(scheduler_strategy_t strategy,
                                 allocator_t persistent_allocator,
                                 allocator_t temporary_allocator) {
  scheduler_t ret;

  ret.temporary_allocator = temporary_allocator;
  ret.persistent_allocator = persistent_allocator;

  ret.strategy = strategy;
  ret.is_requirement_list_set = false;
  ret.is_ticked = false;
  ret.is_running = false;

  list_init_ty(scheduler_system_t, &ret.scheduler_systems,
               ret.persistent_allocator);

  return ret;
}

static void scheduler_add_system(scheduler_t *scheduler, system_req_t req,
                                 system_runner_f runner) {
  scheduler_system_t system;
  system.runner = runner;
  system.reqs = req;

  list_push_var(&scheduler->scheduler_systems, system);

  scheduler->is_schedule_planned = false;
}

static void scheduler_plan(scheduler_t *scheduler) {
  ASSERT_(!scheduler->is_running,
          "Attempt to change plans while the scheduler is running");

  scheduler->is_schedule_planned = true;
  // TODO: actually plan everything
}

static void scheduler_set_requirements(scheduler_t *scheduler,
                                       system_req_t reqs) {
  scheduler->requirements = reqs;
  scheduler->is_requirement_list_set = true;
}

/// Update all the active data members of a scheduler
/// like delta_time
static void scheduler_tick(scheduler_t *scheduler, double delta_time) {
  scheduler->requirements.delta_time = delta_time;
  scheduler->is_ticked = true;
}

static void scheduler_begin_running(scheduler_t *scheduler) {
  ASSERT_(scheduler->is_requirement_list_set,
          "The requirement list must be set before running");
  ASSERT_(scheduler->is_schedule_planned,
          "The scheduler must have planned the system order");
  ASSERT_(scheduler->is_ticked,
          "The scheduler must be ticked every time before running");
  ASSERT_(!scheduler->is_running, "The scheduler is already running");

  for (sz i = 0; i < scheduler->scheduler_systems.size; i++) {
    scheduler_system_t *system =
        list_get_ty_ptr(scheduler_system_t, &scheduler->scheduler_systems, i);
    system_req_fill_in(&system->reqs, scheduler->requirements);
  }

  scheduler->is_running = true;

  for (sz i = 0; i < scheduler->scheduler_systems.size; i++) {
    scheduler_system_t *system =
        list_get_ty_ptr(scheduler_system_t, &scheduler->scheduler_systems, i);
    system->runner(system->reqs, scheduler->temporary_allocator);
  }
}

static void scheduler_end_running(scheduler_t *scheduler) {
  scheduler->is_running = false;
  scheduler->is_ticked = false;
}

#endif
