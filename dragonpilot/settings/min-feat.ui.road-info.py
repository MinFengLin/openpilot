from dragonpilot.settings import tr

ITEMS = [
  {
    "section": "UI",
    "key": "dp_dashy_road_info",
    "type": "toggle_item",
    "title": lambda: tr("車機與 Dashy 道路限速與名稱"),
    "description": lambda: tr("使用與 Sunnypilot 相同的 mapd 離線道路資料，在車機顯示限速牌與頂端道路名稱，並在 Dashy HUD 左上角顯示。請先到本頁下方的離線地圖選擇國家並下載資料；只顯示資訊，不調整車速。資料來源：© OpenStreetMap contributors，ODbL。"),
    "flags": "PERSISTENT", "param_type": "BOOL", "default": "0",
  },
  {"key": "dp_mapd_country", "flags": "PERSISTENT", "param_type": "STRING", "default": "TW"},
  {"key": "dp_mapd_state", "flags": "PERSISTENT", "param_type": "STRING", "default": ""},
  {"key": "dp_mapd_status", "flags": "PERSISTENT", "param_type": "STRING", "default": "尚未下載圖資"},
  {"key": "dp_mapd_installed_regions", "flags": "PERSISTENT", "param_type": "JSON", "default": "{}"},
  {"key": "dp_mapd_request", "flags": "CLEAR_ON_MANAGER_START", "param_type": "STRING", "default": ""},
  {"key": "dp_mapd_active", "flags": "CLEAR_ON_MANAGER_START", "param_type": "BOOL", "default": "0"},
  {"key": "dp_mapd_busy", "flags": "CLEAR_ON_MANAGER_START", "param_type": "BOOL", "default": "0"},
  {"key": "OSMDownloadLocations", "flags": "CLEAR_ON_MANAGER_START", "param_type": "JSON"},
  {"key": "OSMDownloadProgress", "flags": "PERSISTENT", "param_type": "JSON"},
  {"key": "MapSpeedLimit", "flags": "CLEAR_ON_MANAGER_START", "param_type": "FLOAT"},
  {"key": "RoadName", "flags": "CLEAR_ON_MANAGER_START", "param_type": "STRING"},
  # Used only as mapd's final startup-reset marker, not as a control target.
  {"key": "MapTargetVelocities", "flags": "CLEAR_ON_MANAGER_START", "param_type": "JSON"},
]
