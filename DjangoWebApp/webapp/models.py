from django.db import models


class ScoreboardModel(models.Model):
    name = models.CharField(max_length=255)
    date = models.DateTimeField()
    game_time = models.DateTimeField(null=True, blank=True)

    def __str__(self):
        return self.name
