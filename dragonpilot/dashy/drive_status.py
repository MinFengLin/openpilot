"""Display-only boot time and vehicle assist status for Dashy."""
import math


def extract_drive_status(sm):
    def fresh(topic):
        return sm.alive.get(topic, False) and sm.valid.get(topic, False)

    result = {'assist': None}
    if not fresh('carParams'):
        return result
    cp = sm['carParams']
    control = sm['carControl'] if fresh('carControl') else None
    longitudinal = bool(cp.openpilotLongitudinalControl)
    bars = int(control.hudControl.leadDistanceBars) if control is not None and longitudinal else 0
    bars = bars if bars in (1, 2, 3) else 0
    personality = {1: 'aggressive', 2: 'standard', 3: 'relaxed'}.get(bars)
    if personality not in ('aggressive', 'standard', 'relaxed'):
        personality = None
    cs = sm['carState'] if fresh('carState') else None
    lead = sm['radarState'].leadOne if fresh('radarState') else None
    long_active = bool(control.longActive) if control is not None and longitudinal else None
    model = sm['modelV2'] if fresh('modelV2') else None
    probs = list(model.laneLineProbs) if model is not None else []
    def side_glow(index, blindspot):
        if cs is not None and blindspot:
            return 'amber'
        if len(probs) > index and math.isfinite(probs[index]) and 0.35 <= probs[index] <= 1:
            return 'blue'
        return 'none'
    result['assist'] = {
        'alkaActive': bool(sm['controlsStateExt'].alkaActive) if fresh('controlsStateExt') else False,
        'lateralActive': bool(control.latActive) if control is not None else None,
        'longitudinalActive': long_active,
        'leadDetected': bool(lead.status) if lead is not None else None,
        'leadDistanceMeters': float(lead.dRel) if lead is not None and lead.status else None,
        'leftGlow': side_glow(1, cs.leftBlindspot if cs is not None else False),
        'rightGlow': side_glow(2, cs.rightBlindspot if cs is not None else False),
        'followBars': bars,
        'followMode': personality,
    }
    return result
