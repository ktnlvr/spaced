#ifndef __H__SYSTEMS_SCHEDULER__
#define __H__SYSTEMS_SCHEDULER__

#include <stdio.h>

#include "../list.h"
#include "../names.h"
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
  list_t *scheduler_systems;
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

  ret.scheduler_systems = allocator_alloc_ty(list_t, ret.persistent_allocator,
                                             (int)SYSTEM_PHASE_count);

  for (int i = 0; i < SYSTEM_PHASE_count; i++) {
    list_init_ty(scheduler_system_t, &ret.scheduler_systems[i],
                 ret.persistent_allocator);
  }

  return ret;
}

static void scheduler_add_system(scheduler_t *scheduler, system_req_t req,
                                 system_runner_f runner) {
  scheduler_system_t system;
  system.runner = runner;
  system.reqs = req;

  list_push_var(&scheduler->scheduler_systems[req.phase], system);

  scheduler->is_schedule_planned = false;
}

#define scheduler_declare_system_with_custom_runner(                           \
    scheduler, mut, cons, system_name, runner_f, deps_on, dependency_count,    \
    ...)                                                                       \
  name_t runner_f##_name = as_name(#system_name);                              \
  do {                                                                         \
    system_req_t __system_req = __VA_ARGS__;                                   \
    __system_req.name = runner_f##_name;                                       \
    __system_req.depends_on = deps_on;                                         \
    __system_req.depends_on_count = dependency_count;                          \
    system_req_entity_kinds_const(&__system_req, cons);                        \
    system_req_entity_kinds_mut(&__system_req, mut);                           \
    scheduler_add_system(scheduler, __system_req, runner_f);                   \
  } while (0)

/// @brief Defines a new system with the name `system_name`
/// that depends on the systems with the names from `depends_on`,
/// and requires read only access to `cons` and mutable access to `mut`.
/// Adds the system to the `scheduler`.
/// @memberof scheduler_t
#define scheduler_declare_system(scheduler, mut, cons, system_name,            \
                                 depends_on, dependency_count, ...)            \
  scheduler_declare_system_with_custom_runner(                                 \
      scheduler, mut, cons, system_name, system_##system_name, depends_on,     \
      dependency_count, __VA_ARGS__)

static void scheduler_dump_dependency_graph(scheduler_t *scheduler, FILE *out) {
  sz capacity = 0;
  for (int phase_idx = 0; phase_idx < (int)SYSTEM_PHASE_count; phase_idx++)
    capacity += scheduler->scheduler_systems[phase_idx].size;

  allocator_t alloc = allocator_new_stack_alloc();
  list_t execution_order;
  list_init_ty_with_capacity(name_t, &execution_order, alloc, capacity);

  fprintf(out, "digraph G {\n\trankdir=\"TB\";\n");

  for (int phase_idx = 0; phase_idx < (int)SYSTEM_PHASE_count; phase_idx++) {
    list_t *sys_list = &scheduler->scheduler_systems[phase_idx];

    if (sys_list->size == 0)
      continue;

    const char *sys_name_str = system_phase_to_str((system_phase_t)phase_idx);

    fprintf(out,
            "\tsubgraph cluster_%d {\n\t\tlabel = \"%s\";\n\t\tstyle = "
            "filled;\n\t\tcolor = lightgrey;\n",
            phase_idx, sys_name_str);

    for (sz i = 0; i < sys_list->size; i++) {
      scheduler_system_t *sys =
          list_get_ty_ptr(scheduler_system_t, sys_list, i);

      name_t name = sys->reqs.name;
      const char *name_str = name_as_str(name);

      fprintf(out, "\t\t%ld [label=\"%s #%ld\"];\n", name, name_str, i);

      for (sz j = 0; j < sys->reqs.depends_on_count; j++) {
        name_t dependency = sys->reqs.depends_on[j];
        fprintf(out, "\t\t%ld -> %ld;\n", dependency, name);
      }

      list_push_var(&execution_order, name);
    }

    fprintf(out, "\t}\n");
  }

  fprintf(out, "\tstart [color = blue, shape = Mdiamond];\n");
  fprintf(out, "\tend [color = blue, shape = Mdiamond];\n");
  fprintf(out, "\t{ rank=\"source\"; \"start\";}\n");
  fprintf(out, "\t{ rank=\"sink\"; \"end\";}\n");

  fprintf(out, "\tstart -> %ld [color = blue];\n",
          list_get_ty(name_t, &execution_order, 0));
  for (sz i = 0; i < execution_order.size - 1; i++) {
    name_t n1 = list_get_ty(name_t, &execution_order, i);
    name_t n2 = list_get_ty(name_t, &execution_order, i + 1);

    fprintf(out, "\t%ld -> %ld [color = blue];\n", n1, n2);
  }

  fprintf(out, "\t%ld -> end [color = blue];\n",
          list_get_ty(name_t, &execution_order, execution_order.size - 1));

  fprintf(out, "}\n");
}

static void scheduler__topological_sort(allocator_t alloc,
                                        list_t *scheduler_system_list) {
  ASSERT__(scheduler_system_list->_entry == sizeof(scheduler_system_t));

  map_t coordinate_compression_forward;
  map_init_ty(sz, &coordinate_compression_forward, alloc);
  map_t coordinate_compression_reverse;
  map_init_ty(sz, &coordinate_compression_reverse, alloc);

  for (sz i = 0; i < scheduler_system_list->size; i++) {
    name_t n = list_get_ty(name_t, scheduler_system_list, i);
    if (!map_get(&coordinate_compression_forward, n)) {
      map_insert_ty(sz, &coordinate_compression_forward, n,
                    &coordinate_compression_forward.size);
    }
  }

  ASSERT__(coordinate_compression_forward.size == scheduler_system_list->size);
  ASSERT__(coordinate_compression_reverse.size == scheduler_system_list->size);

  map_cleanup(&coordinate_compression_forward);
  map_cleanup(&coordinate_compression_reverse);
}

static void scheduler_plan(scheduler_t *scheduler) {
  ASSERT_(!scheduler->is_running,
          "Attempt to change plans while the scheduler is running");

  scheduler->is_schedule_planned = true;
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

  for (int phase_idx = 0; phase_idx < (int)SYSTEM_PHASE_count; phase_idx++) {
    list_t *sys_list = &scheduler->scheduler_systems[phase_idx];

    for (sz i = 0; i < sys_list->size; i++) {
      scheduler_system_t *system =
          list_get_ty_ptr(scheduler_system_t, sys_list, i);
      system_req_fill_in(&system->reqs, scheduler->requirements);
    }
  }

  scheduler->is_running = true;

  for (int phase_idx = 0; phase_idx < (int)SYSTEM_PHASE_count; phase_idx++) {
    list_t *sys_list = &scheduler->scheduler_systems[phase_idx];

    for (sz i = 0; i < sys_list->size; i++) {
      scheduler_system_t *system =
          list_get_ty_ptr(scheduler_system_t, sys_list, i);
      system->runner(system->reqs, scheduler->temporary_allocator);
    }
  }
}

static void scheduler_end_running(scheduler_t *scheduler) {
  scheduler->is_running = false;
  scheduler->is_ticked = false;
}

#endif
