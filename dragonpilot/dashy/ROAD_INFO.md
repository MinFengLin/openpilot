# Offline road name and speed limit (SP mapd)

Uses the same pfeiferj mapd v1.12.0 engine pinned by Sunnypilot v2026.002.002.
The wrapper adapts DP liveGPS to mapd's LastGPSPosition memory parameter, then
forwards MapSpeedLimit (m/s) and RoadName to the existing Dashy roadInfo overlay.
It does not read mapd's curve-control outputs or change any driving target.

## Device UI

In **DP Settings**, scroll to **離線道路資料**:

1. Select Country (Taiwan is the default). Country names are bundled with the
   pinned engine version, so opening the selector does not require internet.
2. If selecting the US, choose a state or all states.
3. While Offroad, select **下載／更新離線道路資料** and confirm.
4. Watch the status line: it shows downloaded files / total files.
5. Enable **車機與 Dashy 道路限速與名稱** in the UI section.

Switching country does not erase existing maps. A selected region is usable
only after its download completes. Return to an already downloaded region
without downloading again. The delete button deletes ALL downloaded road data
after confirmation, while Offroad only. It does not delete driving logs.

Initial install and map updates need internet; normal on-road lookup is offline.
The wrapper requires at least 2 GB free before download, but this is only an
initial minimum, not a size guarantee: large countries may need much more.
Road lookup runs only on-road; the engine stops offroad except during downloads.
If the download engine exits, the busy state clears so the user can retry.
Downloads stop if the device goes on-road; select download again while Offroad
to retry. Failed/partial requests are not marked installed.

## Engine and storage

- Fixed ARM64 Linux engine: v1.12.0, SHA-256
  `fdb3b49ee19956e6ce09fdc3373cbba557f1263b2180e9f344c1d4053852284b`.
- Runtime binary: `/data/media/0/osm/bin/mapd-v1.12.0`.
- SP-compatible offline data: `/data/media/0/osm/offline`.
- Downloader/lookup subprocess managed by `dragonpilot.mapd.manager`.
- No Mapbox Token, online matching, map tiles or turn-by-turn navigation.
- Normal desktop x86 binaries are not supported by the pinned ARM64 download.

The matching/output format and downloaded road engine are upstream mapd, not a
new SQLite matcher. Region name catalogs and upstream license are in
`dragonpilot/mapd/`. Road data is © OpenStreetMap contributors, ODbL:
https://www.openstreetmap.org/copyright

## Limitations / validation

Missing GPS, stopped mapd, stale outputs or an uninstalled selection hide the
display. MapSpeedLimit 0/unknown hides the numeric sign; a known road name may
still appear. Coverage/matching can be imperfect, especially at parallel roads
and elevated interchanges; posted signs take priority. This is display only.

The port still requires an ARM64 device download and offline driving check.
Unit tests and Python syntax checks do not prove on-device map coverage or
download service availability.

## Local display preview

```sh
node dragonpilot/dashy/preview_road_info.cjs
```

Open http://127.0.0.1:8765 and keep the page in the foreground for 32 seconds.
The mock WebSocket sequence covers highway/city limits, an unknown limit,
cleared data, restored data and disconnect expiry. The page also has manual
scenario buttons. Manual scenarios override the automatic sequence; reload to
restart the automatic checks. It uses the real HUD JS/CSS with a schematic
background, not the complete Tesla theme or the native DP settings panel.
No mapd binary is executed, no maps are downloaded and no GPS is transmitted.

```sh
python3 -m unittest dragonpilot/dashy/test_road_info.py dragonpilot/dashy/test_tdx.py -v
node --test dragonpilot/dashy/test_road_info_hud.cjs dragonpilot/dashy/test_traffic_hud.cjs
```

## Native c3x HUD

The shared switch also enables the native c3x display. Road names are centered
at the top; the speed-limit sign sits beside the cruise set-speed box. Metric
uses a Vienna red-ring sign; imperial uses a MUTCD SPEED LIMIT sign and mph.
These drawing methods and positions are adapted from Sunnypilot commit
`6f7a2c94c10dffcf9a83b120ab1e6985e4496868`:

- https://github.com/sunnypilot/sunnypilot/blob/6f7a2c94c10dffcf9a83b120ab1e6985e4496868/openpilot/selfdrive/ui/sunnypilot/onroad/road_name.py
- https://github.com/sunnypilot/sunnypilot/blob/6f7a2c94c10dffcf9a83b120ab1e6985e4496868/openpilot/selfdrive/ui/sunnypilot/onroad/speed_limit.py

Both consumers use `dragonpilot/mapd/road_info.py` for freshness and availability
checks. The native HUD additionally requires GPS and car data from the current
onroad session and polls map outputs once per second. Unknown limits hide the
sign while retaining a known road name. No speed-limit controller, warning,
offset or cruise adjustment is imported. The mapd NextMapSpeedLimit output
is shown below the native speed-limit sign as AHEAD, the upcoming limit and
route distance, using Sunnypilot's layout and distance formatting. Dashy shows
the same preview beneath its sign. Unknown, unchanged, malformed or stale
previews hide independently of current road information. TDX remains separate.
The native c3x rendering still requires device validation.

```sh
python3 -m unittest dragonpilot/dashy/test_native_road_info.py dragonpilot/dashy/test_road_info.py dragonpilot/dashy/test_tdx.py
```
