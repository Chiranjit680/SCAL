from django.db import models

class Angle(models.Model):
    curr_angle = models.FloatField(default=0.0)
    def __str__(self):
        return f"Current angle: {self.curr_angle}"