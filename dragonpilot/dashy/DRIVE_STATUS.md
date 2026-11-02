# ALKA vehicle status display

This display contains no current-time or elapsed-time fields, timer UI or
clock collection. The left-side vehicle graphic uses the full former time
card width: clamp(136px, 15vw, 176px), limited to 42% of viewport width.
The vehicle card follows the graphic height, without a square aspect constraint.
The SVG crops unused vertical space (viewBox 0 16 120 84) and preserves the
car and lane geometry proportions.

## Visibility and signals

- The entire block requires fresh, valid controlsStateExt.alkaActive = true,
  matching the existing ALKA badge beside the speed. DP lane-centering settings,
  cruise engagement and longitudinal control alone cannot display the block.
- The two side lane lines are visible whenever the ALKA block is visible.
- Distance bars require actual carControl.longActive and use the commanded
  hudControl.leadDistanceBars: 1 aggressive, 2 standard, 3 relaxed.
- Bars disappear from top to bottom. The front car moves closer and grows
  slightly with fewer selected bars; its position illustrates gap mode,
  not a measured radar distance.
- The front car is visible only if fresh radarState.leadOne.status is true,
  independently of longitudinal engagement. Without a lead, the distance bars
  can remain visible but the car is hidden.
- Missing/stale/invalid ALKA hides the whole block. A disconnected browser
  hides it after four seconds. Behavior is not limited to a vehicle brand.

## Side glow

Fresh modelV2.laneLineProbs[1]/[2] in [0.35, 1] enables the left/right blue
gradient. Fresh carState.leftBlindspot/rightBlindspot overrides its side with
amber (#ffc300), matching the original canvas blindspot color.

## Layout

Road names, speed signs, road preview text and the vehicle panel retain their
original sizes. Compact-layout CSS no longer reduces these fonts, widths or
sign dimensions, and no corner scale transform is applied. TDX is repositioned
below visible corner cards. Extremely short viewports may require more vertical
space; readability is prioritized over shrinking the information again.

The standalone preview embeds current runtime assets and has ALKA, live lead,
longitudinal control, gap mode, lane and blindspot simulation controls.
Actual car behavior still needs device validation.

Vehicle status is anchored at the upper left; road name, current speed limit
and ahead-limit information are anchored at the upper right. Safe-area insets
follow the corresponding screen edge. The detail popup opens from the left.
