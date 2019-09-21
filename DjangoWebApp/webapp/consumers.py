from channels.generic.websocket import AsyncWebsocketConsumer
import json


from .toninas import ToninasGame


class GameConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        await self.accept()

    async def disconnect(self, close_code):
        if hasattr(self, 'GameInstance'):
            self.GameInstance.stop()

    async def receive(self, text_data):
        payload = json.loads(text_data)
        print("Received %s" % payload)
        if not isinstance(payload, dict):
            return
        if payload.get('signal') == 'start':
            options = {
                k: v for k, v in payload.get('value').items()
                if v
            }
            game = ToninasGame(
                **options,
            )
            self.GameInstance = game
            game.start()
            await self.send(json.dumps({
                'signal': 'config',
                'value': game.config,
            }))
            game.run(self)
