from django.conf import settings
from django.apps import AppConfig


def sget(key, default=False):
    res = hasattr(settings, key) and getattr(settings, key) or default
    return res


class WebAppConfig(AppConfig):
    name = 'webapp'
