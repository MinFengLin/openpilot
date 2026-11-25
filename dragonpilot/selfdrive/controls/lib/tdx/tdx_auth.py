"""Device-local credentials for the TDX basic road-shape endpoint."""
import requests

CREDENTIALS_PARAM = 'dp_tdx_credentials'
TOKEN_URL = 'https://tdx.transportdata.tw/auth/realms/TDXConnect/protocol/openid-connect/token'


def read_credentials(params):
  try:
    stored = params.get(CREDENTIALS_PARAM)
  except ValueError:
    return None
  if not isinstance(stored, dict):
    return None
  client_id = stored.get('client_id')
  client_secret = stored.get('client_secret')
  if isinstance(client_id, str) and isinstance(client_secret, str) and client_id and client_secret:
    return client_id, client_secret
  return None


def access_token(client_id, client_secret, timeout):
  response = requests.post(TOKEN_URL, data={
    'grant_type': 'client_credentials',
    'client_id': client_id,
    'client_secret': client_secret,
  }, timeout=timeout, allow_redirects=False)
  if response.status_code != 200:
    raise ValueError(f'TDX token request returned HTTP {response.status_code}')
  payload = response.json()
  token = payload.get('access_token') if isinstance(payload, dict) else None
  if not isinstance(token, str) or not token or any(char.isspace() for char in token):
    raise ValueError('TDX token response has no usable access token')
  return token
