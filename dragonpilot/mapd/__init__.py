"""Display-only adapter for pfeiferj mapd, as used by Sunnypilot.

The region name catalogs come from openpilot-mapd v1.12.0 (MIT).
See UPSTREAM_LICENSE. Road data is © OpenStreetMap contributors, ODbL.
"""
import json
from pathlib import Path
from functools import lru_cache

VERSION = 'v1.12.0'
SHA256 = 'fdb3b49ee19956e6ce09fdc3373cbba557f1263b2180e9f344c1d4053852284b'
URL = f'https://github.com/pfeiferj/mapd/releases/download/{VERSION}/mapd'
ROOT = Path('/data/media/0/osm')
BINARY = ROOT / 'bin' / f'mapd-{VERSION}'


@lru_cache(maxsize=2)
def regions(states=False):
    name = 'us_states_names.json' if states else 'nation_names.json'
    return json.loads(Path(__file__).with_name(name).read_text(encoding='utf-8-sig'))


def download_locations(country, state=''):
    if country not in regions():
        raise ValueError('請先選擇國家')
    if country == 'US' and state:
        if state not in regions(True):
            raise ValueError('無效的美國州別')
        return {'nations': [], 'states': [state]}
    return {'nations': [country], 'states': []}


def region_key(country, state=''):
    return f'US:{state}' if country == 'US' and state else country


def progress_complete(progress):
    return isinstance(progress, dict) and progress.get('total_files', 0) > 0 and (
        progress.get('downloaded_files', 0) >= progress['total_files'])
