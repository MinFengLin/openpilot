# Upper-right boot clock and vehicle assist graphic

The clock uses Linux CLOCK_BOOTTIME, including offroad time and suspend.
Refreshing the phone or restarting dashyd does not reset it; rebooting does.

The vehicle SVG follows these display rules:

- Side lane lines light when the DP lane-centering setting is enabled.
  This is intentionally the setting, not proof of active lateral actuation.
- Distance bars appear only while valid carControl.longActive is true.
  Their count is the actual carControl.hudControl.leadDistanceBars command:
  1 aggressive, 2 standard, 3 relaxed. These are commanded bars, not stock
  instrument-cluster PCM feedback or a measured following distance.
- Bars disappear from top to bottom: 3 to 2 hides the upper bar; 2 to 1
  hides the middle bar. Removed bars have zero opacity.
- The front-car silhouette requires fresh radarState.leadOne.status.
  It moves downward and grows slightly as the selected bar count decreases.
  This position illustrates the selected gap mode, not radar dRel.
- No separate steering-wheel or Stop & Go icon is displayed underneath.
- With both centering off and longitudinal control inactive/unknown, the
  entire assist graphic hides. The boot clock remains.

All vehicle brands use the same shared signals; there is no brand whitelist.
Without valid carParams only the clock is shown. Stock ACC control is not inferred.
The backend refreshes once per second; four seconds without a received
payload hides the panel. All graphics are display-only. Tap the graphic for
an explanation; labels are not visible by default.

`preview-drive-status.html` embeds the actual SVG/CSS with simulated controls.
Actual phone rendering and vehicle behavior still require device validation.

```sh
python3 -m unittest dragonpilot/dashy/test_drive_status.py
node dragonpilot/dashy/test_drive_status_hud.cjs
```

## Side glow

Each side of the assist diagram has its own outward-fading gradient.
The adjacent ego-lane confidence is modelV2.laneLineProbs[1] (left) or [2]
(right). A finite confidence in [0.35, 1] lights blue, matching the existing
Tesla theme's 0.35 confidence cutoff. Fresh carState.leftBlindspot or
rightBlindspot takes priority and lights that side amber (#ffc300), even when its lane
is not detected. Missing/stale/invalid model and car-state signals do not
retain their previous glow. The cluster's existing visibility rule remains:
with neither centering enabled nor longitudinal control active, it hides.
The preview has simulated independent lane and blindspot switches.

## Time labels

The top line shows vehicle local wall-clock time, including its UTC offset,
interpolated between updates independently of the phone timezone. The second
line is labeled 行駛時間 but still measures time since device boot, including
offroad time; it is not accumulated moving time. Unknown wall-clock data shows
an em dash. The standalone preview uses the computer clock as simulated data.

The backend payload is driveStatus.assist. The former Toyota-only gate and
ToyotaFlags dependency have been removed. Brand-specific settings elsewhere
in Dashy remain filtered to their applicable vehicles.

## Compact viewport avoidance

Below 760 px wide or 420 px high (HUD content dimensions), corner cards use
compact styles with a central gap. The layout worker measures visible corner
cards and places TDX below both. Short screens scale corners only when needed
to leave room for TDX. Resize and state changes recalculate placement; hidden
cards stop reserving space. The preview includes the actual road/TDX overlays.
The original canvas side-light implementation is unchanged; blindspot gradients
in the new graphic now match its amber RGB (255, 195, 0).
