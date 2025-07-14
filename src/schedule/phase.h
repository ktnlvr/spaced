#ifndef __H__SCHEDULE_PHASE__
#define __H__SCHEDULE_PHASE__

typedef enum {
  SYSTEM_PHASE_PRE_UPDATE,
  SYSTEM_PHASE_UPDATE,
  SYSTEM_PHASE_FIXED_UPDATE,
  SYSTEM_PHASE_POST_UPDATE,
  SYSTEM_PHASE_PRE_RENDER,
  SYSTEM_PHASE_RENDER,
  SYSTEM_PHASE_POST_RENDER,
  SYSTEM_PHASE_count,
} system_phase_t;

static const char *system_phase_to_str(system_phase_t phase) {
  switch (phase) {
  case SYSTEM_PHASE_PRE_UPDATE:
    return "PRE_UPDATE";
  case SYSTEM_PHASE_UPDATE:
    return "UPDATE";
  case SYSTEM_PHASE_FIXED_UPDATE:
    return "FIXED_UPDATE";
  case SYSTEM_PHASE_POST_UPDATE:
    return "POST_UPDATE";
  case SYSTEM_PHASE_PRE_RENDER:
    return "PRE_RENDER";
  case SYSTEM_PHASE_RENDER:
    return "RENDER";
  case SYSTEM_PHASE_POST_RENDER:
    return "RENDER";
  default:
    return 0;
  }
}

#endif
