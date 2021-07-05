from django import forms

from crispy_forms.helper import FormHelper
from crispy_forms.layout import Submit

from .models import ScoreboardModel


class GameForm(forms.ModelForm):

    def __init__(self, *args, **kwargs):
        super(GameForm, self).__init__(*args, **kwargs)
        self.fields['name'].label = 'Nombre:'
        self.helper = FormHelper()
        self.helper.form_id = 'id-exampleForm'
        self.helper.form_class = 'blueForms'
        self.helper.form_method = 'post'
        self.helper.form_action = 'index'

        self.helper.add_input(
            Submit('submit', 'Jugar!', css_class="float-right")
        )

    class Meta:
        model = ScoreboardModel
        fields = [
            'name',
        ]
