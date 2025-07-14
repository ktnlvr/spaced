#ifndef __H__SCHEDULE_SCHEDULER__
#define __H__SCHEDULE_SCHEDULER__

#include <stdio.h>

#include "../list.h"
#include "../names.h"
#include "requirements.h"

typedef enum {
  SCHEDULER_STRATEGY_RANDOM,
} scheduler_strategy_t;

typedef struct {
  system_req_t reqs;
  system_runner_f runner;
} scheduler_system_t;

typedef struct {
  /// @brief Allocator for the data that persist over the scheduler's lifetime
  allocator_t persistent_allocator;
  /// @brief Allocator for all data that lives for one frame
  allocator_t temporary_allocator;
  scheduler_strategy_t strategy;

  system_payload_t parent_payload;
  list_t *scheduler_systems;
} scheduler_t;

static scheduler_t scheduler_new(scheduler_strategy_t strategy,
                                 allocator_t persistent_allocator,
                                 allocator_t temporary_allocator) {
  scheduler_t ret;

  ret.temporary_allocator = temporary_allocator;
  ret.persistent_allocator = persistent_allocator;

  ret.strategy = strategy;

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
}

static void scheduler_add_system_declared_specific_data(
    scheduler_t *scheduler, system_requirements_declaration_t *declaration,
    void *system_specific_data) {
  name_t name = as_name(declaration->name);

  name_t *deps = 0;
  if (declaration->dependencies) {
    deps = allocator_alloc_ty(name_t, scheduler->persistent_allocator,
                              declaration->dependency_count);
    for (sz i = 0; i < declaration->dependency_count; i++)
      deps[i] = as_name(declaration->dependencies[i]);
  }

  system_req_t reqs;
  reqs.depends_on = deps;
  reqs.depends_on_count = declaration->dependency_count;
  reqs.name = name;
  reqs.phase = declaration->phase;
  reqs.pin_to_main = declaration->pin_to_main;
  reqs._entity_kinds_const = declaration->entities_const;
  reqs._entity_kinds_mut = declaration->entities_mut;

  system_runner_f runner = declaration->runner;
  scheduler_add_system(scheduler, reqs, runner);
}

static void
scheduler_add_system_declared(scheduler_t *scheduler,
                              system_requirements_declaration_t *declaration) {
  scheduler_add_system_declared_specific_data(scheduler, declaration, 0);
}

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

static void scheduler_set_parent_payload(scheduler_t *scheduler,
                                         system_payload_t payload) {
  scheduler->parent_payload = payload;
}

/// Update all the active data members of a scheduler
/// like delta_time
static void scheduler_tick(scheduler_t *scheduler, double delta_time) {
  scheduler->parent_payload.delta_time = delta_time;
}

static void scheduler_begin_running(scheduler_t *scheduler) {
  for (int phase_idx = 0; phase_idx < (int)SYSTEM_PHASE_count; phase_idx++) {
    list_t *sys_list = &scheduler->scheduler_systems[phase_idx];

    for (sz i = 0; i < sys_list->size; i++) {
      scheduler_system_t *system =
          list_get_ty_ptr(scheduler_system_t, sys_list, i);

      system_payload_t payload =
          system_payload_new_inherit(system->reqs, &scheduler->parent_payload);
      system->runner(payload, scheduler->temporary_allocator);
    }
  }
}

static void scheduler_end_running(scheduler_t *scheduler) {}

#endif
