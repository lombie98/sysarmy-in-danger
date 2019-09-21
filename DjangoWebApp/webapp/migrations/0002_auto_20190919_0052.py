# Generated by Django 2.2.5 on 2019-09-19 00:52

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0001_initial'),
    ]

    operations = [
        migrations.CreateModel(
            name='ScoreboardModel',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=255)),
                ('date', models.DateTimeField()),
                ('game_time', models.DateTimeField()),
            ],
        ),
        migrations.DeleteModel(
            name='UserExperienceModel',
        ),
    ]