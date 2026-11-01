"""Display-only boot time and Toyota assist status for Dashy."""
import math
import time


def boot_seconds():
    # CLOCK_BOOTTIME includes suspend and survives daemon/browser restarts.
    return max(0, int(time.clock_gettime(time.CLOCK_BOOTTIME)))


def extract_drive_status(sm, params, toyota_sng_flag):
    def fresh(topic):
        return sm.alive.get(topic, False) and sm.valid.get(topic, False)

    result = {'uptimeSeconds': boot_seconds(), 'currentTimeSeconds': time.time(),
              'utcOffsetSeconds': time.localtime().tm_gmtoff, 'toyota': None}
    if not fresh('carParams'):
        return result
    cp = sm['carParams']
    if str(cp.brand) != 'toyota':
        return result
    control = sm['carControl'] if fresh('carControl') else None
    longitudinal = bool(cp.openpilotLongitudinalControl)
    # This uses the applied CP capability, not a pending settings change.
    stop_and_go = bool(int(cp.flags) & toyota_sng_flag) or float(cp.minEnableSpeed) < 0
    bars = int(control.hudControl.leadDistanceBars) if control is not None and longitudinal else 0
    bars = bars if bars in (1, 2, 3) else 0
    personality = {1: 'aggressive', 2: 'standard', 3: 'relaxed'}.get(bars)
    if personality not in ('aggressive', 'standard', 'relaxed'):
        personality = None
    cs = sm['carState'] if fresh('carState') else None
    lead = sm['radarState'].leadOne if fresh('radarState') else None
    long_active = bool(control.longActive) if control is not None and longitudinal else None
    phase = None
    if stop_and_go and long_active is True and cs is not None:
        phase = 'holding' if cs.standstill else 'following'
    model = sm['modelV2'] if fresh('modelV2') else None
    probs = list(model.laneLineProbs) if model is not None else []
    def side_glow(index, blindspot):
        if cs is not None and blindspot:
            return 'green'
        if len(probs) > index and math.isfinite(probs[index]) and 0.35 <= probs[index] <= 1:
            return 'blue'
        return 'none'
    result['toyota'] = {
        'laneCenteringEnabled': params.get_bool('dp_lane_centering'),
        'lateralActive': bool(control.latActive) if control is not None else None,
        'stopAndGoAvailable': stop_and_go if longitudinal else None,
        'longitudinalActive': long_active,
        'stopAndGoPhase': phase,
        'leadDetected': bool(lead.status) if lead is not None else None,
        'leadDistanceMeters': float(lead.dRel) if lead is not None and lead.status else None,
        'leftGlow': side_glow(1, cs.leftBlindspot if cs is not None else False),
        'rightGlow': side_glow(2, cs.rightBlindspot if cs is not None else False),
        'followBars': bars,
        'followMode': personality,
    }
    return result
