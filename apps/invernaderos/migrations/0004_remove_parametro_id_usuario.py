# Generated by Django 2.1.2 on 2018-11-04 15:59

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('invernaderos', '0003_parametro_id_usuario'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='parametro',
            name='id_usuario',
        ),
    ]
