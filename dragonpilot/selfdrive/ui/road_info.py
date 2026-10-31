"""
Copyright (c) 2021-, Haibin Wen, sunnypilot, and a number of other contributors.

Adapted from Sunnypilot's onroad/road_name.py and onroad/speed_limit.py
at 6f7a2c94c10dffcf9a83b120ab1e6985e4496868, under the MIT License.
See dragonpilot/mapd/SUNNYPILOT_LICENSE.
"""
import time
import pyray as rl

from openpilot.common.params import Params
from openpilot.selfdrive.ui.ui_state import ui_state
from openpilot.system.ui.lib.application import gui_app, FontWeight
from openpilot.system.ui.lib.text_measure import measure_text_cached
from openpilot.system.ui.widgets import Widget
from dragonpilot.mapd.road_info import read_road_info


class Colors:
  WHITE = rl.WHITE
  BLACK = rl.BLACK
  RED = rl.Color(235, 32, 32, 255)
  GREY = rl.Color(145, 155, 149, 255)
  SUB_BG = rl.Color(0, 0, 0, 180)
  MUTCD_LINES = rl.Color(255, 255, 255, 100)
  DARK_GREY = rl.Color(77, 77, 77, 255)


class OfflineRoadInfoRenderer(Widget):
  def __init__(self):
    super().__init__()
    self.mem = Params('/dev/shm/params')
    self.road_name = ''
    self.limit = ''
    self.ahead_limit = ''
    self.ahead_distance = 0
    self.ahead_rect = None
    self._last_read = -float('inf')
    self.font_bold = gui_app.font(FontWeight.BOLD)
    self.font_demi = gui_app.font(FontWeight.SEMI_BOLD)

  def update(self):
    sm = ui_state.sm
    if (not ui_state.started or sm.recv_frame['carState'] < ui_state.started_frame
            or sm.recv_frame['liveGPS'] < ui_state.started_frame):
      self.road_name = self.limit = self.ahead_limit = ''
      self.ahead_distance = 0
      self._last_read = -float('inf')
      return
    now = time.monotonic()
    if now - self._last_read < 1:
      return
    self._last_read = now
    info = read_road_info(ui_state.params, self.mem, sm)
    self.road_name = info['roadName'] if info else ''
    kph = info['speedLimitKph'] if info else None
    self.limit = str(round(kph if ui_state.is_metric else kph / 1.609344)) if kph else ''
    ahead = info.get('aheadSpeedLimitKph') if info else None
    self.ahead_limit = str(round(ahead if ui_state.is_metric else ahead / 1.609344)) if ahead else ''
    self.ahead_distance = info.get('aheadDistanceM', 0) if info else 0

  def _render(self, rect: rl.Rectangle):
    # Local import avoids a cycle with HudRenderer's integration below.
    from openpilot.selfdrive.ui.onroad.hud_renderer import UI_CONFIG
    self.ahead_rect = None
    if self.road_name:
      self._draw_road_name(rect)
    if not self.limit:
      return
    width = UI_CONFIG.set_speed_width_metric if ui_state.is_metric else UI_CONFIG.set_speed_width_imperial
    x = rect.x + 60 + width + 30 - 6
    y = rect.y + 45 - 6
    sign_rect = rl.Rectangle(x, y, width, UI_CONFIG.set_speed_height + 12)
    if ui_state.is_metric:
      self._render_vienna(sign_rect, self.limit, '', Colors.BLACK, True)
    else:
      self._render_mutcd(sign_rect, self.limit, '', Colors.BLACK, True)
    if self.ahead_limit:
      self._draw_ahead_info(sign_rect)

  def _draw_road_name(self, rect):
    text = self.road_name
    text_size = measure_text_cached(self.font_demi, text, 46)

    padding = 40
    rect_width = max(200, min(text_size.x + padding, rect.width - 40))

    road_rect = rl.Rectangle(rect.x + rect.width / 2 - rect_width / 2, rect.y - 4, rect_width, 60)

    rl.draw_rectangle_rounded(road_rect, 0.2, 10, rl.Color(0, 0, 0, 120))

    max_text_width = road_rect.width - 20
    if text_size.x > max_text_width:
      while text_size.x > max_text_width and len(text) > 3:
        text = text[:-1]
        text_size = measure_text_cached(self.font_demi, text + "...", 46)
      text = text + "..."

    sz = measure_text_cached(self.font_demi, text, 46)
    origin = rl.Vector2(road_rect.x + road_rect.width / 2 - sz.x / 2, road_rect.y + road_rect.height / 2 - sz.y / 2)
    rl.draw_text_ex(self.font_demi, text, origin, 46, 0, rl.Color(255, 255, 255, 200))

  @staticmethod
  def _draw_text_centered(font, text, size, pos_center, color):
    sz = measure_text_cached(font, text, size)
    rl.draw_text_ex(font, text, rl.Vector2(pos_center.x - sz.x / 2, pos_center.y - sz.y / 2), size, 0, color)

  def _render_vienna(self, rect, val, sub, color, has_limit, alpha=1.0):
    center = rl.Vector2(rect.x + rect.width / 2, rect.y + rect.height / 2)
    radius = (rect.width + 18) / 2

    white = rl.color_alpha(Colors.WHITE, alpha)
    red = rl.color_alpha(Colors.RED, alpha)
    black = rl.color_alpha(Colors.BLACK, alpha)
    dark_grey = rl.color_alpha(Colors.DARK_GREY, alpha)
    text_color = rl.color_alpha(color, alpha)

    rl.draw_circle_v(center, radius, white)
    rl.draw_ring(center, radius * 0.75, radius, 0, 360, 36, red)

    font_size = 70 if len(val) >= 3 else 85
    self._draw_text_centered(self.font_bold, val, font_size, center, text_color)

    if sub and has_limit:
      s_radius = radius * 0.4
      s_center = rl.Vector2(rect.x + rect.width - s_radius / 2, rect.y + s_radius / 2)

      rl.draw_circle_v(s_center, s_radius, black)
      rl.draw_ring(s_center, s_radius - 3, s_radius, 0, 360, 36, dark_grey)

      font_scale = 0.5 if len(sub) < 3 else 0.45
      self._draw_text_centered(self.font_bold, sub, int(s_radius * 2 * font_scale), s_center, white)

  def _render_mutcd(self, rect, val, sub, color, has_limit, alpha=1.0):
    white = rl.color_alpha(Colors.WHITE, alpha)
    black = rl.color_alpha(Colors.BLACK, alpha)
    dark_grey = rl.color_alpha(Colors.DARK_GREY, alpha)
    text_color = rl.color_alpha(color, alpha)

    rl.draw_rectangle_rounded(rect, 0.35, 10, white)

    inner = rl.Rectangle(rect.x + 10, rect.y + 10, rect.width - 20, rect.height - 20)
    outer_radius = 0.35 * rect.width / 2.0
    inner_radius = outer_radius - 10.0
    inner_roundness = inner_radius / (inner.width / 2.0)

    rl.draw_rectangle_rounded_lines_ex(inner, inner_roundness, 10, 4, black)

    self._draw_text_centered(self.font_demi, "SPEED", 40, rl.Vector2(rect.x + rect.width / 2, rect.y + 40), black)
    self._draw_text_centered(self.font_demi, "LIMIT", 40, rl.Vector2(rect.x + rect.width / 2, rect.y + 80), black)
    self._draw_text_centered(self.font_bold, val, 90, rl.Vector2(rect.x + rect.width / 2, rect.y + 150), text_color)

    if sub and has_limit:
      box_sz = rect.width * 0.3
      overlap = box_sz * 0.2
      s_rect = rl.Rectangle(rect.x + rect.width - box_sz / 1.5 + overlap, rect.y - box_sz / 1.25 + overlap, box_sz, box_sz)

      rl.draw_rectangle_rounded(s_rect, 0.35, 10, black)
      rl.draw_rectangle_rounded_lines_ex(s_rect, 0.35, 10, 6, dark_grey)

      f_scale = 0.6 if len(sub) < 3 else 0.475
      self._draw_text_centered(self.font_bold, sub, int(box_sz * f_scale), rl.Vector2(s_rect.x + box_sz / 2, s_rect.y + box_sz / 2), white)

  def _draw_ahead_info(self, sign_rect):
    rect = rl.Rectangle(sign_rect.x + (sign_rect.width - 170) / 2, sign_rect.y + sign_rect.height + 10, 170, 160)
    self.ahead_rect = rect
    rl.draw_rectangle_rounded(rect, 0.35, 10, Colors.SUB_BG)
    rl.draw_rectangle_rounded_lines_ex(rect, 0.35, 10, 3, Colors.MUTCD_LINES)

    mid_x = rect.x + rect.width / 2
    self._draw_text_centered(self.font_demi, "AHEAD", 40, rl.Vector2(mid_x, rect.y + 28), Colors.GREY)
    self._draw_text_centered(self.font_bold, self.ahead_limit, 70, rl.Vector2(mid_x, rect.y + 82), Colors.WHITE)
    self._draw_text_centered(self.font_demi, self._format_dist(self.ahead_distance), 36, rl.Vector2(mid_x, rect.y + 134), Colors.GREY)

  @staticmethod
  def _format_dist(d):
    # metric
    if ui_state.is_metric:
      if d < 50:
        return "Near"

      if d >= 1000:
        return f"{d / 1000:.1f} km"

      d_rounded = round(d, -1) if d < 200 else round(d, -2)
      return f"{int(d_rounded)} m"

    # imperial
    d_ft = d * 3.28084
    if d_ft < 100:
      return "Near"

    if d_ft >= 900:
      return f"{d * 0.000621371:.1f} mi"

    if d_ft < 500:
      return f"{int(round(d_ft / 50) * 50)} ft"

    return f"{int(round(d_ft / 100) * 100)} ft"
