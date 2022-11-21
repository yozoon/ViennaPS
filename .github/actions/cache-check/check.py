import sys

import requests


def on_error():
    print("cache-hit=false")
    exit(0)


def on_success():
    print("cache-hit=true")
    exit(0)


def main(repository, token, key):
    URL = f"https://api.github.com/repos/{repository}/actions/caches"

    try:
        ret = requests.get(
            URL,
            headers={
                "Accept": "application/vnd.github+json",
                "Authorization": f"Bearer {token}",
            },
            timeout=20,
        )
    except requests.exceptions.Timeout:
        on_error()

    try:
        parsed = ret.json()
    except requests.exceptions.JSONDecodeError:
        on_error()

    if "actions_caches" in parsed:
        cache_list = parsed["actions_caches"]
    else:
        on_error()

    cache_keys = [e["key"] for e in cache_list]
    if len(list(filter(lambda e: e == f"{key}", cache_keys))) > 0:
        on_success()
    else:
        on_error()


if __name__ == "__main__":
    if len(sys.argv) > 3:
        repository = sys.argv[1]
        token = sys.argv[2]
        key = sys.argv[3]
        main(repository, token, key)
    else:
        on_error()
