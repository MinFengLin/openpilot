"""Device-local credentials for the TDX basic road-shape endpoint."""
from datetime import datetime, timezone
from email.utils import parsedate_to_datetime

import requests

CREDENTIALS_PARAM = 'dp_tdx_credentials'
TOKEN_URL = 'https://tdx.transportdata.tw/auth/realms/TDXConnect/protocol/openid-connect/token'


class TdxRateLimited(ValueError):
  def __init__(self, retry_after):
    super().__init__('TDX returned HTTP 429')
    try:
      delay = int(retry_after)
    except (ValueError, TypeError):
      try:
        until = parsedate_to_datetime(retry_after)
        delay = (until - datetime.now(timezone.utc)).total_seconds()
      except (ValueError, TypeError, OverflowError):
        delay = 900
    self.retry_seconds = max(60, delay)


def check_rate_limit(response):
  if response.status_code == 429:
    raise TdxRateLimited(response.headers.get('Retry-After'))


def read_credential_fields(params):
  try:
    stored = params.get(CREDENTIALS_PARAM)
  except ValueError:
    return {}
  if not isinstance(stored, dict):
    return {}
  return {field: stored[field] for field in ('client_id', 'client_secret')
          if isinstance(stored.get(field), str) and stored[field]}


def read_credentials(params):
  fields = read_credential_fields(params)
  if 'client_id' in fields and 'client_secret' in fields:
    return fields['client_id'], fields['client_secret']
  return None


def token_form(client_id, client_secret):
  return {'grant_type': 'client_credentials', 'client_id': client_id, 'client_secret': client_secret}


def parse_token(payload):
  token = payload.get('access_token') if isinstance(payload, dict) else None
  if not isinstance(token, str) or not token or any(char.isspace() for char in token):
    raise ValueError('TDX token response has no usable access token')
  expires_in = payload.get('expires_in', 0)
  return token, expires_in if isinstance(expires_in, int) and expires_in > 0 else 0


def request_token(client_id, client_secret, timeout):
  response = requests.post(TOKEN_URL, data=token_form(client_id, client_secret), timeout=timeout, allow_redirects=False)
  check_rate_limit(response)
  if response.status_code != 200:
    raise ValueError(f'TDX token request returned HTTP {response.status_code}')
  return parse_token(response.json())


def access_token(client_id, client_secret, timeout):
  return request_token(client_id, client_secret, timeout)[0]
