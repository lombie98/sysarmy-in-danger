from django.urls import path
from django.views.generic import RedirectView

from . import views

app_name = 'webapp'
urlpatterns = [
    path('', RedirectView.as_view(url="index")),
    path('index', views.IndexView.as_view(), name='index'),
    path('thanks', views.Thanks.as_view()),
    path('game', views.GameView.as_view()),
    path('scoreboard', views.ScoreboardView.as_view()),
    path('test', views.GameView.as_view()),
]
