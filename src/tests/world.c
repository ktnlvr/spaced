#include "../engine/world.h"
#include "../arena.h"
#include <stdio.h>

static world_t world;

int _test_count = 4;
const char *_tests[] = {
    "_test_identity_and_generation",
    "_test_two_entities",
    "_test_generations",
    "_test_entity_iter",
};

void _test_setup() { world_init(&world); }

void _test_cleanup() { world_cleanup(&world); }

void _test_identity_and_generation() {
  entity_id_t id = 0x0123456789ABCDEF;
  u32 gen = entity_id_get_generation(id);
  u64 identity = entity_id_get_identity(id);

  ASSERT(gen == 0x012, "%03X == 0x012", gen);
  ASSERT__(identity == 0x3456789ABCDEF);

  entity_id_t id_new = entity_id_new(gen, identity);
  ASSERT__(id == id_new);
}

void _test_two_entities() {
  entity_id_t e1 = world_spawn_entity(&world);

  ASSERT__(world_count_entities(&world) == 1);
  ASSERT__(world_get_entity(&world, e1)->kind == ENTITY_KIND_TOMBSTONE);

  bool element_deleted = world_destroy_entity(&world, e1);
  ASSERT__(element_deleted);

  ASSERT__(world_count_entities(&world) == 0);

  entity_id_t e2 = world_spawn_entity(&world);

  ASSERT__(entity_id_get_identity(e2) == entity_id_get_identity(e1));

  u32 e1_gen = entity_id_get_generation(e1);
  u32 e2_gen = entity_id_get_generation(e2);
  ASSERT__(e2_gen > e1_gen);
}

void _test_generations() {
  arena_t a;
  arena_init(&a, allocator_new_malloc(), 0x4000);

  list_t entities;
  list_init_ty(entity_id_t, &entities, arena_as_allocator(&a));

  for (int i = 0; i < 0xFF; i++) {
    entity_id_t e = world_spawn_entity(&world);
    list_push(&entities, &e);
  }

  ASSERT__(world_count_entities(&world) == 0xFF);

  for (int i = 0; i < 0xFF; i++)
    world_destroy_entity(&world, list_get_ty(entity_id_t, &entities, i));

  ASSERT__(world_count_entities(&world) == 0);

  sz null_pointers = 0;
  for (int i = 0; i < 0xFF; i++) {
    entity_t *ptr =
        world_get_entity(&world, list_get_ty(entity_id_t, &entities, i));
    ASSERT(ptr == 0, "ptr=%lX, world->entities._data=%lX", (sz)ptr,
           (sz)world._entities.data);
  }
}

void _test_entity_iter() {
  for (int i = 0; i < 50; i++) {
    entity_id_t e = world_spawn_entity(&world);
    // TODO: Introduce a difference between a TOMBSTONE and an EMPTY
    world_get_entity(&world, e)->kind = ~ENTITY_KIND_TOMBSTONE;
  }

  for (int i = 0; i < 50; i++)
    if (i % 2)
      world_destroy_entity(&world, i);

  ASSERT__(world_count_entities(&world) == 25);

  sz iters = 0;
  for (entity_iter_t it = world_entity_iter(&world); entity_iter_next(&it);)
    iters++;

  ASSERT(iters == 25, "%ld", iters);
}
