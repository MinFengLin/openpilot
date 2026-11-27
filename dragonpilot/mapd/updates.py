"""Check the map archives used by the pinned mapd downloader."""
import json
import math
from email.utils import parsedate_to_datetime
from pathlib import Path

import requests

ARCHIVE_ROOT = 'https://map-data.pfeifer.dev/offline'
# Bounds and archive grid from pfeiferj/mapd v1.12.0 (MIT, see UPSTREAM_LICENSE).
ARCHIVE_DEGREES = 2


def archive_urls(selection):
    bounds = json.loads(Path(__file__).with_name('region_bounds.json').read_text())
    south, west, north, east = bounds[selection]
    for lat in range(math.floor(south / ARCHIVE_DEGREES) * ARCHIVE_DEGREES,
                     math.ceil(north / ARCHIVE_DEGREES) * ARCHIVE_DEGREES, ARCHIVE_DEGREES):
        for lon in range(math.floor(west / ARCHIVE_DEGREES) * ARCHIVE_DEGREES,
                         math.ceil(east / ARCHIVE_DEGREES) * ARCHIVE_DEGREES, ARCHIVE_DEGREES):
            yield f'{ARCHIVE_ROOT}/{lat}/{lon}.tar.gz'


def check_update(selection, installed_at, offroad):
    if type(installed_at) is not int or installed_at <= 0:
        return 'unknown'

    incomplete = False
    with requests.Session() as session:
        for url in archive_urls(selection):
            if not offroad():
                return 'cancelled'
            try:
                with session.head(url, timeout=(3, 5)) as response:
                    if response.status_code != 200:
                        incomplete = True
                        continue
                    modified = parsedate_to_datetime(response.headers.get('Last-Modified', ''))
                    if modified.tzinfo is None:
                        incomplete = True
                    elif modified.timestamp() > installed_at:
                        return 'available'
            except (requests.RequestException, ValueError, OverflowError):
                incomplete = True
    return 'unknown' if incomplete else 'current'
