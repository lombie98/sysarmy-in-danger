import os
from datetime import datetime

from django.views.generic import TemplateView
from django.views.generic.edit import CreateView
from django.views.generic.list import ListView
from django.shortcuts import redirect

from .forms import GameForm
from .models import ScoreboardModel


class IndexView(CreateView):
    template_name = 'webapp/index.html'
    success_url = '/webapp/game'
    form_class = GameForm

    def get_context_data(self, **kwargs):
        hostname = os.uname().nodename
        context = super().get_context_data(**kwargs)
        context.update({
            'hostname': hostname,
        })
        return context

    def post(self, request, **kwargs):
        form = self.get_form()
        game = form.save(commit=False)
        game.date = datetime.now()
        game.save()
        return redirect(self.success_url)


class Thanks(TemplateView):
    template_name = 'webapp/thanks.html'


class ScoreboardView(ListView):
    template_name = 'webapp/scoreboard.html'
    model = ScoreboardModel


class GameView(TemplateView):
    template_name = 'webapp/game.html'
