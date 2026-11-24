from dragonpilot.settings import tr

ITEMS = [
  {
    "section": "Longitudinal",
    "key": "dp_lon_custom_stop_distance_enabled",
    "type": "toggle_item",
    "title": lambda: tr("自訂車距"),
    "description": lambda: tr("啟用後使用自訂停止距離，取代所有駕駛人格的停止距離設定。"),
    "condition": "openpilotLongitudinalControl",
    "flags": "PERSISTENT",
    "param_type": "BOOL",
    "default": "0",
  },
  {
    "section": "Longitudinal",
    "key": "dp_lon_custom_stop_distance",
    "type": "double_spin_button_item",
    "title": lambda: tr("自訂停止距離"),
    "description": lambda: tr("設定 MPC 期望停止距離（3.5 至 6.0 公尺），實際停車間距會受到路況與其他控制條件影響。"),
    "default": "4.0",
    "min_val": 3.5,
    "max_val": 6.0,
    "step": 0.1,
    "suffix": lambda: tr("m"),
    "condition": "openpilotLongitudinalControl",
    "depends_on": "dp_lon_custom_stop_distance_enabled == 1",
    "flags": "PERSISTENT",
    "param_type": "FLOAT",
  },
]
