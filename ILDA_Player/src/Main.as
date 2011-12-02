﻿package 
{
	import flash.display.Bitmap;
	import flash.display.SimpleButton;
	import flash.display.Sprite;
	import fl.controls.*;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.events.ProgressEvent;
	import flash.geom.Point;
	import flash.net.URLRequest;
	import flash.net.URLStream;
	import flash.text.TextField;
    import flash.text.TextFieldType;
	import flash.utils.ByteArray;
	import flash.utils.getTimer;
	
	/**
	 * ...
	 * @author sweetie
	 */
	public class Main extends Sprite 
	{
		protected var stream:URLStream;
		protected var request:URLRequest;
		protected var nextFrameData:ByteArray;
		
		protected var fpsCount:int;
		protected var fpsTime:int;
		protected var textF:TextField;
		protected var textIn:TextField;
		protected var lastPoint:Point;
		protected var frameCount:uint;
		protected var pointMax:uint;
		
		protected var colorList:Array;
		
		public function Main():void 
		{
			if (stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
		}
		
		private function CreateButton(text:String, width:uint, height:uint):Sprite {
			var button:Sprite = new Sprite();
			button.graphics.lineStyle(1, 0xBBBBBB);
			button.graphics.beginFill(0xEEEEEE);
			button.graphics.drawRoundRect(0, 0, width, height, 5, 5);
			button.graphics.endFill();
			button.mouseChildren = false;
			button.buttonMode = true;
			
			var field:TextField = new TextField();
			field.width = button.width;
			field.height = button.height;
			field.htmlText = "<p align='center'><font face='_sans'>" + text + "</span></p>";
			button.addChild(field);
			
			return button
		}
		
		private function init(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);
			
			textF = new TextField();
			textF.textColor = 0x000000;
			addChild(textF);
			
			textIn = new TextField();
			textIn.type = TextFieldType.INPUT;
			textIn.background = true;
			textIn.backgroundColor = 0x000000;
			textIn.textColor = 0xffffff;
			textIn.height = 20;
			textIn.width = 300;
			textIn.x = 20;
			textIn.y = stage.stageHeight - textIn.height * 1.5;
			textIn.text = "http://wordpress.click3.org/ilda/BadApple.ild";
			textIn.text = "http://sweetie089.no-ip.biz/aaa_3peso/PJ_BadApple.ild";
			textIn.text = "http://wordpress.click3.org/ilda/PJ_BadApple5.ild";
			textIn.text = "http://wordpress.click3.org/ilda/new/in_apple.ild";
			textIn.text = "http://wordpress.click3.org/ilda/badapple.ild";
			addChild(textIn);

			colorList = [
				[   0,   0,   0 ], // Black/blanked (fixed)
				[ 255, 255, 255 ], // White (fixed)
				[ 255,   0,   0 ],  // Red (fixed)
				[ 255, 255,   0 ],  // Yellow (fixed)
				[   0, 255,   0 ],  // Green (fixed)
				[   0, 255, 255 ],  // Cyan (fixed)
				[   0,   0, 255 ],  // Blue (fixed)
				[ 255,   0, 255 ],  // Magenta (fixed)
				[ 255, 128, 128 ],  // Light red
				[ 255, 140, 128 ],
				[ 255, 151, 128 ],
				[ 255, 163, 128 ],
				[ 255, 174, 128 ],
				[ 255, 186, 128 ],
				[ 255, 197, 128 ],
				[ 255, 209, 128 ],
				[ 255, 220, 128 ],
				[ 255, 232, 128 ],
				[ 255, 243, 128 ],
				[ 255, 255, 128 ], // Light yellow
				[ 243, 255, 128 ],
				[ 232, 255, 128 ],
				[ 220, 255, 128 ],
				[ 209, 255, 128 ],
				[ 197, 255, 128 ],
				[ 186, 255, 128 ],
				[ 174, 255, 128 ],
				[ 163, 255, 128 ],
				[ 151, 255, 128 ],
				[ 140, 255, 128 ],
				[ 128, 255, 128 ], // Light green
				[ 128, 255, 140 ],
				[ 128, 255, 151 ],
				[ 128, 255, 163 ],
				[ 128, 255, 174 ],
				[ 128, 255, 186 ],
				[ 128, 255, 197 ],
				[ 128, 255, 209 ],
				[ 128, 255, 220 ],
				[ 128, 255, 232 ],
				[ 128, 255, 243 ],
				[ 128, 255, 255 ], // Light cyan
				[ 128, 243, 255 ],
				[ 128, 232, 255 ],
				[ 128, 220, 255 ],
				[ 128, 209, 255 ],
				[ 128, 197, 255 ],
				[ 128, 186, 255 ],
				[ 128, 174, 255 ],
				[ 128, 163, 255 ],
				[ 128, 151, 255 ],
				[ 128, 140, 255 ],
				[ 128, 128, 255 ], // Light blue
				[ 140, 128, 255 ],
				[ 151, 128, 255 ],
				[ 163, 128, 255 ],
				[ 174, 128, 255 ],
				[ 186, 128, 255 ],
				[ 197, 128, 255 ],
				[ 209, 128, 255 ],
				[ 220, 128, 255 ],
				[ 232, 128, 255 ],
				[ 243, 128, 255 ],
				[ 255, 128, 255 ], // Light magenta
				[ 255, 128, 243 ],
				[ 255, 128, 232 ],
				[ 255, 128, 220 ],
				[ 255, 128, 209 ],
				[ 255, 128, 197 ],
				[ 255, 128, 186 ],
				[ 255, 128, 174 ],
				[ 255, 128, 163 ],
				[ 255, 128, 151 ],
				[ 255, 128, 140 ],
				[ 255,   0,   0 ], // Red (cycleable)
				[ 255,  23,   0 ],
				[ 255,  46,   0 ],
				[ 255,  70,   0 ],
				[ 255,  93,   0 ],
				[ 255, 116,   0 ],
				[ 255, 139,   0 ],
				[ 255, 162,   0 ],
				[ 255, 185,   0 ],
				[ 255, 209,   0 ],
				[ 255, 232,   0 ],
				[ 255, 255,   0 ], //Yellow (cycleable)
				[ 232, 255,   0 ],
				[ 209, 255,   0 ],
				[ 185, 255,   0 ],
				[ 162, 255,   0 ],
				[ 139, 255,   0 ],
				[ 116, 255,   0 ],
				[  93, 255,   0 ],
				[  70, 255,   0 ],
				[  46, 255,   0 ],
				[  23, 255,   0 ],
				[   0, 255,   0 ], // Green (cycleable)
				[   0, 255,  23 ],
				[   0, 255,  46 ],
				[   0, 255,  70 ],
				[   0, 255,  93 ],
				[   0, 255, 116 ],
				[   0, 255, 139 ],
				[   0, 255, 162 ],
				[   0, 255, 185 ],
				[   0, 255, 209 ],
				[   0, 255, 232 ],
				[   0, 255, 255 ], // Cyan (cycleable)
				[   0, 232, 255 ],
				[   0, 209, 255 ],
				[   0, 185, 255 ],
				[   0, 162, 255 ],
				[   0, 139, 255 ],
				[   0, 116, 255 ],
				[   0,  93, 255 ],
				[   0,  70, 255 ],
				[   0,  46, 255 ],
				[   0,  23, 255 ],
				[   0,   0, 255 ], // Blue (cycleable)
				[  23,   0, 255 ],
				[  46,   0, 255 ],
				[  70,   0, 255 ],
				[  93,   0, 255 ],
				[ 116,   0, 255 ],
				[ 139,   0, 255 ],
				[ 162,   0, 255 ],
				[ 185,   0, 255 ],
				[ 209,   0, 255 ],
				[ 232,   0, 255 ],
				[ 255,   0, 255 ], // Magenta (cycleable)
				[ 255,   0, 232 ],
				[ 255,   0, 209 ],
				[ 255,   0, 185 ],
				[ 255,   0, 162 ],
				[ 255,   0, 139 ],
				[ 255,   0, 116 ],
				[ 255,   0,  93 ],
				[ 255,   0,  70 ],
				[ 255,   0,  46 ],
				[ 255,   0,  23 ],
				[ 128,   0,   0 ], // Dark red
				[ 128,  12,   0 ],
				[ 128,  23,   0 ],
				[ 128,  35,   0 ],
				[ 128,  47,   0 ],
				[ 128,  58,   0 ],
				[ 128,  70,   0 ],
				[ 128,  81,   0 ],
				[ 128,  93,   0 ],
				[ 128, 105,   0 ],
				[ 128, 116,   0 ],
				[ 128, 128,   0 ], // Dark yellow
				[ 116, 128,   0 ],
				[ 105, 128,   0 ],
				[  93, 128,   0 ],
				[  81, 128,   0 ],
				[  70, 128,   0 ],
				[  58, 128,   0 ],
				[  47, 128,   0 ],
				[  35, 128,   0 ],
				[  23, 128,   0 ],
				[  12, 128,   0 ],
				[   0, 128,   0 ], // Dark green
				[   0, 128,  12 ],
				[   0, 128,  23 ],
				[   0, 128,  35 ],
				[   0, 128,  47 ],
				[   0, 128,  58 ],
				[   0, 128,  70 ],
				[   0, 128,  81 ],
				[   0, 128,  93 ],
				[   0, 128, 105 ],
				[   0, 128, 116 ],
				[   0, 128, 128 ], // Dark cyan
				[   0, 116, 128 ],
				[   0, 105, 128 ],
				[   0,  93, 128 ],
				[   0,  81, 128 ],
				[   0,  70, 128 ],
				[   0,  58, 128 ],
				[   0,  47, 128 ],
				[   0,  35, 128 ],
				[   0,  23, 128 ],
				[   0,  12, 128 ],
				[   0,   0, 128 ], // Dark blue
				[  12,   0, 128 ],
				[  23,   0, 128 ],
				[  35,   0, 128 ],
				[  47,   0, 128 ],
				[  58,   0, 128 ],
				[  70,   0, 128 ],
				[  81,   0, 128 ],
				[  93,   0, 128 ],
				[ 105,   0, 128 ],
				[ 116,   0, 128 ],
				[ 128,   0, 128 ], // Dark magenta
				[ 128,   0, 116 ],
				[ 128,   0, 105 ],
				[ 128,   0,  93 ],
				[ 128,   0,  81 ],
				[ 128,   0,  70 ],
				[ 128,   0,  58 ],
				[ 128,   0,  47 ],
				[ 128,   0,  35 ],
				[ 128,   0,  23 ],
				[ 128,   0,  12 ],
				[ 255, 192, 192 ], // Very light red
				[ 255,  64,  64 ], // Light-medium red
				[ 192,   0,   0 ], // Medium-dark red
				[  64,   0,   0 ], // Very dark red
				[ 255, 255, 192 ], // Very light yellow
				[ 255, 255,  64 ], // Light-medium yellow
				[ 192, 192,   0 ], // Medium-dark yellow
				[  64,  64,   0 ], // Very dark yellow
				[ 192, 255, 192 ], // Very light green
				[  64, 255,  64 ], // Light-medium green
				[   0, 192,   0 ], // Medium-dark green
				[   0,  64,   0 ], // Very dark green
				[ 192, 255, 255 ], // Very light cyan
				[  64, 255, 255 ], // Light-medium cyan
				[   0, 192, 192 ], // Medium-dark cyan
				[   0,  64,  64 ], // Very dark cyan
				[ 192, 192, 255 ], // Very light blue
				[  64,  64, 255 ], // Light-medium blue
				[   0,   0, 192 ], // Medium-dark blue
				[   0,   0,  64 ], // Very dark blue
				[ 255, 192, 255 ], // Very light magenta
				[ 255,  64, 255 ], // Light-medium magenta
				[ 192,   0, 192 ], // Medium-dark magenta
				[  64,   0,  64 ], // Very dark magenta
				[ 255,  96,  96 ], // Medium skin tone
				[ 255, 255, 255 ], // White (cycleable)
				[ 245, 245, 245 ],
				[ 235, 235, 235 ], 
				[ 224, 224, 224 ], // Very light gray (7/8 intensity)
				[ 213, 213, 213 ],
				[ 203, 203, 203 ],
				[ 192, 192, 192 ], // Light gray (3/4 intensity)
				[ 181, 181, 181 ],
				[ 171, 171, 171 ],
				[ 160, 160, 160 ], // Medium-light gray (5/8 int.)
				[ 149, 149, 149 ],
				[ 139, 139, 139 ],
				[ 128, 128, 128 ], // Medium gray (1/2 intensity)
				[ 117, 117, 117 ],
				[ 107, 107, 107 ],
				[  96,  96,  96 ], // Medium-dark gray (3/8 int.)
				[  85,  85,  85 ],
				[  75,  75,  75 ],
				[  64,  64,  64 ], // Dark gray (1/4 intensity)
				[  53,  53,  53 ],
				[  43,  43,  43 ],
				[  32,  32,  32 ], // Very dark gray (1/8 intensity)
				[  21,  21,  21 ],
				[  11,  11,  11 ],
				[   0,   0,   0 ] // Black
			];
			
			var submit:Sprite = CreateButton("再生", 60, textIn.height);
			submit.x = textIn.x + textIn.width + 10;
			submit.y = textIn.y;
			submit.addEventListener(MouseEvent.CLICK, OnSubmitButton);
			addChild(submit);
			
			var stop:Sprite = CreateButton("停止", 60, submit.height);
			stop.x = submit.x + submit.width + 10;
			stop.y = submit.y;
			stop.addEventListener(MouseEvent.CLICK, OnStopButton);
			addChild(stop);
		
			fpsTime = getTimer();
			fpsCount = 0;
		}
		
		protected function OnSubmitButton(e:MouseEvent):void {
			trace("OnSubmitButton");
			if(request == null || request.url != textIn.text){
				StartRequest(textIn.text);
			} else {
				OnStartButton(e);
			}
		}
		
		private function StartRequest(url:String):void {
			trace("StartRequest");
			frameCount = 0;
			pointMax = 0;
			request = new URLRequest(url);
			stream = new URLStream();
			stream.load(request);
			addEventListener(Event.ENTER_FRAME, EnterFrame);
		}
		
		private function EnterFrame(e:Event):void {
			Check();
		}
		
		private function Check():void {
			if (stream == null) {
				return;
			}
			if (nextFrameData == null) {
				if (stream.bytesAvailable < 12) {
					return;
				}
				nextFrameData = new ByteArray();
				stream.readBytes(nextFrameData, 0, 12);
			}
			nextFrameData.position = 0;
			if (nextFrameData.readUTFBytes(4) != "ILDA") {
				nextFrameData.position -= 4;
				trace("Identity Not ILDA:", nextFrameData.readUTFBytes(4));
				return;
			}
			var FormatID:uint = nextFrameData.readUnsignedInt();
			var ID3Flag:Boolean = true;
			if (FormatID == 3) {
				var i:uint = 0;
				while (i < 4) {
					var byte:int = nextFrameData.readUnsignedByte();
					ID3Flag = ID3Flag && (0x20 <= byte && byte <= 0x79);
					i++;
				}
				nextFrameData.position = 8;
			}
			
			var length:uint;
			switch(FormatID) {
				default:
				case 3: {
					if (FormatID != 3 || !ID3Flag) {
						length = 3 * nextFrameData.readUnsignedInt();
						break;
					}
				}
				case 0:
				case 1:
				case 2: {
					if (nextFrameData.length == 12) {
						if(stream.bytesAvailable < 20) {
							return;
						} else {
							stream.readBytes(nextFrameData, nextFrameData.length, 20);
						}
					}
					var sizeList:Array = new Array(8, 6, 3, 3);
					nextFrameData.position = 24;
					length = sizeList[FormatID] * nextFrameData.readUnsignedShort();
					break;
				}
			}
			
			if (stream.bytesAvailable < length) {
				return;
			}
			if(length > 0){
				stream.readBytes(nextFrameData, nextFrameData.length, length);
			}
			nextFrameData.position = 0;
			Render(nextFrameData);
			nextFrameData = null;
		}
		
		protected function Render(data:ByteArray):void {
			data.position = 4;
			var FormatID:uint = data.readUnsignedInt();
			data.position = 0;
			if (FormatID == 0) {
				RenderImage(data,false);
			} else if (FormatID == 1) {
				RenderImage(data,true);
			}
		}
		
		protected function GetColor(colorID:uint):uint {
			var color:Array = this.colorList[colorID];
			var result:uint = color[0] * 0x10000 + color[1] * 0x100 + color[2];
			return result;
		}
		
		protected function RenderImage(data:ByteArray, IsXYOnle:Boolean):void {
			graphics.clear();
			graphics.lineStyle(1, 0xFFFFFF);
			data.position = 24;
			var pointNum:uint = data.readUnsignedShort();
			var pointTotal:uint = pointNum;
			var disMax:uint = 0;
			var prev:Point = new Point();
			var light:Boolean = false;
			var colorID:uint;
			if (lastPoint != null) {
				graphics.moveTo(lastPoint.x, lastPoint.y);
				prev.x = lastPoint.x;
				prev.y = lastPoint.y;
			}
			data.position = 32;
			while (pointNum > 0) {
				var x:int = (data.readShort() + 32767) / 180;
				var y:int = (data.readShort() * -1 + 32767) / 180;
				var flags:int = data.readUnsignedByte();
				if (!IsXYOnle) {
					data.position += 2;
				}
				if ((flags & 0x3F) != 0) {
					trace("flags error",flags);
				}
				light = ((flags & 0x40) == 0);
				colorID = data.readUnsignedByte();
				graphics.lineStyle(1, GetColor(colorID));
				if (light) {
					graphics.lineTo( x, y);
				} else {
					graphics.moveTo( x, y);
				}
				{
					var a:int = Math.abs(x - prev.x);
					var b:int = Math.abs(y - prev.y);
					var dis:uint = Math.max(a, b);
					//trace(dis);
					if (dis > disMax) {
						disMax = dis;
					}
				}
				prev.x = x;
				prev.y = y;
				pointNum--;
			}
			if (pointMax < pointTotal) {
				pointMax = pointTotal;
				//trace("frame:", frameCount, " max is ", pointMax);
			}
			trace(pointTotal);
			//trace("disMax",disMax * 180);
			lastPoint = prev;
			//trace(lastPoint);
			frameCount++;
			//trace("frame:",frameCount);
			fpsCount++;
			if (getTimer()-fpsTime>=1000) {
				textF.text = fpsCount + "fps";
				fpsTime = getTimer();
				fpsCount = 0;
			}
		}
		
		protected function OnStopButton(e:MouseEvent):void {
			removeEventListener(Event.ENTER_FRAME, EnterFrame);
		}
		protected function OnStartButton(e:MouseEvent):void {
			removeEventListener(Event.ENTER_FRAME, EnterFrame);
			addEventListener(Event.ENTER_FRAME, EnterFrame);
		}
		
	}
}
