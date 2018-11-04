# Generated by Django 2.1.2 on 2018-11-04 05:42

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
        ('invernaderos', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='cultivo',
            name='id_usuario',
            field=models.ForeignKey(default=1, error_messages={'select': 'Debe seleccionar uno de la lista'}, help_text='Usuario al que pertenece este cultivo', on_delete=django.db.models.deletion.DO_NOTHING, to=settings.AUTH_USER_MODEL, verbose_name='Usuario'),
            preserve_default=False,
        ),
    ]
