"""Device-local credentials for the TDX basic road-shape endpoint."""
import requests

CREDENTIALS_PARAM = 'dp_tdx_credentials'
TOKEN_URL = 'https://tdx.transportdata.tw/auth/realms/TDXConnect/protocol/openid-connect/token'


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
