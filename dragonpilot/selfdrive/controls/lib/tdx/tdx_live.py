"""Authenticated TDX feeds with one reusable token per credentials pair."""
import time

import requests

from dragonpilot.selfdrive.controls.lib.tdx import freeway_lanes, tdx_auth

BASE_URL = 'https://tdx.transportdata.tw/api/basic'
TRAFFIC = 'v2/Road/Traffic/Live/Freeway'
EVENTS = 'v1/Traffic/RoadEvent/LiveEvent/Freeway'
VD = 'v2/Road/Traffic/Live/VD/Freeway'
HIGHWAY_TRAFFIC = 'v2/Road/Traffic/Live/Highway'
HIGHWAY_EVENTS = 'v1/Traffic/RoadEvent/LiveEvent/Highway'
HIGHWAY_VD = 'v2/Road/Traffic/Live/VD/Highway'
TIMEOUT_S = 5


class TdxLiveClient:
  def __init__(self):
    self._credentials = None
    self._token = None
    self._expires_at = 0.0
    self._retry_at = 0.0

  def cooling_down(self):
    return time.monotonic() < self._retry_at

  def pause(self, rate_limit):
    self._retry_at = max(self._retry_at, time.monotonic() + rate_limit.retry_seconds)

  def prepare(self, credentials):
    if credentials != self._credentials:
      self._credentials = credentials
      self._token = None
    if credentials is None or self.cooling_down():
      return False
    now = time.monotonic()
    if self._token is None or now >= self._expires_at:
      try:
        self._token, expires_in = tdx_auth.request_token(*credentials, timeout=TIMEOUT_S)
      except tdx_auth.TdxRateLimited as e:
        self.pause(e)
        raise
      self._expires_at = now + max(0, expires_in - 15)
    return True

  def fetch(self, path, collection, now, max_age=freeway_lanes.MAX_AGE_S):
    response = requests.get(f'{BASE_URL}/{path}', params={'$format': 'JSON'},
                            headers={'Authorization': f'Bearer {self._token}'}, timeout=TIMEOUT_S, allow_redirects=False)
    try:
      tdx_auth.check_rate_limit(response)
    except tdx_auth.TdxRateLimited as e:
      self.pause(e)
      raise
    if response.status_code == 401:
      self._expires_at = 0.0
    if response.status_code != 200:
      raise ValueError(f'HTTP {response.status_code}')
    data = response.json()
    if not isinstance(data, dict) or not isinstance(data.get(collection), list):
      raise ValueError('unexpected payload')
    if not freeway_lanes.is_fresh_timestamp(data.get('UpdateTime'), now, max_age):
      raise ValueError('stale payload')
    return data
